#ifndef KEYBOARDINPUTCONTROLLER_H
#define KEYBOARDINPUTCONTROLLER_H

#include <Windows.h>
#include <unordered_map>

class KeyboardInputController {
public:
    KeyboardInputController();

    // ����� ��� ���������� ��������� ������
    void Update(float deltaTime = 0.016);

    // ��������, ���� �� ������ �������
    bool IsKeyPressed(int virtualKeyCode) const;

    // ��������, ������������ �� �������
    bool IsKeyHeld(int virtualKeyCode) const;

    // ��������, ���� �� �������� �������
    bool IsKeyReleased(int virtualKeyCode) const;

private:
    std::unordered_map<int, bool> currentKeyState; // ������� ��������� ������
    std::unordered_map<int, bool> previousKeyState; // ���������� ��������� ������
};

#endif
