#include "IDrawableObject.h"
#include "DrawableGameObject.h"

DrawableGameObject::DrawableGameObject(std::string name, COORD position, std::vector<std::vector<SHORT>> texture)
	: GameObject(name), position(position), texture(texture) {

}

const std::vector<std::vector<SHORT>>& DrawableGameObject::GetTexture() const {
	return texture;
}

COORD DrawableGameObject::GetPosition() const {
	return position;
}