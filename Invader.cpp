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

void Invader::moveStep(int xDelta, int yDelta)
{
	destRect.x += xDelta;
	destRect.y += yDelta;
	// Change 'frame' column being used from spritesheet each time to create animation
	if (srcRect.x == 0) {
		srcRect.x = 32;
	}
	else if (srcRect.x == 32) {
		srcRect.x = 0;
	}
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

