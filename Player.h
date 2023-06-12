#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Player
{
private:
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;
	int xvel; // +/- Pixels/second
	int speed; // Pixels/second
	int timeStamp;
	int lives;

	SDL_Texture* projectileTexture;
	std::vector<Projectile> projectiles;
	int projectileSpeed;
	int shotTimeStamp;

public:
	Player() {}
	Player(int x, int y, int w, int h, int s, int projSpeed, SDL_Texture* tex1, SDL_Texture* tex2);
	~Player() {}

	void setPosition(int x);
	void setVelocityDirection(int v);
	SDL_Rect getRect();
	int getNumLives();
	void loseLife();

	void shoot();
	int getNumProjectiles();
	SDL_Rect getProjectileRect(int i);
	void deleteProjectile(int i);

	void update();
	void render(SDL_Renderer* renderer);
};
