#ifndef GRAPHICENGINE_H
#define GRAPHICENGINE_H

#include <Windows.h>
#include <chrono>
#include <vector>
#include <iostream>
#include "IDrawableObject.h"

struct ColorRGB {
    DWORD Red, Green, Blue;
};

class GraphicEngine {
public:

    GraphicEngine();
    void DrawObjects(std::vector<IDrawableObject*>& objects);
    void ChangeTitle(std::string title);
    void WriteDebugIndo(std::string debugInfo);
    COORD GetWindowSize();

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
    void DrawPixel(wchar_t* screenBuffer, COORD screenSizes, short X, short Y);
    void ClearPixel(wchar_t* screenBuffer, COORD screenSizes, short X, short Y);
    float FPS(std::chrono::system_clock::time_point& tp1, std::chrono::system_clock::time_point& tp2);
    void DrawObject(const IDrawableObject& obj);
};

#endif
