#pragma once

#include "objects\GameObject.h"

#include <Windows.h>
#include <string>
#include <vector>

class IDrawableObject;

class DrawableGameObject : public GameObject, public IDrawableObject {
public:
	DrawableGameObject(std::string name, COORD position, std::vector<std::vector<SHORT>> texture);
	const std::vector<std::vector<SHORT>>& GetTexture() const override;
	COORD GetPosition() const override;
private:
	std::vector<std::vector<SHORT>> texture;
	COORD position;
};