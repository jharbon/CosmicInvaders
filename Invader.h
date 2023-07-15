#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Invader
{
private:
	int projectileSpeed;

public:
	SDL_Texture* texture;
	SDL_Texture* projectileTexture;
	SDL_Rect srcRect, destRect;

	Invader() {}
	Invader(int x, int y, int w, int h, int projSpeed, SDL_Texture* tex1, SDL_Texture* tex2);

	void moveStep(int xDelta, int yDelta);
	Projectile shoot();
};