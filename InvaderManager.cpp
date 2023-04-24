#include "InvaderManager.h"
#include <iostream>

InvaderManager::InvaderManager(int rows, int cols, int winWidth, float spacingToOffsetRatio, int invaderSize, SDL_Texture* invaderTex, SDL_Texture* projectileTex)
{
	initCols = cols;
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

void InvaderManager::update(SDL_Rect playerRect)
{
	bool invaderShotReady = static_cast<float>(SDL_GetTicks() - invaderShotTimeStamp) / 1000 > 1.5;
	if (invaderShotReady) {
		for (int i = 0; i < invaders.size(); ++i) {
			for (int j = 0; j < invaders[i].size(); ++j) {
				bool invaderBelow;
				// First check if we are in the bottom row
				if (i == invaders.size() - 1) {
					invaderBelow = false;
				}
				// Check if row below has not lost any invaders yet
				else if (invaders[i + 1].size() == initCols) {
					invaderBelow = true;
				}
				else {
					// Check if any invaders in the rows below are directly below the current invader  
					invaderBelow = false;
					for (int k = i+1; k < invaders.size(); ++k) {
						for (int c = 0; c < invaders[k].size(); ++c) {
							// An invader below will be horizontally aligned with the current invader
							if (invaders[i][j]->destRect.x == invaders[k][c]->destRect.x) {
								invaderBelow = true;
							}
						}
					}
				}
				bool playerDirectlyBelow = false;
				int invaderMidPos = invaders[i][j]->destRect.x + (invaders[i][j]->destRect.w) / 2;
				if (playerRect.x < invaderMidPos && playerRect.x + playerRect.w > invaderMidPos) {
					playerDirectlyBelow = true;
				}

				if (!invaderBelow && playerDirectlyBelow) {
					projectiles.push_back(invaders[i][j]->shoot());
					invaderShotTimeStamp = SDL_GetTicks();
				}
			}
		}
	}

	// Update positions of projectiles
	for (auto p : projectiles) {
		p->update();
	}
}

void InvaderManager::render(SDL_Renderer* renderer)
{
	for (int i = 0; i < invaders.size(); ++i) {
		int numCols = invaders[i].size();
		for (int j = 0; j < numCols; ++j) { 
			SDL_RenderCopyEx(renderer, invaders[i][j]->texture, &invaders[i][j]->srcRect, &invaders[i][j]->destRect, NULL, NULL, SDL_FLIP_NONE);
		}
	}

	// Render projectiles
	for (auto p : projectiles) {
		SDL_RenderCopyEx(renderer, p->texture, NULL, &p->destRect, NULL, NULL, SDL_FLIP_NONE);
	}
}