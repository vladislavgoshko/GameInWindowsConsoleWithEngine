#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

class Scene;

class GameObject {
public:
    GameObject(const std::string& name);
    virtual ~GameObject(); // ����������� ���������� ��� ������������
    void Destroy();

    const std::string& GetName() const { return name; } // ����� ��� ��������� �����

protected:
    Scene* scene = nullptr; // ��������� �� �����, �������� �����������

private:
    std::string name;

    void setScene(Scene* scene);
    friend class Scene;
};

#endif