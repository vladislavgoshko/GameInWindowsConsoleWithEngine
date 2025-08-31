#pragma once
#include "GameObject.h"
#include "IDrawableObject.h"
#include "IUpdatable.h"
#include "MiniWaveOutAudio.h"
#include "PhysicsEngine.h"

#include <chrono>
#include <vector>
#include "Bullet.h"

class KeyboardInputController;
class MouseInputController; // ���������

class Player : public GameObject, public IDrawableObject, public IUpdatable {
public:
    Player(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture, KeyboardInputController* inputController);
    ~Player() override; // �������������� ����������� ����������

    void TakeDamage(int damage);

    void Update(float deltaTime) override;
    void FixedUpdate(float fixedDeltaTime) override;

    const std::vector<std::vector<SHORT>>& GetTexture() const override;
    COORD GetPosition() const override;

    // ������� ����� ��� ����� � ������� �����������
    void SetBulletContainer(std::vector<Bullet*>* bullets);
    void SetPhysicsEngine(PhysicsEngine* physics); // ���������
    void SetMouseController(MouseInputController* mouse); // ���������

    // --- ������ ---
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
    float speed; // �������� ������
    std::chrono::system_clock::time_point lastMoveTP;
    std::vector<Bullet*>* bullets = nullptr;
    std::chrono::system_clock::time_point lastShotTP = std::chrono::system_clock::now();
    float fireCooldown = 0.1f; // seconds
    PhysicsEngine* physics = nullptr; // ���������
    MouseInputController* mouseController = nullptr; // ���������
};