#pragma once

#include "core\PhysicsEngine.h"
#include "interfaces\IDrawableObject.h"

#include <Windows.h>
#include <vector>
#include <string>

class Wall : public IDrawableObject {
public:
    Wall(COORD pos, COORD size, SHORT color = 8);
    const std::vector<std::vector<SHORT>>& GetTexture() const override;
    COORD GetPosition() const override;

    BoxCollider2D collider;
private:
    COORD position;
    COORD size;
    std::vector<std::vector<SHORT>> texture;
};
