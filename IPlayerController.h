#ifndef IPLAYERCONTROLLER_H
#define IPLAYERCONTROLLER_H

class IPlayerController {
public:
    virtual ~IPlayerController() = default;

    // Метод для обработки ввода игрока
    virtual void HandleInput(float deltaTime) = 0;

    // Метод для управления обновлениями игрока
    virtual void Update(float deltaTime) = 0;
};

#endif
