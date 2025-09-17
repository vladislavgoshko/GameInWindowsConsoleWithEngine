#include "objects\RemotePlayer.h"
#include <sstream>
#include <Windows.h>
#include <algorithm>

RemotePlayer::RemotePlayer(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture)
    : GameObject(name), health(health), speed(speed), position(position), texture(texture) {
    collider.position = Vector2(position.X, position.Y);
    collider.size = Vector2(10, 15);
}

RemotePlayer::~RemotePlayer() {
    std::stringstream ss;
    ss << GetName() << " (RemotePlayer) destroyed\n";
    OutputDebugStringA(ss.str().c_str());
}

void RemotePlayer::HandleNetworkData(const std::string data) {
    // Expect simple messages like: "POS x y" or "SHOT x y vx vy"
    std::istringstream ss(data);
    std::string cmd;
    ss >> cmd;
    if (cmd == "POS") {
        int x, y; ss >> x >> y;
        position.X = static_cast<SHORT>(x);
        position.Y = static_cast<SHORT>(y);
        collider.position.x = (float)position.X;
        collider.position.y = (float)position.Y;
    }
    else if (cmd == "SHOT") {
        int bx, by; float vx, vy; ss >> bx >> by >> vx >> vy;
        COORD bulletPos = { (SHORT)bx, (SHORT)by };
        Bullet* b = new Bullet(bulletPos);
        b->rigidbody.velocity = Vector2(vx, vy);
        if (bullets) {
            bullets->push_back(b);
            if (physics) physics->AddObject(&b->rigidbody, &b->collider, b, 2, 0b1000);
        }
    }
}

const std::vector<std::vector<SHORT>>& RemotePlayer::GetTexture() const { return texture; }
COORD RemotePlayer::GetPosition() const { return position; }

void RemotePlayer::Update(float deltaTime) {
    // nothing local, movement is driven by network
}
void RemotePlayer::FixedUpdate(float fixedDeltaTime) {
    // sync physics if present
    if (physics) {
        position.X = static_cast<SHORT>(collider.position.x);
        position.Y = static_cast<SHORT>(collider.position.y);
    }
}

void RemotePlayer::SetBulletContainer(std::vector<Bullet*>* bullets) { this->bullets = bullets; }
void RemotePlayer::SetPhysicsEngine(PhysicsEngine* physics) { this->physics = physics; }
