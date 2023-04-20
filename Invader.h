#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Invader
{
private:
public:
	SDL_Texture* texture;
	SDL_Texture* projectileTexture;
	SDL_Rect srcRect, destRect;

	std::vector<Projectile*> projectiles;

	Invader() {}
	Invader(int x, int y, int w, int h, SDL_Texture* tex1, SDL_Texture* tex2);
	~Invader() {}

	void shoot();
	void update();
};