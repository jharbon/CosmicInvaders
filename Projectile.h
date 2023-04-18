#pragma once

#include "SDL.h"

class Projectile
{
private:
public:
	int yvel;
	int timeStamp;

	SDL_Texture* texture;
	SDL_Rect destRect;

	Projectile() {}
	Projectile(int x, int y, int w, int h, int yvel, SDL_Texture* tex);
	~Projectile() {}

	void update();
};