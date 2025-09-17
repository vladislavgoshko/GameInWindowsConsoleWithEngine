#pragma once

#define _WINSOCKAPI_ // Prevent winsock.h

#include "interfaces\IDrawableObject.h"
#include "interfaces\IUpdatable.h"

#include <vector>
#include <chrono>
#include <windows.h> // Для COORD

class KeyboardInputController;
class ConsoleEngine;

class TestObject : public IDrawableObject, public IUpdatable {
private:
    std::vector<std::vector<SHORT>> texture;
    COORD position;
    KeyboardInputController* inputController;
    float speed; // Скорость игрока
    ConsoleEngine* ce;
    std::chrono::system_clock::time_point lastMoveTP;

public:
    // Конструктор
    TestObject(COORD startPos, float playerSpeed, KeyboardInputController* inputCtrl, ConsoleEngine* ce);

    // Получить текстуру
    const std::vector<std::vector<SHORT>>& GetTexture() const override;

    // Получить позицию
    COORD GetPosition() const override;

    // Установить позицию
    void SetPosition(COORD newPosition);

    // Метод для обновления объекта, вызывается каждый кадр
    void Update(float deltaTime);

    // Метод для фиксированных обновлений, вызывается с постоянным интервалом времени
    void FixedUpdate(float deltaTime);
};