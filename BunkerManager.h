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

	int getNumBunkers();
	int getBunkerRows(int i);
	int getBunkerCols(int i, int j);
	SDL_Rect getBlockRect(int i, int j, int k);
	void deleteBlock(int i, int j, int k);

	void render(SDL_Renderer* renderer);
};