#include "KeyboardInputController.h"

KeyboardInputController::KeyboardInputController() {
    // Инициализируем начальное состояние клавиш
    for (int key = 0; key < 256; ++key) {
        currentKeyState[key] = false;
        previousKeyState[key] = false;
    }
}

void KeyboardInputController::Update(float deltaTime) {
    // Копируем текущее состояние в предыдущее
    previousKeyState = currentKeyState;

    // Обновляем текущее состояние всех клавиш
    for (int key = 0; key < 256; ++key) {
        currentKeyState[key] = (GetAsyncKeyState(key) & 0x8000) != 0;
    }
}

bool KeyboardInputController::IsKeyPressed(int virtualKeyCode) const {
    // Клавиша была нажата в текущем кадре, но не удерживалась в предыдущем
    return currentKeyState.at(virtualKeyCode) && !previousKeyState.at(virtualKeyCode);
}

bool KeyboardInputController::IsKeyHeld(int virtualKeyCode) const {
    // Клавиша удерживается
    return currentKeyState.at(virtualKeyCode);
}

bool KeyboardInputController::IsKeyReleased(int virtualKeyCode) const {
    // Клавиша была отпущена
    return !currentKeyState.at(virtualKeyCode) && previousKeyState.at(virtualKeyCode);
}
