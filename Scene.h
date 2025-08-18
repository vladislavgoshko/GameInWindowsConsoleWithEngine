#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>

class GameObject;

class Scene {
public:
    Scene(const std::string& name);

    void AddObject(GameObject& gameObject); // Принимаем объект по ссылке
    void DestroyObject(GameObject* gameObject);

    std::string GetName() const;

private:
    std::string name;
    std::vector<GameObject*> gameObjects; // Указатели на объекты (не владение)
};

#endif
