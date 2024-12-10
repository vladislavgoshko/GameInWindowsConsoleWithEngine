#ifndef KEYBOARDINPUTCONTROLLER_H
#define KEYBOARDINPUTCONTROLLER_H

#include <Windows.h>
#include <unordered_map>

class KeyboardInputController {
public:
    KeyboardInputController();

    // Метод для обновления состояния клавиш
    void Update(float deltaTime = 0.016);

    // Проверка, была ли нажата клавиша
    bool IsKeyPressed(int virtualKeyCode) const;

    // Проверка, удерживается ли клавиша
    bool IsKeyHeld(int virtualKeyCode) const;

    // Проверка, была ли отпущена клавиша
    bool IsKeyReleased(int virtualKeyCode) const;

private:
    std::unordered_map<int, bool> currentKeyState; // Текущее состояние клавиш
    std::unordered_map<int, bool> previousKeyState; // Предыдущее состояние клавиш
};

#endif
