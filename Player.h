#pragma once

#include "SDL.h"
#include <vector>
#include "Projectile.h"

class Player
{
private:
public:
	float xpos;
	int xvel; // +/- Pixels/second
	int speed; // Pixels/second
	int timeStamp;
	std::vector<Projectile*> projectiles;

	SDL_Texture* texture;
	SDL_Texture* projectileTexture;
	SDL_Rect srcRect, destRect;

	Player() {}
	Player(int x, int y, int w, int h, int s, SDL_Texture* tex1, SDL_Texture* tex2);
	~Player() {}

	void shoot(); 
	void update();
};
