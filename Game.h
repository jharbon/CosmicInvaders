#pragma once

#include "SDL.h"

class Game
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

public:
	Game(const char* title, int width, int height);
	~Game() {}

	bool running = true;
	bool gameOver = false;

	void init();

	void handleEvents();
	void update();
	void render();

};
