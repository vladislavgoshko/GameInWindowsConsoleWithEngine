#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

class Scene;

class GameObject {
public:
    GameObject(const std::string& name);
    virtual ~GameObject(); // Виртуальный деструктор для полиморфизма
    void Destroy();

    const std::string& GetName() const { return name; } // Метод для получения имени

protected:
    Scene* scene = nullptr; // Указатель на сцену, доступен наследникам

private:
    std::string name;

    void setScene(Scene* scene);
    friend class Scene;
};

#endif