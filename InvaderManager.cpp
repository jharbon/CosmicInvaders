#include "InvaderManager.h"
#include <cmath>
#include <iostream>

const float INIT_MOVEMENT_WAIT_PERIOD = 1.5;
const float INIT_SHOT_WAIT_PERIOD = 1.5;

InvaderManager::InvaderManager(int rows, int cols, float spacingToOffsetRatio, int winWidth, int yOffset, int invaderSize, int projectileSpeed, std::vector<SDL_Texture*> invaderTexVec, SDL_Texture* projectileTex)
{
	initRows = rows;
	initCols = cols;
	// xOffset = number of pixels between left side and first invader in col = number of pixels between right side and last invader in col
	// yOffset = number of pixels between top side and invaders in first row
	int xOffset;
	// Number of pixels between invaders in x and y respectively
	int xSpacing, ySpacing;
	xSpacing = ySpacing = round(spacingToOffsetRatio*(winWidth - cols * invaderSize) /(2 + spacingToOffsetRatio*(cols - 1)));
	xOffset = xSpacing / spacingToOffsetRatio;
	// Split up xOffset into multiple horizontal steps 
	xStep = 6*xOffset / (invaderSize);
	yStep = yOffset / 3;
	// Define left and right horizontal boundaries to maintain a minimum distance from each window side
	xBoundaryLeft = 6*xOffset % (invaderSize);
	xBoundaryRight = winWidth - (6*xOffset % (invaderSize));

	// Determine how many rows to allocate for each texture 
	std::vector<int> rowsPerTex{};
	if (invaderTexVec.size() == 1) {
		rowsPerTex.push_back(rows);
	}
	else {
		// Give first texture only 1 row if we have more than 1 texture
		rowsPerTex.push_back(1);
		// Find int number of rows per texture we can now allocate with remaining rows
		int divResult = (rows - 1) / (invaderTexVec.size() - 1);
		for (int i = 1; i < invaderTexVec.size(); ++i) {
			rowsPerTex.push_back(divResult);
		}
		// Add the remainder from the division to the rows value corresponding to the last texture
		rowsPerTex[invaderTexVec.size() - 1] += (rows - 1) % (invaderTexVec.size() - 1);
	}
	
	int xpos = xOffset;
	int ypos = yOffset;
	for (int i = 0; i < rows; ++i) {
		invaders.push_back(std::vector<Invader>{});

		// Determine which texture we should be using for the current row
		int rowsSum = 0;
		SDL_Texture* invaderTex = nullptr;
		for (int r = 0; r < rowsPerTex.size(); ++r) {
			rowsSum += rowsPerTex[r];
			if (i < rowsSum) {
				invaderTex = invaderTexVec[r];
				break;
			}
		}

		for (int j = 0; j < cols; ++j) {
			invaders[i].push_back(Invader(xpos, ypos, 32, 32, projectileSpeed, invaderTex, projectileTex));
			xpos += invaderSize + xSpacing;
		}
		xpos = xOffset;
		ypos +=  invaderSize + ySpacing;
	}

	movementTimeStamp = shotTimeStamp = SDL_GetTicks();
	movementWaitPeriod = INIT_MOVEMENT_WAIT_PERIOD;
	shotWaitPeriod = INIT_SHOT_WAIT_PERIOD;
}

int InvaderManager::getNumRows()
{
	return invaders.size();
}

int InvaderManager::getNumCols(int row)
{
	return invaders[row].size();
}

int InvaderManager::getNumInvaders()
{
	int numInvaders = 0;
	for (int r = 0; r < getNumRows(); ++r) {
		numInvaders += getNumCols(r);
	}

	return numInvaders;
}

SDL_Rect InvaderManager::getInvaderRect(int row, int col)
{
	return invaders[row][col].destRect;
}

void InvaderManager::deleteInvader(int row, int col)
{
	invaders[row].erase(invaders[row].begin() + col);

	// Find fraction of invaders left
	int initNumInvaders = initRows * initCols;
	int numInvaders = getNumInvaders();
	float fracInvadersLeft = static_cast<float>(numInvaders) / initNumInvaders;

	// Decrease the movement wait period each time
	movementWaitPeriod = fracInvadersLeft * INIT_MOVEMENT_WAIT_PERIOD;	
}

void InvaderManager::resetShotTimeStamp()
{
	shotTimeStamp = SDL_GetTicks();
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
	// Delete any row vectors where all invaders in that row have been destroyed
	invaders.erase(std::remove_if(invaders.begin(), invaders.end(), [](std::vector<Invader> v) { return v.size() == 0; }), invaders.end());
	
	bool invaderMoveReady = static_cast<float>(SDL_GetTicks() - movementTimeStamp) / 1000 > movementWaitPeriod;
	bool moveInvaderDown = false;
	if (invaderMoveReady) {
		movementTimeStamp = SDL_GetTicks();
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

	bool invaderShotReady = static_cast<float>(SDL_GetTicks() - shotTimeStamp) / 1000 > shotWaitPeriod;
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
					shotTimeStamp = SDL_GetTicks();
				}
			}
			
			if (invaderMoveReady) {
				// Check if another invader has already reached a boundary and signalled that all invaders should only move down for this step
				if (moveInvaderDown) {
					invaders[i][j].moveStep(0, yStep);
				}
				else {
					invaders[i][j].moveStep(xStep, 0);
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