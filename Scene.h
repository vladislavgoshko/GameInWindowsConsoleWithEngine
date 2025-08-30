#pragma once

#include <vector>
#include <string>

class GameObject;

class Scene {
public:
    Scene(const std::string& name);

    void AddObject(GameObject& gameObject); // ��������� ������ �� ������
    void DestroyObject(GameObject* gameObject);

    std::string GetName() const;

private:
    std::string name;
    std::vector<GameObject*> gameObjects; // ��������� �� ������� (�� ��������)
};