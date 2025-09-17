#include "objects\Bullet.h"
#include <cmath>


Bullet::Bullet(COORD pos, SHORT color, float speed)
    : position(pos), color(color), speed(speed) {
    texture = {{color}};
    collider.position = Vector2(pos.X, pos.Y);
    collider.size = Vector2(1, 1); // маленький коллайдер
    rigidbody.velocity = Vector2(0,0); // летит вверх
    rigidbody.mass = 0.1f;
    rigidbody.isStatic = false;
}

Bullet::~Bullet() {}

void Bullet::Update(float deltaTime) {
    // Позиция обновляется через физику
    position.X = static_cast<SHORT>(std::round(collider.position.x));
    position.Y = static_cast<SHORT>(std::round(collider.position.y));
}

void Bullet::FixedUpdate(float fixedDeltaTime) {
    // Физика обновляется PhysicsEngine
}

COORD Bullet::GetPosition() const {
    return position;
}

const std::vector<std::vector<SHORT>>& Bullet::GetTexture() const {
    return texture;
}

bool Bullet::IsOutOfBounds(COORD screenSize) const {
    return position.Y < 0 || position.Y >= screenSize.Y || position.X < 0 || position.X >= screenSize.X;
}

void Bullet::MarkDestroyed() {
    destroyed = true;
    texture = { {5} };
}

bool Bullet::IsDestroyed() const {
    return destroyed;
}
