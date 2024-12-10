#include "GraphicEngine.h"
#include <iostream>

#define UPPER_HALF_SYMBOL 9600
#define LOWER_HALF_SYMBOL 9604
#define FULL_SYMBOL 9608

GraphicEngine::GraphicEngine() {
	handleConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(handleConsole);
	GetConsoleScreenBufferInfoEx(handleConsole, &info);
	info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(handleConsole, &info);
	currentSize = info.dwSize;

	screenBuffer = new wchar_t[32768];
	for (int i = 0; i < 32768; ++i) {
		screenBuffer[i] = ' ';
	}
}

void GraphicEngine::DrawObjects(std::vector<IDrawableObject*>& objects) {
	//auto tp1 = std::chrono::system_clock::now(); // Начало отсчета кадра
	//auto tp2 = std::chrono::system_clock::now(); // Конец отсчета кадра
	//while (true) {

		//float fps = FPS(tp1, tp2); // FPS

		

		GetConsoleScreenBufferInfoEx(handleConsole, &info);
		if (currentSize.X != info.dwSize.X || currentSize.Y != info.dwSize.Y) {
			delete[] screenBuffer;
			screenBuffer = new wchar_t[info.dwSize.X * info.dwSize.Y];
			for (int i = 0; i < info.dwSize.X * info.dwSize.Y; ++i) {
				screenBuffer[i] = ' ';
			}
			currentSize = info.dwSize;
			FillConsoleOutputAttribute(handleConsole, consoleColor, currentSize.X * currentSize.Y, { 0, 0 }, new DWORD);
		}

		for (auto& obj : objects) {
			DrawObject(*obj);
		}

		//swprintf_s(screenBuffer, 60, L"Sizes: %3d %3d %20s", info.dwSize.X, info.dwSize.Y, debugInfo);
		swprintf_s(screenBuffer, 51, L"%s", debugInfo);
		WriteConsoleOutputCharacter(handleConsole, screenBuffer, info.dwSize.X * info.dwSize.Y, COORD{ 0, 0 }, new DWORD);

		for (int i = 0; i < info.dwSize.X * info.dwSize.Y; ++i) {
			screenBuffer[i] = ' ';
		}
	//}
}

void GraphicEngine::ChangeTitle(std::string title) {
	std::cout << "\033]0;" << title << "\007"; // Вывод фпс в название
}

void GraphicEngine::WriteDebugIndo(std::string debugInfo) {

	// Преобразуем std::string в std::wstring
	std::wstring wstr(debugInfo.begin(), debugInfo.end());

	// Проверяем, помещается ли строка в массив
	if (wstr.size() + 1 > 50) { // +1 для нулевого символа
		std::wcerr << L"Ошибка: строка слишком длинная для массива wchar_array" << std::endl;
		return; // Завершаем выполнение программы
	}

	// Копируем строку в wchar_array с использованием wcsncpy_s
	errno_t err = wcsncpy_s(this->debugInfo, 50, wstr.c_str(), wstr.size());
	if (err != 0) {
		std::wcerr << L"Ошибка при копировании строки" << std::endl;
		return;
	}
}

COORD GraphicEngine::GetWindowSize() {
	return currentSize;
}

void GraphicEngine::ChangeConsoleColor(int bgColor, int textColor) {
	consoleColor = (bgColor << 4) | textColor;
	FillConsoleOutputAttribute(handleConsole, consoleColor, currentSize.X * currentSize.Y, { 0, 0 }, new DWORD);
}

void GraphicEngine::SetConsoleColor(int textColor, int bgColor) {
	SetConsoleTextAttribute(handleConsole, (bgColor << 4) | textColor);
}

void GraphicEngine::SetColorPallete(std::vector<ColorRGB> colors) {
	if (colors.size() != 16) {
		return;
	}

	for (int i = 0; i < 16; ++i) {
		info.ColorTable[i] = RGB(colors[i].Red, colors[i].Green, colors[i].Blue);
	}

	SetConsoleScreenBufferInfoEx(handleConsole, &info); // для применения изменений, например, палитры цветов

}

void GraphicEngine::DrawObject(const IDrawableObject& obj) {
	const auto& texture = obj.GetTexture();
	COORD position = obj.GetPosition();

	for (size_t y = 0; y < texture.size(); ++y) {
		for (size_t x = 0; x < texture[y].size(); ++x) {
			if (texture[y][x]) {
				DrawPixel(screenBuffer, currentSize, position.X + x, position.Y + y);
			}
		}
	}
}


void GraphicEngine::DrawPixel(wchar_t* screenBuffer, COORD screenSizes, short X, short Y) { // Вставляет нужный символ: полный, верхнюю часть или нижнюю часть
	if (X > screenSizes.X || Y >= screenSizes.Y * 2 || X < 0 || Y < 0) return;

	wchar_t* c = &screenBuffer[screenSizes.X * (Y >> 1) + X];
	if (Y % 2 == 0) {
		if (*c == LOWER_HALF_SYMBOL) {
			*c = FULL_SYMBOL;
		}
		else if (*c == ' ') {
			*c = UPPER_HALF_SYMBOL;
		}
	}
	else {
		if (*c == UPPER_HALF_SYMBOL)
		{
			*c = FULL_SYMBOL;
		}
		else if (*c == ' ')
		{
			*c = LOWER_HALF_SYMBOL;
		}
	}
}

void GraphicEngine::ClearPixel(wchar_t* screenBuffer, COORD screenSizes, short X, short Y) {
	if (screenSizes.X <= X || screenSizes.Y * 2 <= Y) return;

	wchar_t* c = &screenBuffer[(screenSizes.X) * (Y >> 1) + X];
	if (Y % 2 == 0) {
		if (*c == UPPER_HALF_SYMBOL) {
			*c = ' ';
		}
		else if (*c == FULL_SYMBOL) {
			*c = LOWER_HALF_SYMBOL;
		}
	}
	else {
		if (*c == LOWER_HALF_SYMBOL)
		{
			*c = ' ';
		}
		else if (*c == FULL_SYMBOL)
		{
			*c = UPPER_HALF_SYMBOL;
		}
	}
}
float GraphicEngine::FPS(std::chrono::system_clock::time_point& tp1, std::chrono::system_clock::time_point& tp2) {
	tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	return 1 / elapsedTime.count(); // FPS
}