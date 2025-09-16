#include "PhysicsEngine.h"
#include "Bullet.h"
#include <sstream>
#include <algorithm> // Для std::min и std::max
#include <cmath> // Для std::roundf

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

void PhysicsEngine::AddObject(Rigidbody2D* rb, BoxCollider2D* col, void* userData, uint16_t layer, uint16_t mask) {
    objects.push_back({ rb, col, userData, layer, mask });
}

void PhysicsEngine::Update(float deltaTime) {
    // Update positions
    for (auto& obj : objects) {
        if (!obj.rigidbody || !obj.collider || obj.rigidbody->isStatic) continue;
        obj.collider->position += obj.rigidbody->velocity * deltaTime;
    }
    ResolveCollisions();
}

void PhysicsEngine::ResolveCollisions() {
    const float epsilon = 1e-3f;
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto& a = objects[i];
            auto& b = objects[j];
            if (!a.collider || !b.collider) continue;
            if (!(a.collisionMask & (1 << b.layer)) && !(b.collisionMask & (1 << a.layer))) continue;
            if (CheckAABB(*a.collider, *b.collider)) {
                // Если один из объектов - Bullet, уничтожаем пулю
                Bullet* bulletA = dynamic_cast<Bullet*>(static_cast<IDrawableObject*>(a.userData));
                Bullet* bulletB = dynamic_cast<Bullet*>(static_cast<IDrawableObject*>(b.userData));
                if (bulletA) bulletA->MarkDestroyed();
                if (bulletB) bulletB->MarkDestroyed();
                // --- стандартное разрешение коллизий ---
                float aMinX = a.collider->position.x;
                float aMaxX = a.collider->position.x + a.collider->size.x;
                float aMinY = a.collider->position.y;
                float aMaxY = a.collider->position.y + a.collider->size.y;
                float bMinX = b.collider->position.x;
                float bMaxX = b.collider->position.x + b.collider->size.x;
                float bMinY = b.collider->position.y;
                float bMaxY = b.collider->position.y + b.collider->size.y;

                float overlapX = min(aMaxX, bMaxX) - max(aMinX, bMinX);
                float overlapY = min(aMaxY, bMaxY) - max(aMinY, bMinY);

                if (overlapX < overlapY) {
                    if (a.rigidbody && !a.rigidbody->isStatic) {
                        if (a.rigidbody->velocity.x > 0) {
                            a.collider->position.x -= overlapX;
                        } else if (a.rigidbody->velocity.x < 0) {
                            a.collider->position.x += overlapX;
                        }
                        a.rigidbody->velocity.x = 0;
                        a.collider->position.x = roundf(a.collider->position.x);
                    } else if (b.rigidbody && !b.rigidbody->isStatic) {
                        if (b.rigidbody->velocity.x > 0) {
                            b.collider->position.x -= overlapX;
                        } else if (b.rigidbody->velocity.x < 0) {
                            b.collider->position.x += overlapX;
                        }
                        b.rigidbody->velocity.x = 0;
                        b.collider->position.x = roundf(b.collider->position.x);

                    }
                } else {
                    if (overlapY > epsilon) {
                        if (a.rigidbody && !a.rigidbody->isStatic) {
                            if (a.rigidbody->velocity.y > 0) {
                                a.collider->position.y -= overlapY;
                            } else if (a.rigidbody->velocity.y < 0) {
                                a.collider->position.y += overlapY;
                            }
                            a.rigidbody->velocity.y = 0;
                            a.collider->position.y = roundf(a.collider->position.y);
                        } else if (b.rigidbody && !b.rigidbody->isStatic) {
                            if (b.rigidbody->velocity.y > 0) {
                                b.collider->position.y -= overlapY;
                            } else if (b.rigidbody->velocity.y < 0) {
                                b.collider->position.y += overlapY;
                            }
                            b.rigidbody->velocity.y = 0;
                            b.collider->position.y = roundf(b.collider->position.y);
                        }
                    }
                }
            }
        }
    }
}

bool PhysicsEngine::CheckAABB(const BoxCollider2D& a, const BoxCollider2D& b) {
    return (a.position.x < b.position.x + b.size.x &&
            a.position.x + a.size.x > b.position.x &&
            a.position.y < b.position.y + b.size.y &&
            a.position.y + a.size.y > b.position.y);
}

void PhysicsEngine::RemoveObject(void* userData) {
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->userData == userData) {
            objects.erase(it);
            break;
        }
    }
}
