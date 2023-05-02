#include "InvaderManager.h"
#include <cmath>
#include <iostream>

InvaderManager::InvaderManager(int rows, int cols, int winWidth, float spacingToOffsetRatio, int invaderSize, int projectileSpeed, SDL_Texture* invaderTex, SDL_Texture* projectileTex)
{
	initCols = cols;
	// xOffset = number of pixels between left side and first invader in col = number of pixels between right side and last invader in col
	// yOffset = number of pixels between top side and invaders in first row
	int xOffset, yOffset;
	// Number of pixels between invaders in x and y respectively
	int xSpacing, ySpacing;
	xSpacing = ySpacing = round(spacingToOffsetRatio*(winWidth - cols * invaderSize) /(2 + spacingToOffsetRatio*(cols - 1)));
	xOffset = xSpacing / spacingToOffsetRatio;
	yOffset = ySpacing / spacingToOffsetRatio;
	// Split up xOffset into multiple horizontal steps 
	xStep = 6*xOffset / (invaderSize);
	yStep = yOffset / 2;
	// Define left and right horizontal boundaries to maintain a minimum distance from each window side
	xBoundaryLeft = 6*xOffset % (invaderSize);
	xBoundaryRight = winWidth - (6*xOffset % (invaderSize));
	
	int xpos = xOffset;
	int ypos = yOffset;
	for (int i = 0; i < rows; ++i) {
		invaders.push_back(std::vector<Invader>{});
		for (int j = 0; j < cols; ++j) {
			invaders[i].push_back(Invader(xpos, ypos, 32, 32, projectileSpeed, invaderTex, projectileTex));
			xpos += invaderSize + xSpacing;
		}
		xpos = xOffset;
		ypos +=  invaderSize + ySpacing;
	}

	invaderMoveTimeStamp = invaderShotTimeStamp = SDL_GetTicks();
}

int InvaderManager::getNumRows()
{
	return invaders.size();
}

int InvaderManager::getNumCols(int i)
{
	return invaders[i].size();
}

SDL_Rect InvaderManager::getInvaderRect(int row, int col)
{
	return invaders[row][col].destRect;
}

void InvaderManager::deleteInvader(int row, int col)
{
	invaders[row].erase(invaders[row].begin() + col);
}

int InvaderManager::getNumProjectiles()
{
	return projectiles.size();
}

SDL_Rect InvaderManager::getProjectileRect(int i)
{
	return projectiles[i].destRect;
}

void InvaderManager::deleteProjectile(int i)
{
	projectiles.erase(projectiles.begin() + i);
}

void InvaderManager::update(SDL_Rect playerRect)
{
	
	bool invaderMoveReady = static_cast<float>(SDL_GetTicks() - invaderMoveTimeStamp) / 1000 > 1.5;
	bool moveInvaderDown = false;
	if (invaderMoveReady) {
		invaderMoveTimeStamp = SDL_GetTicks();
		// Check if any invader at the start or end of each row will be beyond a horizontal boundary after the next x step
		for (int i = 0; i < invaders.size(); ++i) {
			// Skip this row if all invaders have been deleted
			if (invaders[i].size() == 0) {
				continue;
			}
			int xNewStart = invaders[i][0].destRect.x + xStep;
			int endIndex = invaders[i].size() - 1;
			int xNewEnd = invaders[i][endIndex].destRect.x + invaders[i][endIndex].destRect.w + xStep;
			if (xNewStart < xBoundaryLeft || xNewEnd > xBoundaryRight) {
				moveInvaderDown = true;
			}
		}
		// Horizontal step direction needs to be reversed when the invaders move down
		if (moveInvaderDown) {
			xStep = -xStep;
		}
	}

	bool invaderShotReady = static_cast<float>(SDL_GetTicks() - invaderShotTimeStamp) / 1000 > 1.5;
	for (int i = 0; i < invaders.size(); ++i) {
		for (int j = 0; j < invaders[i].size(); ++j) {
			if (invaderShotReady) {
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
					for (int k = i + 1; k < invaders.size(); ++k) {
						for (int c = 0; c < invaders[k].size(); ++c) {
							// An invader below will be horizontally aligned with the current invader
							if (invaders[i][j].destRect.x == invaders[k][c].destRect.x) {
								invaderBelow = true;
							}
						}
					}
				}
				bool playerDirectlyBelow;
				int invaderMidPos = invaders[i][j].destRect.x + (invaders[i][j].destRect.w) / 2;
				if (playerRect.x < invaderMidPos && playerRect.x + playerRect.w > invaderMidPos) {
					playerDirectlyBelow = true;
				}
				else {
					playerDirectlyBelow = false;
				}

				if (!invaderBelow && playerDirectlyBelow) {
					projectiles.push_back(invaders[i][j].shoot());
					invaderShotTimeStamp = SDL_GetTicks();
				}
			}
			
			if (invaderMoveReady) {
				// Check if another invader has already reached a boundary and signalled that all invaders should only move down for this step
				if (moveInvaderDown) {
					invaders[i][j].destRect.y += yStep;
				}
				else {
					invaders[i][j].destRect.x += xStep;
				}
			}
		}
	}
	
	// Update positions of projectiles
	for (auto &p : projectiles) {
		p.update();
	}
}

void InvaderManager::render(SDL_Renderer* renderer)
{
	for (int i = 0; i < invaders.size(); ++i) {
		int numCols = invaders[i].size();
		for (int j = 0; j < numCols; ++j) { 
			SDL_RenderCopyEx(renderer, invaders[i][j].texture, &invaders[i][j].srcRect, &invaders[i][j].destRect, NULL, NULL, SDL_FLIP_NONE);
		}
	}

	// Render projectiles
	for (auto &p : projectiles) {
		SDL_RenderCopyEx(renderer, p.texture, NULL, &p.destRect, NULL, NULL, SDL_FLIP_NONE);
	}
}