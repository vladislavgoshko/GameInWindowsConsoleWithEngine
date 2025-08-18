#include "Scene.h"
#include "GameObject.h"
//#include <algorithm>
#include <sstream>
#include <Windows.h>


Scene::Scene(const std::string& name) : name(name) {}

void Scene::AddObject(GameObject& gameObject) {
    gameObject.SetScene(this); // ������������� �����
    gameObjects.push_back(&gameObject); // ��������� ��������� �� ������
}

void Scene::DestroyObject(GameObject* gameObject) {
    auto it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
    if (it != gameObjects.end()) {
        std::stringstream ss;
        ss << "Destroying: " << gameObject->GetName() << "\n";
        OutputDebugStringA(ss.str().c_str());
        gameObjects.erase(it); // ������� ������ �� ������
        //delete gameObject;
    }
    else {
        std::stringstream ss;
        ss << "Object not found in scene: " << gameObject << "\n";
        OutputDebugStringA(ss.str().c_str());
    }
}

std::string Scene::GetName() const {
    return name;
}

