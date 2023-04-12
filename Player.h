#pragma once

#include "SDL.h"

class Player
{
private:
public:
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

	float xpos;
	int xvel; // +/- Pixels/second
	int speed; // Pixels/second
	int timeStamp;

	Player() {}
	Player(int x, int y, int w, int h, int s, SDL_Texture* tex);
	~Player() {}

	void update();
};
