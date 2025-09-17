#pragma once

#include <vector>
#include <Windows.h>

class IDrawableObject {
public:
    virtual ~IDrawableObject() = default;

    // Возвращает текстуру объекта (двумерный булевый массив)
    virtual const std::vector<std::vector<SHORT>>& GetTexture() const = 0;

    // Возвращает координаты текстуры
    virtual COORD GetPosition() const = 0;
};