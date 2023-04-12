#include "Game.h"
#include "SDL_image.h"

Game::Game(const char* title, int width, int height)
{
	winWidth = width;
	winHeight = height;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	init();
}

void Game::init()
{
	SDL_Texture* playerTex = loadTexture("assets/player.png");
	player = Player(0, 0.9*winHeight, 64, 32, 125, playerTex);
}

SDL_Texture* Game::loadTexture(const char* imgPath)
{
	SDL_Surface* tmpSurface = IMG_Load(imgPath);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	return tex;
}

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		running = false;
	}
	else {
		handlePlayerInput();
	}
}

void Game::handlePlayerInput()
{
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_LEFT:
			player.xvel = -player.speed;
			break;
		case SDLK_RIGHT:
			player.xvel = player.speed;
			break;
		default:
			break;
		}
	}
	else if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		case SDLK_LEFT:
			player.xvel = 0;
			break;
		case SDLK_RIGHT:
			player.xvel = 0;
		default:
			break;
		}
	}
}

void Game::update()
{
	player.update();
}

void Game::render()
{
	SDL_RenderClear(renderer);

	SDL_RenderCopyEx(renderer, player.texture, &player.srcRect, &player.destRect, NULL, NULL, SDL_FLIP_NONE);

	SDL_RenderPresent(renderer);
}