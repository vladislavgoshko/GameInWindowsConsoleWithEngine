#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>

class GameObject;

class Scene {
public:
    Scene(std::string name);
    void AddObject(std::unique_ptr<GameObject> gameObject); // Принимаем уникальный указатель
    void DestroyObject(GameObject* gameObject);
    std::string GetName() const;
private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};

#endif