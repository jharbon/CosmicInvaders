#pragma once

#include "SDL.h"
#include "Player.h"

class Game
{
private:
	int winWidth, winHeight;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

public:
	Game(const char* title, int width, int height);
	~Game() {}

	bool running = true;
	bool gameOver = false;

	Player player;

	void init();
	SDL_Texture* loadTexture(const char* imgPath);

	void handleEvents();
	void handlePlayerInput();

	void update();
	void render();

};
