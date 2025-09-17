#pragma once

#include <string>

class Scene;

class GameObject {
public:
    GameObject(const std::string& name);
    virtual ~GameObject(); // Виртуальный деструктор

    void Destroy();

    const std::string& GetName() const { return name; }

    void SetScene(Scene* scene);

protected:
    Scene* scene = nullptr;

private:
    std::string name;
};
