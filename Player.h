#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Player
{
private:
	SDL_Texture* projectileTexture;
	std::vector<Projectile> projectiles;

public:
	float xpos;
	int xvel; // +/- Pixels/second
	int speed; // Pixels/second
	int timeStamp;

	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	Player() {}
	Player(int x, int y, int w, int h, int s, SDL_Texture* tex1, SDL_Texture* tex2);
	~Player() {}

	void shoot(); 
	int getNumProjectiles();
	SDL_Rect getProjectileRect(int i);
	void deleteProjectile(int i);
	void update();
	void render(SDL_Renderer* renderer);
};
