#include "Player.h"
#include <cmath>
#include <iostream>

Player::Player(int x, int y, int w, int h, int s, SDL_Texture* tex) : texture{ tex }, xvel{ 0 }, speed{ s }
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

void Player::update()
{
	float deltaT = static_cast<float>(SDL_GetTicks() - timeStamp)/1000; // Convert from ms to s
	xpos += xvel * deltaT;
	destRect.x = round(xpos);

	timeStamp = SDL_GetTicks();
}