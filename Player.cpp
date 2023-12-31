#include "Player.h"
#include <cmath>
#include <iostream>

const float SHOT_WAIT_PERIOD = 0.75;

Player::Player(int x, int y, int w, int h, int s, int projSpeed, SDL_Texture* tex1, SDL_Texture* tex2)
	: projectileSpeed{ projSpeed }, texture { tex1 }, projectileTexture{ tex2 }, xvel{ 0 }, speed{ s }, lives{ 3 } 
{
	destRect.w = w;
	destRect.h = h;
	destRect.x = x;
	destRect.y = y;

	timeStamp = shotTimeStamp = SDL_GetTicks();
}

void Player::setPosition(int x)
{
	destRect.x = x;
}

void Player::setVelocityDirection(int v)
{
	if (v < 0) {
		// Left
		xvel = -speed;
	}
	else if (v == 0) {
		// Intepret this as zeroing the velocity
		xvel = 0;
	}
	else if (v > 0) {
		// Right
		xvel = speed;
	}
}

SDL_Rect Player::getRect()
{
	return destRect;
}

int Player::getNumLives()
{
	return lives;
}

void Player::loseLife()
{
	lives -= 1;
}

void Player::shoot()
{
	bool shotReady = static_cast<float>(SDL_GetTicks() - shotTimeStamp) / 1000 > SHOT_WAIT_PERIOD;
	if (shotReady) {
		// Define the size of projectile
		int w = destRect.w / 20;
		int h = destRect.h / 3;
		// Projectile should start above the centre of player
		int x = destRect.x + (destRect.w / 2) - (w / 2);
		int y = destRect.y;

		projectiles.push_back(Projectile(x, y, w, h, -projectileSpeed, projectileTexture));
		shotTimeStamp = SDL_GetTicks();
	}
}

int Player::getNumProjectiles()
{
	return projectiles.size();
}

SDL_Rect Player::getProjectileRect(int i)
{
	return projectiles[i].destRect;
}

void Player::deleteProjectile(int i)
{
	projectiles.erase(projectiles.begin() + i);
}

void Player::update()
{
	float deltaT = static_cast<float>(SDL_GetTicks() - timeStamp)/1000; // Convert from ms to s
	destRect.x += round(xvel * deltaT);
	timeStamp = SDL_GetTicks();

	for (auto &p : projectiles) {
		p.update();
	}
}

void Player::render(SDL_Renderer* renderer)
{
	SDL_RenderCopyEx(renderer, texture, NULL, &destRect, NULL, NULL, SDL_FLIP_NONE);
	for (auto &p : projectiles) {
		SDL_RenderCopyEx(renderer, p.texture, NULL, &p.destRect, NULL, NULL, SDL_FLIP_NONE);
	}
}