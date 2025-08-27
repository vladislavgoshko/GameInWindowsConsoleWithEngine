#include <windows.h>
#include <string>
#include <sstream>

class MouseHelper {
public:
    static bool IsUpperHalf(COORD consolePos) {
        HWND hwnd = GetConsoleWindow();
        if (!hwnd) {
            OutputDebugStringA("GetConsoleWindow returned NULL\n");
            return false;
        }

        POINT cursorPos;
        GetCursorPos(&cursorPos);

        // Получаем координаты окна консоли
        RECT wndRect{};
        if (!GetWindowRect(hwnd, &wndRect)) {
            OutputDebugStringA("GetWindowRect failed, cannot get window position.\n");
            return false;
        }

        POINT clientPos = cursorPos;
        if (!ScreenToClient(hwnd, &clientPos)) {
            OutputDebugStringA("ScreenToClient failed, cannot determine relative cursor position.\n");
            return false;
        }

        // Получаем размер шрифта
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_FONT_INFOEX fontInfoEx{};
        fontInfoEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);

        if (!GetCurrentConsoleFontEx(hOut, FALSE, &fontInfoEx)) {
            OutputDebugStringA("GetCurrentConsoleFontEx failed, cannot determine font size.\n");
            return false;
        }

        int fontH = fontInfoEx.dwFontSize.Y;

        // Проверяем, что высота шрифта корректна
        if (fontH <= 0) {
            OutputDebugStringA("Invalid font height received.\n");
            return false;
        }

        // Координата Y ячейки в пикселях
        int cellY = consolePos.Y * fontH;

        // Положение курсора внутри ячейки
        int localY = clientPos.y - cellY;
        bool upper = localY < (fontH / 2);

        // Логирование для отладки
        std::stringstream ss;
        ss << "Click at console cell (" << consolePos.X << ", " << consolePos.Y << ")\n"
            << "Cursor screen pos: (" << cursorPos.x << ", " << cursorPos.y << ")\n"
            << "Console window rect: L=" << wndRect.left << " T=" << wndRect.top
            << " R=" << wndRect.right << " B=" << wndRect.bottom << "\n"
            << "Cursor client pos: (" << clientPos.x << ", " << clientPos.y << ")\n"
            << "Font size: (X=" << fontInfoEx.dwFontSize.X << "x" << fontH << ")\n"
            << "-> " << (upper ? "UPPER" : "LOWER") << " half of char\n";
        OutputDebugStringA(ss.str().c_str());

        return upper;
    }
};