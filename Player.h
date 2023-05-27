#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Player
{
private:
	int lives;

	SDL_Texture* projectileTexture;
	std::vector<Projectile> projectiles;
	int projectileSpeed;

public:
	float xpos;
	int xvel; // +/- Pixels/second
	int speed; // Pixels/second
	int timeStamp;

	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	Player() {}
	Player(int x, int y, int w, int h, int s, int projSpeed, SDL_Texture* tex1, SDL_Texture* tex2);
	~Player() {}

	void shoot(); 
	int getNumLives();
	void loseLife();
	int getNumProjectiles();
	SDL_Rect getProjectileRect(int i);
	void deleteProjectile(int i);
	void update();
	void render(SDL_Renderer* renderer);
};
