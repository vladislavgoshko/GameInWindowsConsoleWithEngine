#pragma once

#define _WINSOCKAPI_ // Prevent winsock.h
#include <Windows.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>



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
    void SetColorPallete(std::vector<ColorRGB> colors); // Works only on old terminal

    bool IsFullcreen = false;

private:
    HANDLE handleConsole;
    
    // For getting console info
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    SMALL_RECT rect;
    
    bool isFullScreen();
    void SimulateF11KeyPress();
};