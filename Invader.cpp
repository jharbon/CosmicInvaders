#include "Invader.h"
#include <iostream>

Invader::Invader(int x, int y, int w, int h, int projSpeed, SDL_Texture* tex1, SDL_Texture* tex2)
	: projectileSpeed{ projSpeed }, texture {tex1}, projectileTexture{ tex2 }
{
	srcRect.w = srcRect.h = 32;
	srcRect.x = srcRect.y = 0;

	destRect.w = w;
	destRect.h = h;
	destRect.x = x;
	destRect.y = y;
}

Projectile Invader::shoot()
{
	// Define the size of projectile
	int w = destRect.w / 10;
	int h = destRect.h / 3;
	// Projectile should start below the centre of invader
	int x = destRect.x + (destRect.w / 2) - (w / 2);
	int y = destRect.y + destRect.h;

	return Projectile(x, y, w, h, projectileSpeed, projectileTexture);
}

void Invader::update()
{
	
}