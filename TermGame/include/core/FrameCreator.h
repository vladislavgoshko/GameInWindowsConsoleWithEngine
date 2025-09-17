#pragma once

#define UPPER_HALF_SYMBOL 9600
#define LOWER_HALF_SYMBOL 9604
#define FULL_SYMBOL 9608

#include "interfaces\IDrawableObject.h"

#include <Windows.h>
#include <string>
#include <vector>

class FrameCreator {
public:
	FrameCreator(COORD frameSize);
	void DrawPixel(short X, short Y, SHORT color);
	void DrawObject(const IDrawableObject& obj);
	void WriteText(COORD coords, std::string text, int textColor = 0, int bgColor = 15);
	void ClearFrame();
	void Resize(COORD frameSize);
	const std::vector<CHAR_INFO>& GetFrame() const;
	COORD GetWindowSize();

private:

	std::vector<CHAR_INFO> charInfoArray;
	COORD windowSize;
	std::vector<WORD> colorPalette = std::vector<WORD>(16);
};