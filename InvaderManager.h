#pragma once

#include "SDL.h"
#include "Invader.h"
#include "Projectile.h"
#include <vector>

class InvaderManager
{
private:
	int initRows;
	int initCols;
	int xStep, yStep;
	int xBoundaryLeft, xBoundaryRight;

	std::vector<std::vector<Invader>> invaders;
	int movementTimeStamp;
	float movementWaitPeriod;
	int shotTimeStamp;
	float shotWaitPeriod;
	std::vector<Projectile> projectiles;

public:
	InvaderManager() {}
	InvaderManager(int rows, int cols, float spacingToOffsetRatio, int winWidth, int yOffset, int invaderSize, int projectileSpeed, SDL_Texture* invaderTex, SDL_Texture* projectileTex);
	~InvaderManager() {}

	int getNumRows();
	int getNumCols(int i);
	int getNumInvaders();
	SDL_Rect getInvaderRect(int row, int col);
	void deleteInvader(int row, int col);

	void resetShotTimeStamp();
	int getNumProjectiles();
	SDL_Rect getProjectileRect(int i);
	void deleteProjectile(int i);

	void update(SDL_Rect playerRect);
	void render(SDL_Renderer* renderer);
};