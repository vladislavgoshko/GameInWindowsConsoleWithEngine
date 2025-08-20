#include "FrameCreator.h"

FrameCreator::FrameCreator(COORD frameSize) : frameSize(frameSize),
colorPalette(16),
charInfoArray(frameSize.X* frameSize.Y)
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
			if (color) {
				auto X = position.X + static_cast<short>(x), Y = position.Y + static_cast<short>(y);
				DrawPixel(X, Y, color);
			}
		}
	}
}

void FrameCreator::WriteText(COORD coords, std::string text, int textColor, int bgColor) {
	// ����������� std::string � std::wstring
	std::wstring wtext(text.begin(), text.end());

	// ��������� ����� �� ������� ������
	if (coords.Y < 0 || coords.Y >= frameSize.Y || coords.X < 0 || coords.X >= frameSize.X) {
		return; // ���� ���������� �� �������� ������, ������ �� ������
	}

	// ��������� ����
	WORD color = (bgColor << 4) | textColor; // ��������� ������� �����

	// ���������� ����� � screenBuffer � ������������� ���� � consoleColorBuffer
	for (size_t i = 0; i < wtext.size(); ++i) {
		short x = coords.X + static_cast<short>(i); // ���������� X �������� �������
		if (x >= frameSize.X) {
			break; // ���������� ������, ���� ������ ������� �� ������� ������
		}
		charInfoArray[coords.Y * frameSize.X + x].Char.UnicodeChar = wtext[i]; // ������ �������
		charInfoArray[coords.Y * frameSize.X + x].Attributes = color;
		//FillConsoleOutputAttribute(handleConsole, color, 1, { x, coords.Y }, &lpNumberOfCharsWritten); // ��������� �����
	}
}

void FrameCreator::ClearFrame() {
	CHAR_INFO clearChar{};
	clearChar.Char.UnicodeChar = UPPER_HALF_SYMBOL;
	clearChar.Attributes = 0; // ������ ��� + ������ �����

	std::fill(charInfoArray.begin(), charInfoArray.end(), clearChar);
}

void FrameCreator::Resize(COORD newSize) {
	charInfoArray.resize(newSize.X * newSize.Y);
	frameSize = newSize;
	ClearFrame();
}

std::vector<CHAR_INFO> FrameCreator::GetFrame() {
	return charInfoArray;
}

COORD FrameCreator::GetFrameSize() {
	return frameSize;
}


void FrameCreator::DrawPixel(short X, short Y, SHORT color) {
	if (X >= frameSize.X || Y >= frameSize.Y * 2 || X < 0 || Y < 0) {
		return;
	}
	WORD* currentColor = &charInfoArray[frameSize.X * (Y >> 1) + X].Attributes;

	if (Y % 2 == 0) {
		*currentColor = *currentColor & 0b11110000 | color;
	}
	else {
		*currentColor = *currentColor & 0b00001111 | color << 4;

	}
}