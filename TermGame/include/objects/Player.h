#pragma once

#include "core\PhysicsEngine.h"
#include "audio\MiniWaveOutAudio.h"
#include "interfaces\IDrawableObject.h"
#include "interfaces\IUpdatable.h"
#include "objects\GameObject.h"
#include "objects\Bullet.h"

#include <chrono>
#include <vector>

class KeyboardInputController;
class MouseInputController;

struct KeyBindings {
    int moveUp = 'W';
    int moveDown = 'S';
    int moveLeft = 'A';
    int moveRight = 'D';
    int shoot = VK_SPACE;
};

class Player : public GameObject, public IDrawableObject, public IUpdatable {
public:
    Player(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture, KeyboardInputController* inputController);
    ~Player() override; // Переопределяем виртуальный деструктор

    void TakeDamage(int damage);

    void Update(float deltaTime) override;
    void FixedUpdate(float fixedDeltaTime) override;

    const std::vector<std::vector<SHORT>>& GetTexture() const override;
    COORD GetPosition() const override;

    void SetBulletContainer(std::vector<Bullet*>* bullets);
    void SetPhysicsEngine(PhysicsEngine* physics); // Добавлено
    void SetMouseController(MouseInputController* mouse); // Добавлено
    void SetBindings(const KeyBindings& newBindings);

    // --- Физика ---
    Rigidbody2D rigidbody;
    BoxCollider2D collider;
    void SyncPhysicsToPosition();
    void SyncPositionToPhysics();

private:
    MiniAudio::Engine audio;
    MiniAudio::WavData shot;

    int health;
    COORD position;
    std::vector<std::vector<SHORT>> texture;
    KeyboardInputController* inputController;
    KeyBindings bindings;

    float speed; // Скорость игрока
    std::chrono::system_clock::time_point lastMoveTP;
    std::vector<Bullet*>* bullets = nullptr;
    std::chrono::system_clock::time_point lastShotTP = std::chrono::system_clock::now();
    float fireCooldown = 0.1f; // seconds
    PhysicsEngine* physics = nullptr; // Добавлено
    MouseInputController* mouseController = nullptr; // Добавлено
};