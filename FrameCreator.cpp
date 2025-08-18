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
				DrawPixel(position.X + static_cast<short>(x),
					position.Y + static_cast<short>(y),
					color);
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


std::vector<CHAR_INFO> FrameCreator::GetFrame() {
	return charInfoArray;
}


void FrameCreator::DrawPixel(short X, short Y, SHORT color) {
	WORD* currentColor = &charInfoArray[frameSize.X * (Y >> 1) + X].Attributes;

	if (Y % 2 == 0) {
		*currentColor = *currentColor | color;
	}
	else {
		*currentColor = *currentColor | color << 4;

	}
}