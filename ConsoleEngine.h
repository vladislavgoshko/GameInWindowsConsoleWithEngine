#ifndef GRAPHICENGINE_H
#define GRAPHICENGINE_H

#define _WINSOCKAPI_ // Prevent winsock.h
#include <Windows.h>
#include <chrono>
#include <vector>
#include <iostream>
#include "IDrawableObject.h"

struct ColorRGB {
    DWORD Red, Green, Blue;
};

class ConsoleEngine {
public:
    bool IsFullcreen = false;

    ConsoleEngine();
    void WriteText(COORD coords, std::string text);
    //void WriteText(COORD coords, std::string text, int textColor, int bgColor);
    void DrawObjects(std::vector<IDrawableObject*>& objects);
    void ChangeTitle(std::string title);
    void WriteDebugInfo(std::string debugInfo);
    COORD GetWindowSize();
    void SwitchFullscreen(bool enable);

    void SetColorPallete(std::vector<ColorRGB> colors); // Works only on old terminal
    void SetConsoleColor(int textColor, int bgColor);
    void ChangeConsoleColor(int bgColor, int textColor); // Works on both versions of terminal

private:
    HANDLE handleConsole;
    wchar_t* screenBuffer;
    WORD consoleColor = 7 << 4 | 12;
    COORD currentSize;
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    wchar_t debugInfo[50] = L"Hello";
    DWORD lpNumberOfCharsWritten;

    bool isFullScreen();
    void DrawPixel(short X, short Y);
    void ClearPixel(short X, short Y);
    float FPS(std::chrono::system_clock::time_point& tp1, std::chrono::system_clock::time_point& tp2);
    void DrawObject(const IDrawableObject& obj);
    void SimulateF11KeyPress();
};

#endif
