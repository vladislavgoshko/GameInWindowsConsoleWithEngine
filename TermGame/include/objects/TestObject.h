#pragma once

#define _WINSOCKAPI_ // Prevent winsock.h

#include "interfaces\IDrawableObject.h"
#include "interfaces\IUpdatable.h"

#include <vector>
#include <chrono>
#include <windows.h> // ��� COORD

class KeyboardInputController;
class ConsoleEngine;

class TestObject : public IDrawableObject, public IUpdatable {
private:
    std::vector<std::vector<SHORT>> texture;
    COORD position;
    KeyboardInputController* inputController;
    float speed; // �������� ������
    ConsoleEngine* ce;
    std::chrono::system_clock::time_point lastMoveTP;

public:
    // �����������
    TestObject(COORD startPos, float playerSpeed, KeyboardInputController* inputCtrl, ConsoleEngine* ce);

    // �������� ��������
    const std::vector<std::vector<SHORT>>& GetTexture() const override;

    // �������� �������
    COORD GetPosition() const override;

    // ���������� �������
    void SetPosition(COORD newPosition);

    // ����� ��� ���������� �������, ���������� ������ ����
    void Update(float deltaTime);

    // ����� ��� ������������� ����������, ���������� � ���������� ���������� �������
    void FixedUpdate(float deltaTime);
};