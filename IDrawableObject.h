#ifndef IDRAWABLEOBJECT_H
#define IDRAWABLEOBJECT_H

#include <vector>
#include <Windows.h>

class IDrawableObject {
public:
    virtual ~IDrawableObject() = default;

    // Возвращает текстуру объекта (двумерный булевый массив)
    virtual const std::vector<std::vector<bool>>& GetTexture() const = 0;

    // Возвращает координаты текстуры
    virtual COORD GetPosition() const = 0;
};

#endif
