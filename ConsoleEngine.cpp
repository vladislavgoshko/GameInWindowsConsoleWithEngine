#include "ConsoleEngine.h"


ConsoleEngine::ConsoleEngine() {
	handleConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(handleConsole);

	GetConsoleScreenBufferInfoEx(handleConsole, &info);
	info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(handleConsole, &info);

	COORD currentWindowSize = info.dwSize;

	rect = { 0, 0, (short)(currentWindowSize.X - 1), (short)(currentWindowSize.Y - 1) };
	//IsFullcreen = isFullScreen();
}




void ConsoleEngine::Draw(const CHAR_INFO* screenArray) {
	auto currentWindowSize = GetWindowSize();

	rect = { 0, 0, (short)(currentWindowSize.X - 1), (short)(currentWindowSize.Y - 1) };

	try {
		WriteConsoleOutput(handleConsole, screenArray, currentWindowSize, { 0, 0 }, &rect);
	}
	catch (...) {
	}
}


void ConsoleEngine::ChangeTitle(std::string title) {
	std::cout << "\033]0;" << title << "\007";
}

COORD ConsoleEngine::GetWindowSize() {
	GetConsoleScreenBufferInfoEx(handleConsole, &info);
	return info.dwSize;
}


void ConsoleEngine::SetColorPallete(std::vector<ColorRGB> colors) {
	if (colors.size() != 16) {
		return;
	}

	for (int i = 0; i < 16; ++i) {
		info.ColorTable[i] = RGB(colors[i].Red, colors[i].Green, colors[i].Blue);
	}

	SetConsoleScreenBufferInfoEx(handleConsole, &info); // для применения изменений, например, палитры цветов

}


void ConsoleEngine::SwitchFullscreen(bool enable) {
	if (enable && !IsFullcreen) {
		SimulateF11KeyPress();
		IsFullcreen = true;
	}
	if (!enable && IsFullcreen) {
		SimulateF11KeyPress();
		IsFullcreen = false;
	}
}

void ConsoleEngine::SimulateF11KeyPress() {
	// Create an INPUT structure for the key press
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = VK_F11; // Virtual key code for F11

	// Simulate key press
	SendInput(1, &input, sizeof(INPUT));

	// Simulate key release
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

bool ConsoleEngine::isFullScreen()
{
	CONSOLE_SCREEN_BUFFER_INFOEX info1, info2;
	GetConsoleScreenBufferInfoEx(handleConsole, &info1);
	SimulateF11KeyPress();
	GetConsoleScreenBufferInfoEx(handleConsole, &info2);
	SimulateF11KeyPress();
	if (info1.dwSize.X < info2.dwSize.X || info1.dwSize.Y < info2.dwSize.Y) {
		return true;
	}
	return false;
}
