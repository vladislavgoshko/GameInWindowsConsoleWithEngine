#pragma once

#define _WINSOCKAPI_ // Prevent winsock.h
#include <Windows.h>
#include <vector>
#include <string>

struct ColorRGB {
    DWORD Red, Green, Blue;
};

class ConsoleEngine {
public:
    ConsoleEngine();
    void Draw(const CHAR_INFO* screenArray);
    void ChangeTitle(std::string title);
    COORD GetWindowSize();
    void SwitchFullscreen(bool enable);
    void IncreaseScale();
    void DecreaseScale();
    bool IsFullscreen() const;
    void SetColorPallete(std::vector<ColorRGB> colors); // Works only on old terminal

    bool IsFullcreen = false;

    HANDLE handleConsoleOut; // TODO: move to private
    HANDLE handleConsoleIn;
private:
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    SMALL_RECT rect;
};