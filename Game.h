#pragma once

#include "SDL.h"
#include "Player.h"
#include "Projectile.h"
#include <vector>
#include "Invader.h"

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
	Invader invader;
	int invaderShotTimeStamp;

	void init();
	SDL_Texture* loadTexture(const char* imgPath);

	void handleEvents();
	void handlePlayerInput();

	void update();

	void render();
};
