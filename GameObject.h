#pragma once

#include <string>
#include <vector>
class Scene;

class GameObject {
public:
    GameObject(const std::string& name);
    virtual ~GameObject(); // Виртуальный деструктор

    void Destroy();

    const std::string& GetName() const { return name; }

    void SetScene(Scene* scene); // Сделан публичным для удобства ручной привязки

protected:
    Scene* scene = nullptr;

private:
    std::string name;
    std::vector<int> largeData; // Большой массив данных
};
