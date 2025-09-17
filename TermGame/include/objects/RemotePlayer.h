#pragma once

#include "core\PhysicsEngine.h"
#include "interfaces\IDrawableObject.h"
#include "interfaces\IUpdatable.h"
#include "interfaces\INetworkDataObserver.h"
#include "objects\GameObject.h"
#include "objects\Bullet.h"

#include <vector>
#include <Windows.h>

class RemotePlayer : public GameObject, public IDrawableObject, public IUpdatable, public INetworkDataObserver {
public:
    RemotePlayer(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture);
    ~RemotePlayer() override;

    // INetworkDataObserver
    void HandleNetworkData(const std::string data) override;

    // IDrawableObject
    const std::vector<std::vector<SHORT>>& GetTexture() const override;
    COORD GetPosition() const override;

    // IUpdatable
    void Update(float deltaTime) override;
    void FixedUpdate(float fixedDeltaTime) override;

    void SetBulletContainer(std::vector<Bullet*>* bullets);
    void SetPhysicsEngine(PhysicsEngine* physics);

    Rigidbody2D rigidbody;
    BoxCollider2D collider;

private:
    int health;
    COORD position;
    std::vector<std::vector<SHORT>> texture;
    float speed;
    std::vector<Bullet*>* bullets = nullptr;
    PhysicsEngine* physics = nullptr;
};
