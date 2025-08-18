#include "GameObject.h"
#include "Scene.h"
#include <sstream>
#include <Windows.h>

GameObject::GameObject(const std::string& name) : name(name), largeData(10000000, 42) {}

GameObject::~GameObject() {
    std::stringstream ss;
    ss << name << " destroyed\n";
    OutputDebugStringA(ss.str().c_str());
}

void GameObject::Destroy() {
    if (scene) {
        scene->DestroyObject(this);
    }
    delete this;
}

void GameObject::SetScene(Scene* scene) {
    this->scene = scene;
}

