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

#pragma pack(push,1)
    struct RiffHeader { char id[4]; uint32_t size; char wave[4]; };
    struct FmtChunk { char id[4]; uint32_t size; uint16_t audioFmt; uint16_t channels; uint32_t sampleRate; uint32_t byteRate; uint16_t blockAlign; uint16_t bitsPerSample; };
    struct ChunkHdr { char id[4]; uint32_t size; };
#pragma pack(pop)

    class Engine {
    public:
        Engine(int sampleRate = 44100, int channels = 2, int bufferFrames = 1024, int numBuffers = 4);
        ~Engine();

        bool loadWav(const std::wstring& path, WavData& out);
        int play(const WavData& wav, bool loop = false, float volume = 1.0f);
        void stop(int voiceId);
        void stopAll();
        void setMasterVolume(float v);

    private:
        struct Voice {
            int id = 0;
            const WavData* wav = nullptr;
            bool loop = false;
            bool playing = false;
            float volume = 1.0f;
            size_t pos = 0;
            double fpos = 0.0;
            double ratio = 1.0;
        };

        void initDevice();
        void startThread();
        void shutdown();
        void submitIfFree(int i);
        void threadFunc();
        void mixInto(int16_t* out, size_t sampleCount);

        // Config
        int m_outRate;
        int m_outCh;
        int m_bufFrames;
        int m_numBuf;

        // Device/buffers
        HWAVEOUT m_hwo = nullptr;
        HANDLE m_event = nullptr;
        std::vector<std::vector<int16_t>> m_buffers;
        std::vector<WAVEHDR> m_wh;

        // Mixer state
        std::atomic<bool> m_running{ false };
        std::thread m_thread;
        std::mutex m_mx;
        std::condition_variable m_cv;
        std::vector<Voice> m_voices;
        std::atomic<float> m_master{ 1.0f };
        int m_nextVoiceId = 0;
    };

} // namespace MiniAudio
