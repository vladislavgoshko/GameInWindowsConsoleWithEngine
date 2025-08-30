#pragma once

class IUpdatable {
public:
    virtual ~IUpdatable() = default;

    // Метод для обновления объекта, вызывается каждый кадр
    virtual void Update(float deltaTime) = 0;

    // Метод для фиксированных обновлений, вызывается с постоянным интервалом времени
    virtual void FixedUpdate(float fixedDeltaTime) = 0;
};