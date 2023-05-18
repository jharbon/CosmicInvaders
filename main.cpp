#include "Game.h"
#include <iostream>

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	const int FPS = 60;
	const int frameDelay = 1000 / 60;

	Uint32 frameStart;
	int frameTime;

	// Get screen resolution
	SDL_DisplayMode dm;
	int foo = SDL_GetCurrentDisplayMode(0, &dm);
	int screenWidth = dm.w;
	int screenHeight = dm.h;

	Game* game = new Game("Cosmic Invaders", (float(1)/3)*screenWidth, (float(2)/3)*screenHeight);

	while (game->running) {

		frameStart = SDL_GetTicks();

		game->handleEvents();

		if (!game->gameOver) {
			game->update();
		}

		game->render();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
	
	return 0;
}