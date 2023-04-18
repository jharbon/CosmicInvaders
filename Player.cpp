#include "Player.h"
#include <cmath>
#include <iostream>

Player::Player(int x, int y, int w, int h, int s, SDL_Texture* tex1, SDL_Texture* tex2)
	: texture{ tex1 }, projectileTexture{ tex2 }, xvel { 0 }, speed{ s }
{
	xpos = static_cast<float>(x);

	srcRect.w = 64;
	srcRect.h = 32;
	srcRect.x = srcRect.y = 0;
	destRect.w = w;
	destRect.h = h;
	destRect.x = x;
	destRect.y = y;

	timeStamp = SDL_GetTicks();
}

void Player::shoot()
{
	// Define the size of projectile
	int w = destRect.w / 10;
	int h = destRect.h / 3;
	// Projectile should start above the centre of player
	int x = destRect.x + (destRect.w/2)  - (w/2);
	int y = destRect.y;

	projectiles.push_back(new Projectile(x, y, w, h, -200, projectileTexture));
}

void Player::update()
{
	float deltaT = static_cast<float>(SDL_GetTicks() - timeStamp)/1000; // Convert from ms to s
	xpos += xvel * deltaT;
	destRect.x = round(xpos);
	timeStamp = SDL_GetTicks();

	for (auto p : projectiles) {
		p->update();
	}
}