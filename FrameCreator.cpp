#include "FrameCreator.h"
//#include "IDrawableObject.h"
#include <numeric>

FrameCreator::FrameCreator(COORD windowSize) : windowSize(windowSize),
colorPalette(16),
charInfoArray(windowSize.X* windowSize.Y)
{
	// ��������� ������� ������� 0..15
	std::iota(colorPalette.begin(), colorPalette.end(), 0);

	// ������� ������ � ����
	CHAR_INFO clearChar{};
	clearChar.Char.UnicodeChar = UPPER_HALF_SYMBOL;
	clearChar.Attributes = colorPalette[0] << 4 | colorPalette[0];

	// ��������� ���� ������ �����
	std::fill(charInfoArray.begin(), charInfoArray.end(), clearChar);
}


void FrameCreator::DrawObject(const IDrawableObject& obj) {
	const auto& texture = obj.GetTexture();
	COORD position = obj.GetPosition();

	for (size_t y = 0; y < texture.size(); ++y) {
		for (size_t x = 0; x < texture[y].size(); ++x) {
			SHORT color = texture[y][x];
			if (color >= 0) {
				auto frameX = position.X + static_cast<short>(x), frameY = position.Y + static_cast<short>(y);
				DrawPixel(frameX, frameY, color);
			}
		}
	}
}

void FrameCreator::WriteText(COORD windowPosition, std::string text, int textColor, int bgColor) {
	// ����������� std::string � std::wstring
	std::wstring wtext(text.begin(), text.end());

	// ��������� ����� �� ������� ������
	if (windowPosition.Y < 0 || windowPosition.Y >= windowSize.Y || windowPosition.X < 0 || windowPosition.X >= windowSize.X) {
		return; // ���� ���������� �� �������� ������, ������ �� ������
	}

	// ��������� ����
	WORD color = (bgColor << 4) | textColor; // ��������� ������� �����

	// ���������� ����� � screenBuffer � ������������� ���� � consoleColorBuffer
	for (size_t i = 0; i < wtext.size(); ++i) {
		short x = windowPosition.X + static_cast<short>(i); // ���������� X �������� �������
		if (x >= windowSize.X) {
			break; // ���������� ������, ���� ������ ������� �� ������� ������
		}
		charInfoArray[windowPosition.Y * windowSize.X + x].Char.UnicodeChar = wtext[i]; // ������ �������
		charInfoArray[windowPosition.Y * windowSize.X + x].Attributes = color;
		//FillConsoleOutputAttribute(handleConsole, color, 1, { x, coords.Y }, &lpNumberOfCharsWritten); // ��������� �����
	}
}

void FrameCreator::ClearFrame() {
	CHAR_INFO clearChar{};
	clearChar.Char.UnicodeChar = UPPER_HALF_SYMBOL;
	clearChar.Attributes = 0; // ������ ��� + ������ �����

	std::fill(charInfoArray.begin(), charInfoArray.end(), clearChar);
}

void FrameCreator::Resize(COORD newWindowSize) {
	charInfoArray.resize(newWindowSize.X * newWindowSize.Y);
	windowSize = newWindowSize;
	ClearFrame();
}

std::vector<CHAR_INFO> FrameCreator::GetFrame() {
	return charInfoArray;
}

COORD FrameCreator::GetWindowSize() {
	return windowSize;
}


void FrameCreator::DrawPixel(short frameX, short frameY, SHORT color) {
	if (frameX >= windowSize.X || frameY >= windowSize.Y * 2 || frameX < 0 || frameY < 0) {
		return;
	}
	WORD* currentColor = &charInfoArray[windowSize.X * (frameY >> 1) + frameX].Attributes;

	if (frameY % 2 == 0) {
		*currentColor = *currentColor & 0b11110000 | color;
	}
	else {
		*currentColor = *currentColor & 0b00001111 | color << 4;

	}
}