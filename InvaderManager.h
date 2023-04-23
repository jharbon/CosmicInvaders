#pragma once

#include "SDL.h"
#include "Invader.h"
#include <vector>

class InvaderManager
{
private:
public:
	std::vector<std::vector<Invader*>> invaders;

	InvaderManager() {}
	InvaderManager(int rows, int cols, int winWidth, float spacingToOffsetRatio, int invaderSize, SDL_Texture* invaderTex, SDL_Texture* projectileTex);
	~InvaderManager() {}

	void update();
	void render(SDL_Renderer* renderer);
};