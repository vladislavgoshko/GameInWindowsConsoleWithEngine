#include <sstream>
#include "IDrawableObject.h"
#include "IUpdatable.h"
#include "KeyboardInputController.h"

class TestObject : public IDrawableObject, public IUpdatable {
	std::vector<std::vector<bool>> texture;
	COORD position;
	KeyboardInputController* inputController;
	float speed;                   // �������� ������
	float posX;                    // ������� �� X � ������� ������
	float posY;                    // ������� �� Y � ������� ������

public:
	TestObject(COORD startPos, float playerSpeed, KeyboardInputController* inputCtrl)
		: position(startPos), speed(playerSpeed), inputController(inputCtrl) {
		posX = static_cast<float>(startPos.X);
		posY = static_cast<float>(startPos.Y);
		texture = {
			{ true, true, true },
			{ true, false, true },
			{ true, true, true }
		};
	}

	// �������� ��������
	const std::vector<std::vector<bool>>& GetTexture() const override {
		return texture;
	}

	// �������� �������
	COORD GetPosition() const override {
		return position;
	}

	// ���������� �������
	void SetPosition(COORD newPosition) {
		position = newPosition;
		posX = static_cast<float>(newPosition.X);
		posY = static_cast<float>(newPosition.Y);
	}

	// ����� ��� ���������� �������, ���������� ������ ����
	void Update(float deltaTime) {

	}

	// ����� ��� ������������� ����������, ���������� � ���������� ���������� �������
	void FixedUpdate(float deltaTime) {
		if (inputController->IsKeyHeld(VK_UP) || inputController->IsKeyPressed(VK_UP)) {
			posY -= speed * deltaTime;
		}
		if (inputController->IsKeyHeld(VK_DOWN) || inputController->IsKeyPressed(VK_DOWN)) {
			posY += speed * deltaTime;
		}
		if (inputController->IsKeyHeld(VK_LEFT) || inputController->IsKeyPressed(VK_LEFT)) {
			posX -= speed * deltaTime;
		}
		if (inputController->IsKeyHeld(VK_RIGHT) || inputController->IsKeyPressed(VK_RIGHT)) {
			posX += speed * deltaTime;
		}
		UpdateIntegerPosition();
		//SHORT
		//	w = GetAsyncKeyState((SHORT)'W'),
		//	a = GetAsyncKeyState((SHORT)'A'),
		//	s = GetAsyncKeyState((SHORT)'S'),
		//	d = GetAsyncKeyState((SHORT)'D'),
		//	space = GetAsyncKeyState((SHORT)' ');
		//std::stringstream ss;
		//ss << w << " " << a << " " << s << " " << d << std::endl;
		//if (w) {
		//	position.Y--;
		//	//OutputDebugStringA(ss.str().c_str());
		//}
		//if (a) {
		//	position.X--;
		//	//OutputDebugStringA(ss.str().c_str());
		//}
		//if (s) {
		//	position.Y++;
		//	//OutputDebugStringA(ss.str().c_str());
		//}
		//if (d) {
		//	position.X++;
		//	//OutputDebugStringA(ss.str().c_str());
		//}
	}
private:
	void UpdateIntegerPosition() {
		position.X = static_cast<short>(std::round(posX));
		position.Y = static_cast<short>(std::round(posY));
	}
	void Move(short deltaX, short deltaY) {
		position.X += deltaX;
		position.Y += deltaY;
	}
};
