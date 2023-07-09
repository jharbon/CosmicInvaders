#pragma once

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "Player.h"
#include "Projectile.h"
#include <vector>
#include "Invader.h"
#include "InvaderManager.h"
#include "BunkerManager.h"

class Game
{
private:
	int winWidth, winHeight;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;

	Mix_Chunk* newInvadersWaveAudio;
	Mix_Chunk* invaderDeathAudio;
	Mix_Chunk* playerDeathAudio;
	Mix_Chunk* gameOverAudio;

	int yTextBoundaryLine;
	int score;
	SDL_Rect scoreRect;
	SDL_Rect playerLivesRect;

public:
	Game(const char* title, int width, int height);
	~Game() {}

	bool running = true;
	bool gameOver = false;

	Player player;
	int playerDeathTimeStamp;
	bool playerRespawning;

	int invaderWave;
	InvaderManager invaderManager;
	BunkerManager bunkerManager;

	void init(int s);
	SDL_Texture* loadTexture(const char* imgPath);

	void handlePlayerInput(SDL_Event &event);
	void update();
	static bool AABBcollision(SDL_Rect projectileRect, SDL_Rect targetRect);

	void render();
};
