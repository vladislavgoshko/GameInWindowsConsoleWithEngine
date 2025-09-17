#include "core\ConsoleEngine.h"
#include <iostream>

ConsoleEngine::ConsoleEngine() {
	handleConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	handleConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	if (handleConsoleIn == INVALID_HANDLE_VALUE) {
		OutputDebugStringA("Failed to get console input handle.");
	}

	SetConsoleActiveScreenBuffer(handleConsoleOut);

	GetConsoleScreenBufferInfoEx(handleConsoleOut, &info);
	info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(handleConsoleOut, &info);

	DWORD prevMode;
	if (!GetConsoleMode(handleConsoleIn, &prevMode)) {
		OutputDebugStringA("Failed to get current console mode.");
	}

	if (!SetConsoleMode(handleConsoleIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS)) {
		OutputDebugStringA("Failed to set console mode.");
	}

	COORD currentWindowSize = info.dwSize;

	rect = { 0, 0, (short)(currentWindowSize.X - 1), (short)(currentWindowSize.Y - 1) };


}

void ConsoleEngine::Draw(const CHAR_INFO* screenArray) {
	auto currentWindowSize = GetWindowSize();

	rect = { 0, 0, (short)(currentWindowSize.X - 1), (short)(currentWindowSize.Y - 1) };
	if (currentWindowSize.X > 0 && currentWindowSize.Y > 0 && rect.Right > 0 && rect.Bottom > 0) {
		WriteConsoleOutput(handleConsoleOut, screenArray, currentWindowSize, { 0, 0 }, &rect);
	}

}

void ConsoleEngine::ChangeTitle(std::string title) {
	std::cout << "\033]0;" << title << "\007";
}

COORD ConsoleEngine::GetWindowSize() {
	GetConsoleScreenBufferInfoEx(handleConsoleOut, &info);
	return info.dwSize;
}

void ConsoleEngine::SetColorPallete(std::vector<ColorRGB> colors) {
	if (colors.size() != 16) {
		return;
	}

	for (int i = 0; i < 16; ++i) {
		info.ColorTable[i] = RGB(colors[i].Red, colors[i].Green, colors[i].Blue);
	}

	SetConsoleScreenBufferInfoEx(handleConsoleOut, &info); // для применения изменений, например, палитры цветов
}

bool ConsoleEngine::IsFullscreen() const {
	HWND hwnd = GetConsoleWindow();
	WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
	if (GetWindowPlacement(hwnd, &placement)) {
		return placement.showCmd == SW_SHOWMAXIMIZED;
	}
	return false;
}

void ConsoleEngine::SwitchFullscreen(bool enable) {
	HWND hwnd = GetConsoleWindow();
	if (enable) {
		if (IsFullscreen()) return; // Уже в полноэкранном режиме
		// Попытка эмулировать Alt+Enter (только для классической консоли)
		keybd_event(VK_MENU, 0x38, 0, 0); // Alt down
		keybd_event(VK_RETURN, 0x1C, 0, 0); // Enter down
		keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0); // Enter up
		keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0); // Alt up
		// На новых терминалах fallback: просто максимизируем окно
		ShowWindow(hwnd, SW_MAXIMIZE);
	}
	else {
		if (!IsFullscreen()) return; // Уже не в полноэкранном режиме
		// Повторно эмулируем Alt+Enter для выхода из fullscreen
		keybd_event(VK_MENU, 0x38, 0, 0);
		keybd_event(VK_RETURN, 0x1C, 0, 0);
		keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
		ShowWindow(hwnd, SW_RESTORE);
	}
	IsFullcreen = enable;
}

void ConsoleEngine::IncreaseScale() {
	keybd_event(VK_CONTROL, 0x1D, 0, 0); // Ctrl down
	keybd_event(VK_OEM_PLUS, 0x0D, 0, 0); // '+' down
	keybd_event(VK_OEM_PLUS, 0x0D, KEYEVENTF_KEYUP, 0); // '+' up
	keybd_event(VK_CONTROL, 0x1D, KEYEVENTF_KEYUP, 0); // Ctrl up
}

void ConsoleEngine::DecreaseScale() {
	keybd_event(VK_CONTROL, 0x1D, 0, 0); // Ctrl down
	keybd_event(VK_OEM_MINUS, 0x0C, 0, 0); // '-' down
	keybd_event(VK_OEM_MINUS, 0x0C, KEYEVENTF_KEYUP, 0); // '-' up
	keybd_event(VK_CONTROL, 0x1D, KEYEVENTF_KEYUP, 0); // Ctrl up
}
