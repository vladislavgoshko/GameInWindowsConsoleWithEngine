#pragma once
#include <Windows.h>
#include <vector>

struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
    Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
    Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
};

struct Rigidbody2D {
    Vector2 velocity;
    float mass = 1.0f;
    bool isStatic = false;
};

struct BoxCollider2D {
    Vector2 position;
    Vector2 size;
};

class PhysicsEngine {
public:
    struct PhysicsObject {
        Rigidbody2D* rigidbody;
        BoxCollider2D* collider;
        void* userData; // pointer to game object
        uint16_t layer = 0;
        uint16_t collisionMask = 0xFFFF; // по умолчанию — со всеми
    };

    void AddObject(Rigidbody2D* rb, BoxCollider2D* col, void* userData = nullptr, uint16_t layer = 0, uint16_t mask = 0xFFFF);
    void RemoveObject(void* userData); // Новый метод
    void Update(float deltaTime);
    void ResolveCollisions();
    static bool CheckAABB(const BoxCollider2D& a, const BoxCollider2D& b);

private:
    std::vector<PhysicsObject> objects;
};
