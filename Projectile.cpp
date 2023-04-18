#include "Projectile.h"

Projectile::Projectile(int x, int y, int w, int h, int yvel, SDL_Texture* tex) : texture{ tex }, yvel{ yvel }
{
	destRect.w = w;
	destRect.h = h;
	destRect.x = x;
	destRect.y = y;

	timeStamp = SDL_GetTicks();
}

void Projectile::update()
{
	float deltaT = static_cast<float>(SDL_GetTicks() - timeStamp) / 1000; // Convert from ms to s
	destRect.y += yvel * deltaT;

	timeStamp = SDL_GetTicks();
}