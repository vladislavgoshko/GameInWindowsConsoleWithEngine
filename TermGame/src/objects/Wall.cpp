#include "objects\Wall.h"

Wall::Wall(COORD pos, COORD size, SHORT color)
    : position(pos), size(size) {
    // Создать текстуру стены
    texture.resize(size.Y, std::vector<SHORT>(size.X, color));
    collider.position = Vector2(pos.X, pos.Y);
    collider.size = Vector2(size.X, size.Y);
}

const std::vector<std::vector<SHORT>>& Wall::GetTexture() const {
    return texture;
}

COORD Wall::GetPosition() const {
    return position;
}
