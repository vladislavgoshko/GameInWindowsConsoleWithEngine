#pragma once
// MiniWaveOutAudio.h
// Ultra-lightweight sound module for Windows games (no external libs).
// - Single-header, self-contained (uses WinMM: waveOut*)
// - Simple mixer: multiple voices, volume, loop
// - 16-bit PCM WAV loader (mono or stereo)
// - Output: 44.1 kHz, stereo by default
//
// Usage:
//   #define MINIAUDIO_IMPLEMENTATION
//   #include "MiniWaveOutAudio.h"
//
//   MiniAudio::Engine audio;                    // start engine (44100 Hz, stereo)
//   MiniAudio::WavData shot; audio.loadWav(L"shot.wav", shot);
//   audio.play(shot, false, 0.8f);              // fire-and-forget
//   ...
//   // on shutdown (or let destructor handle it)
//
// Notes:
//   * Expects 16-bit PCM .wav files. If sample rate differs from engine's, a fast linear resampler is applied.
//   * Thread-safe for play()/stopAll(); minimal locking in the mixer thread.
//   * Link with WinMM: #pragma comment(lib, "Winmm.lib") or add winmm.lib to your linker settings.
//
// Public API (namespace MiniAudio):
//   struct WavData { int sampleRate, channels; std::vector<int16_t> samples; };
//   class Engine {
//       Engine(int sampleRate=44100, int channels=2, int bufferFrames=1024, int numBuffers=4);
//       ~Engine();
//       bool loadWav(const std::wstring& path, WavData& out);  // 16-bit PCM only
//       int  play(const WavData& wav, bool loop=false, float volume=1.0f); // returns voice id
//       void stop(int voiceId);
//       void stopAll();
//       void setMasterVolume(float v); // 0..1
//   };
//
// --------------------------------------------------------------------------------------
#pragma once
#define NOMINMAX
#undef min
#undef max
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <fstream>
#include <cstring>

#pragma comment(lib, "Winmm.lib")
template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v ? hi : v);
}
namespace MiniAudio {

    struct WavData {
        int sampleRate = 0;
        int channels = 0; // 1 or 2
        std::vector<int16_t> samples; // interleaved if stereo
        bool valid() const { return sampleRate > 0 && (channels == 1 || channels == 2) && !samples.empty(); }
    };

    // --- tiny WAV reader (RIFF PCM 16-bit) ---
#pragma pack(push,1)
    struct RiffHeader { char id[4]; uint32_t size; char wave[4]; };
    struct FmtChunk { char id[4]; uint32_t size; uint16_t audioFmt; uint16_t channels; uint32_t sampleRate; uint32_t byteRate; uint16_t blockAlign; uint16_t bitsPerSample; };
    struct ChunkHdr { char id[4]; uint32_t size; };
#pragma pack(pop)

    class Engine {
    public:
        Engine(int sampleRate = 44100, int channels = 2, int bufferFrames = 1024, int numBuffers = 4)
            : m_outRate(sampleRate), m_outCh(clamp(channels, 1, 2)), m_bufFrames(std::max(256, bufferFrames)), m_numBuf(clamp(numBuffers, 2, 8))
        {
            initDevice();
            startThread();
        }

        ~Engine() {
            shutdown();
        }

        bool loadWav(const std::wstring& path, WavData& out) {
            std::ifstream f(path, std::ios::binary);
            if (!f) return false;
            RiffHeader rh{}; f.read(reinterpret_cast<char*>(&rh), sizeof(rh));
            if (std::memcmp(rh.id, "RIFF", 4) != 0 || std::memcmp(rh.wave, "WAVE", 4) != 0) return false;
            FmtChunk fmt{}; bool haveFmt = false; uint32_t dataSize = 0; std::streampos dataPos{};
            while (f && !haveFmt || !dataSize) {
                ChunkHdr ch{}; f.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                if (!f) break;
                if (std::memcmp(ch.id, "fmt ", 4) == 0) {
                    if (ch.size < sizeof(FmtChunk) - 8) return false; // guard
                    f.read(reinterpret_cast<char*>(&fmt.audioFmt), ch.size);
                    haveFmt = true;
                }
                else if (std::memcmp(ch.id, "data", 4) == 0) {
                    dataSize = ch.size; dataPos = f.tellg();
                    f.seekg(ch.size, std::ios::cur);
                }
                else {
                    f.seekg(ch.size, std::ios::cur);
                }
            }
            if (!haveFmt || !dataSize) return false;
            if (fmt.audioFmt != 1 || fmt.bitsPerSample != 16 || (fmt.channels != 1 && fmt.channels != 2)) return false; // PCM 16 only
            out.sampleRate = static_cast<int>(fmt.sampleRate);
            out.channels = static_cast<int>(fmt.channels);
            out.samples.resize(dataSize / 2);
            f.clear();
            f.seekg(dataPos);
            f.read(reinterpret_cast<char*>(out.samples.data()), dataSize);
            return out.valid();
        }

        // returns voice id (>=1) or 0 on failure
        int play(const WavData& wav, bool loop = false, float volume = 1.0f) {
            if (!wav.valid()) return 0;
            std::lock_guard<std::mutex> lk(m_mx);
            int id = ++m_nextVoiceId; if (id == 0) id = ++m_nextVoiceId; // avoid 0
            Voice v; v.id = id; v.wav = &wav; v.loop = loop; v.volume = clamp(volume, 0.0f, 1.0f); v.playing = true; v.pos = 0; v.fpos = 0.0;
            // precompute resample ratio
            v.ratio = (double)wav.sampleRate / (double)m_outRate;
            m_voices.push_back(v);
            m_cv.notify_all();
            return id;
        }

        void stop(int voiceId) {
            std::lock_guard<std::mutex> lk(m_mx);
            for (auto& v : m_voices) if (v.id == voiceId) { v.playing = false; }
        }

        void stopAll() {
            std::lock_guard<std::mutex> lk(m_mx);
            for (auto& v : m_voices) v.playing = false;
        }

        void setMasterVolume(float v) { m_master.store(clamp(v, 0.0f, 1.0f)); }

    private:
        struct Voice {
            int id = 0; const WavData* wav = nullptr; bool loop = false; bool playing = false; float volume = 1.0f; size_t pos = 0; double fpos = 0.0; double ratio = 1.0; // resample position in source frames
        };

        void initDevice() {
            WAVEFORMATEX wfx{}; wfx.wFormatTag = WAVE_FORMAT_PCM; wfx.nChannels = (WORD)m_outCh; wfx.nSamplesPerSec = m_outRate;
            wfx.wBitsPerSample = 16; wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8; wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
            m_event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            MMRESULT res = waveOutOpen(&m_hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)m_event, 0, CALLBACK_EVENT);
            if (res != MMSYSERR_NOERROR) throw std::runtime_error("waveOutOpen failed");
            m_buffers.resize(m_numBuf);
            m_wh.resize(m_numBuf);
            const size_t bytes = m_bufFrames * wfx.nBlockAlign;
            for (int i = 0; i < m_numBuf; ++i) {
                m_buffers[i].resize(bytes / 2); // int16 samples count
                std::memset(&m_wh[i], 0, sizeof(WAVEHDR));
                m_wh[i].lpData = reinterpret_cast<LPSTR>(m_buffers[i].data());
                m_wh[i].dwBufferLength = (DWORD)bytes;
                waveOutPrepareHeader(m_hwo, &m_wh[i], sizeof(WAVEHDR));
            }
        }

        void startThread() {
            m_running.store(true);
            m_thread = std::thread([this] { this->threadFunc(); });
        }

        void shutdown() {
            if (!m_running.load()) return;
            m_running.store(false);
            m_cv.notify_all();
            if (m_event) SetEvent(m_event);
            if (m_thread.joinable()) m_thread.join();

            if (m_hwo) { waveOutReset(m_hwo); }
            for (auto& wh : m_wh) { if (m_hwo && (wh.dwFlags & WHDR_PREPARED)) waveOutUnprepareHeader(m_hwo, &wh, sizeof(WAVEHDR)); }
            if (m_hwo) { waveOutClose(m_hwo); m_hwo = nullptr; }
            if (m_event) { CloseHandle(m_event); m_event = nullptr; }
        }

        void submitIfFree(int i) {
            if (!(m_wh[i].dwFlags & WHDR_INQUEUE)) {
                // Fill buffer i
                mixInto(reinterpret_cast<int16_t*>(m_wh[i].lpData), m_wh[i].dwBufferLength / 2);
                waveOutWrite(m_hwo, &m_wh[i], sizeof(WAVEHDR));
            }
        }

        void threadFunc() {
            // Prime all buffers
            for (int i = 0; i < m_numBuf; ++i) submitIfFree(i);
            int idx = 0;
            while (m_running.load()) {
                // Wait for buffer done or new voice
                HANDLE handles[1] = { m_event };
                WaitForSingleObject(handles[0], 10);
                // Recycle any buffers marked done
                for (int i = 0; i < m_numBuf; ++i) {
                    if (m_wh[i].dwFlags & WHDR_DONE) {
                        m_wh[i].dwFlags &= ~WHDR_DONE; // clear done before resubmit
                        submitIfFree(i);
                    }
                }
                // Also ensure we keep feeding if output device is hungry
                submitIfFree(idx);
                idx = (idx + 1) % m_numBuf;
            }
        }

        void mixInto(int16_t* out, size_t sampleCount) {
            const int ch = m_outCh;
            const float master = m_master.load();
            // Zero output (int16 samples)
            std::memset(out, 0, sampleCount * sizeof(int16_t));

            std::lock_guard<std::mutex> lk(m_mx);
            // remove finished voices
            m_voices.erase(std::remove_if(m_voices.begin(), m_voices.end(), [](const Voice& v) { return !v.playing; }), m_voices.end());
            if (m_voices.empty()) return; // silence

            // 32-bit accumulators per sample
            for (size_t s = 0; s < sampleCount; s += ch) {
                int accL = 0, accR = 0;
                for (auto& v : m_voices) {
                    if (!v.playing) continue;
                    const WavData* w = v.wav; const int wch = w->channels; const size_t wframes = w->samples.size() / (size_t)wch;
                    // Fetch one frame with (very) fast linear resampling to output rate
                    if (v.fpos >= (double)wframes) {
                        if (v.loop) { v.fpos = std::fmod(v.fpos, (double)wframes); }
                        else { v.playing = false; continue; }
                    }
                    size_t i0 = (size_t)v.fpos; size_t i1 = std::min(i0 + 1, wframes - 1);
                    double t = v.fpos - (double)i0;
                    // read samples
                    int s0L = 0, s0R = 0, s1L = 0, s1R = 0;
                    if (wch == 1) {
                        s0L = s0R = w->samples[i0];
                        s1L = s1R = w->samples[i1];
                    }
                    else {
                        s0L = w->samples[i0 * 2 + 0]; s0R = w->samples[i0 * 2 + 1];
                        s1L = w->samples[i1 * 2 + 0]; s1R = w->samples[i1 * 2 + 1];
                    }
                    int L = (int)((1.0 - t) * s0L + t * s1L);
                    int R = (int)((1.0 - t) * s0R + t * s1R);
                    // accumulate with volume and master
                    float vol = v.volume * master;
                    accL += (int)(L * vol);
                    accR += (int)(R * vol);
                    v.fpos += v.ratio; // advance source position by resample ratio
                }
                // clip to int16
                accL = clamp(accL, -32768, 32767);
                accR = clamp(accR, -32768, 32767);
                if (ch == 1) { out[s] = (int16_t)((accL + accR) / 2); }
                else { out[s + 0] = (int16_t)accL; out[s + 1] = (int16_t)accR; }
            }
        }

        // Config
        int m_outRate; int m_outCh; int m_bufFrames; int m_numBuf;
        // Device/buffers
        HWAVEOUT m_hwo = nullptr; HANDLE m_event = nullptr; std::vector<std::vector<int16_t>> m_buffers; std::vector<WAVEHDR> m_wh;

        // Mixer state
        std::atomic<bool> m_running{ false }; std::thread m_thread; std::mutex m_mx; std::condition_variable m_cv; std::vector<Voice> m_voices; std::atomic<float> m_master{ 1.0f }; int m_nextVoiceId = 0;
    };

} // namespace MiniAudio

// ------------------------- Implementation switch ---------------------------
#ifdef MINIAUDIO_IMPLEMENTATION
// (everything is already inline in this header)
#endif // MINIAUDIO_IMPLEMENTATION
