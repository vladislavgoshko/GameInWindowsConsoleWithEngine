#pragma once
#include "IDrawableObject.h"
//class IDrawableObject;
#include <Windows.h>
#include <string>
#include <vector>


#define UPPER_HALF_SYMBOL 9600
#define LOWER_HALF_SYMBOL 9604
#define FULL_SYMBOL 9608

class FrameCreator {
public:
	FrameCreator(COORD frameSize);
	void DrawPixel(short X, short Y, SHORT color);
	void DrawObject(const IDrawableObject& obj);
	void WriteText(COORD coords, std::string text, int textColor = 0, int bgColor = 15);
	void ClearFrame();
	void Resize(COORD frameSize);
	std::vector<CHAR_INFO> GetFrame();
	COORD GetWindowSize();

private:

	std::vector<CHAR_INFO> charInfoArray;
	COORD windowSize;
	std::vector<WORD> colorPalette = std::vector<WORD>(16);
};