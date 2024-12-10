#include "Scene.h"
#include "GameObject.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <Windows.h>

Scene::Scene(std::string name) : name(name) {

}

void Scene::AddObject(std::unique_ptr<GameObject> gameObject) {
    gameObject->setScene(this); // Устанавливаем сцену
    gameObjects.push_back(std::move(gameObject)); // Добавляем объект в список
}

void Scene::DestroyObject(GameObject* gameObject) {
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& obj) {
            return obj.get() == gameObject;
        });
    if (it != gameObjects.end()) {
        std::stringstream ss;
        ss << "Destroying: " << (*it)->GetName() << "\n";
        OutputDebugStringA(ss.str().c_str());
        gameObjects.erase(it); // Удаляем объект
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

