#pragma once
#include "GameObject.h"
#include "IDrawableObject.h"

class DrawableGameObject : public GameObject, public IDrawableObject {
public:
	DrawableGameObject(std::string name, COORD position, std::vector<std::vector<SHORT>> texture);
	const std::vector<std::vector<SHORT>>& GetTexture() const override;
	COORD GetPosition() const override;
private:
	std::vector<std::vector<SHORT>> texture;
	COORD position;
};