#include "KeyboardInputController.h"

KeyboardInputController::KeyboardInputController() {
    // �������������� ��������� ��������� ������
    for (int key = 0; key < 256; ++key) {
        currentKeyState[key] = false;
        previousKeyState[key] = false;
    }
}

void KeyboardInputController::Update(float deltaTime) {
    // �������� ������� ��������� � ����������
    previousKeyState = currentKeyState;

    // ��������� ������� ��������� ���� ������
    for (int key = 0; key < 256; ++key) {
        currentKeyState[key] = (GetAsyncKeyState(key) & 0x8000) != 0;
    }
}

bool KeyboardInputController::IsKeyPressed(int virtualKeyCode) const {
    // ������� ���� ������ � ������� �����, �� �� ������������ � ����������
    return currentKeyState.at(virtualKeyCode) && !previousKeyState.at(virtualKeyCode);
}

bool KeyboardInputController::IsKeyHeld(int virtualKeyCode) const {
    // ������� ������������
    return currentKeyState.at(virtualKeyCode);
}

bool KeyboardInputController::IsKeyReleased(int virtualKeyCode) const {
    // ������� ���� ��������
    return !currentKeyState.at(virtualKeyCode) && previousKeyState.at(virtualKeyCode);
}
