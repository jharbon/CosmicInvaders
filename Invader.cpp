#include "Invader.h"
#include <iostream>

Invader::Invader(int x, int y, int w, int h, SDL_Texture* tex1, SDL_Texture* tex2)
	: texture{ tex1 }, projectileTexture{ tex2 }
{
	srcRect.w = srcRect.h = 32;
	srcRect.x = srcRect.y = 0;

	destRect.w = w;
	destRect.h = h;
	destRect.x = x;
	destRect.y = y;
}

void Invader::shoot()
{
	// Define the size of projectile
	int w = destRect.w / 10;
	int h = destRect.h / 3;
	// Projectile should start below the centre of invader
	int x = destRect.x + (destRect.w / 2) - (w / 2);
	int y = destRect.y + destRect.h;

	projectiles.push_back(new Projectile(x, y, w, h, 200, projectileTexture));
}

void Invader::update()
{
	for (auto p : projectiles) {
		p->update();
	}
}