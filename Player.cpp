#include "Player.h"
#include <sstream>
#include <Windows.h>

Player::Player(const std::string& name, int health)
    : GameObject(name), health(health) {}

Player::~Player() {
    std::stringstream ss;
    ss << GetName() << " (Player) destroyed\n";
    OutputDebugStringA(ss.str().c_str());
}

void Player::TakeDamage(int damage) {
    health -= damage;
    std::stringstream ss;
    ss << GetName() << " took " << damage << " damage, health now: " << health << "\n";
    OutputDebugStringA(ss.str().c_str());
}