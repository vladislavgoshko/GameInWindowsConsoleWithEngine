#pragma once
#include "IDrawableObject.h"
#include <string>
#include <vector>
#include <numeric>

#define UPPER_HALF_SYMBOL 9600
#define LOWER_HALF_SYMBOL 9604
#define FULL_SYMBOL 9608

class FrameCreator {
public:
	FrameCreator(COORD frameSize);
	void Resize(COORD frameSize);
	void ClearFrame();
	void DrawObject(const IDrawableObject& obj);
	void WriteText(COORD coords, std::string text, int textColor = 0, int bgColor = 15);
	std::vector<CHAR_INFO> GetFrame();
	COORD GetFrameSize();

private:
	void DrawPixel(short X, short Y, SHORT color);

	std::vector<CHAR_INFO> charInfoArray;
	COORD frameSize;
	std::vector<WORD> colorPalette = std::vector<WORD>(16);
};