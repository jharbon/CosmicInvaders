#pragma once

#include "SDL.h"
#include "Invader.h"
#include "Projectile.h"
#include <vector>

class InvaderManager
{
private:
public:
	std::vector<std::vector<Invader*>> invaders;

	InvaderManager() {}
	InvaderManager(int rows, int cols, int winWidth, float spacingToOffsetRatio, int invaderSize, SDL_Texture* invaderTex, SDL_Texture* projectileTex);
	~InvaderManager() {}

	int initCols;
	int xStep, yStep;
	int xBoundaryLeft, xBoundaryRight;

	int invaderMoveTimeStamp;
	int invaderShotTimeStamp;
	std::vector<Projectile*> projectiles;

	void update(SDL_Rect playerRect);

	void render(SDL_Renderer* renderer);
};