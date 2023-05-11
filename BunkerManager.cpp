#include "BunkerManager.h"

BunkerManager::BunkerManager(int yPosBunk, int bunkerWidth, int numBunkers, float spacingToOffsetRatio, int winWidth, SDL_Texture* bunkerBlockTex)
	: bunkerBlockTexture{ bunkerBlockTex }
{
	int bunkerBlockSize = 8;
	// Adjust the bunker width so that we can fit a whole number of blocks into it 
	bunkerWidth -= (bunkerWidth % bunkerBlockSize);
	// Find how many blocks fit in a given row of the widest part of a bunker
	int baseNumBlocks = bunkerWidth / bunkerBlockSize;

	// Determine how much we should offset the first and last bunker from the horizontal boundaries and how much space to maintain between each bunker
	int xOffset, xSpacing;
	xSpacing = round(spacingToOffsetRatio * (winWidth - numBunkers * bunkerWidth) / (2 + spacingToOffsetRatio * (numBunkers - 1)));
	xOffset = xSpacing / spacingToOffsetRatio;

	// Make each bunker and append it to our vector of bunkers
	for (int i = 0; i < numBunkers; ++i) {
		std::vector<std::vector<SDL_Rect>> bunk;
		int xPosBunk = xOffset + i * (bunkerWidth + xSpacing);

		// Make two centered rows of blocks where the width of each row is half the bunker width
		for (int j = 0; j < 2; ++j) {
			std::vector<SDL_Rect> row;
			for (int k = 0; k < baseNumBlocks / 2; ++k) {
				SDL_Rect block;
				block.x = xPosBunk + 0.25*bunkerWidth + k*bunkerBlockSize;
				block.y = yPosBunk + j*bunkerBlockSize;
				block.w = block.h = bunkerBlockSize;
				row.push_back(block);
			}
			bunk.push_back(row);
		}

		// Make 4 rows of blocks where the width of each row is the full bunker width
		for (int j = 2; j < 6; ++j) {
			std::vector<SDL_Rect> row;
			for (int k = 0; k < baseNumBlocks; ++k) {
				SDL_Rect block;
				block.x = xPosBunk + k * bunkerBlockSize;
				block.y = yPosBunk + j * bunkerBlockSize;
				block.w = block.h = bunkerBlockSize;
				row.push_back(block);
			}
			bunk.push_back(row);
		}

		bunkers.push_back(bunk);
	}
}

void BunkerManager::update()
{

}

void BunkerManager::render(SDL_Renderer* renderer)
{
	for (int i = 0; i < bunkers.size(); ++i) {
		for (int j = 0; j < bunkers[i].size(); ++j) {
			for (int k = 0; k < bunkers[i][j].size(); ++k) {
				SDL_RenderCopyEx(renderer, bunkerBlockTexture, NULL, &bunkers[i][j][k], NULL, NULL, SDL_FLIP_NONE);
			}
		}
	}
}