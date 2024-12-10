#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

class Player : public GameObject {
public:
    Player(const std::string& name, int health);
    ~Player() override; // Переопределяем виртуальный деструктор

    void TakeDamage(int damage);

private:
    int health;
};

#endif