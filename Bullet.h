#pragma once
#include "IDrawableObject.h"
#include "IUpdatable.h"
#include "PhysicsEngine.h"
#include <Windows.h>
#include <vector>

class Bullet : public IDrawableObject, public IUpdatable {
public:
    Bullet(COORD pos, SHORT color = 0, float speed = 60.0f);
    ~Bullet() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float fixedDeltaTime) override;
    COORD GetPosition() const override;
    const std::vector<std::vector<SHORT>>& GetTexture() const override;
    bool IsOutOfBounds(COORD screenSize) const;
    void MarkDestroyed();
    bool IsDestroyed() const;

    Rigidbody2D rigidbody;
    BoxCollider2D collider;

private:
    COORD position;
    float speed;
    SHORT color;
    std::vector<std::vector<SHORT>> texture;
    bool destroyed = false;
};
