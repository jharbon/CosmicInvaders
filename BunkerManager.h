#pragma once

#include "SDL.h"
#include <vector>

class BunkerManager
{
private:
	SDL_Texture* bunkerBlockTexture;
	std::vector<std::vector<std::vector<SDL_Rect>>> bunkers;
public:
	BunkerManager() {}
	BunkerManager(int yPosBunk, int bunkerWidth, int numBunkers, float spacingToOffsetRatio, int winWidth, SDL_Texture* bunkerBlockTex);
	~BunkerManager() {}

	void update();
	void render(SDL_Renderer* renderer);
};