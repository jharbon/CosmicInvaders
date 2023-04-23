#include "InvaderManager.h"
#include <iostream>

InvaderManager::InvaderManager(int rows, int cols, int winWidth, float spacingToOffsetRatio, int invaderSize, SDL_Texture* invaderTex, SDL_Texture* projectileTex)
{
	// xOffset = number of pixels between left side and first invader in col = number of pixels between right side and last invader in col
	// yOffset = number of pixels between top side and invaders in first row
	int xOffset, yOffset;
	// Number of pixels between invaders in x and y respectively
	int xSpacing, ySpacing;
	xSpacing = ySpacing = spacingToOffsetRatio*(winWidth - cols * invaderSize) /(2 + spacingToOffsetRatio*(cols - 1));
	xOffset = xSpacing / spacingToOffsetRatio;
	yOffset = ySpacing / spacingToOffsetRatio;
	
	int xpos = xOffset;
	int ypos = yOffset;
	for (int i = 0; i < rows; ++i) {
		invaders.push_back(std::vector<Invader*>{});
		for (int j = 0; j < cols; ++j) {
			invaders[i].push_back(new Invader(xpos, ypos, 32, 32, invaderTex, projectileTex));
			xpos += invaderSize + xSpacing;
		}
		xpos = xOffset;
		ypos +=  invaderSize + ySpacing;
	}
}

void InvaderManager::update()
{

}

void InvaderManager::render(SDL_Renderer* renderer)
{
	for (int i = 0; i < invaders.size(); ++i) {
		int numCols = invaders[i].size();
		for (int j = 0; j < numCols; ++j) { 
			SDL_RenderCopyEx(renderer, invaders[i][j]->texture, &invaders[i][j]->srcRect, &invaders[i][j]->destRect, NULL, NULL, SDL_FLIP_NONE);
		}
	}
}