#include "Game.h"
#include "SDL_image.h"
#include "iostream"

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
	SDL_Texture* projectileTex = loadTexture("assets/player_projectile.png");
	player = Player(0, 0.9*winHeight, 64, 32, 125, playerTex, projectileTex);
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
		case SDLK_SPACE:
			player.shoot();
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
	for (int i = 0; i < player.projectiles.size(); ++i) {
		Projectile* ptr = player.projectiles[i];
		if (ptr->destRect.y < 0) {
			delete ptr;
			player.projectiles.erase(player.projectiles.begin() + i);
		}
	}
}

void Game::render()
{
	SDL_RenderClear(renderer);

	SDL_RenderCopyEx(renderer, player.texture, &player.srcRect, &player.destRect, NULL, NULL, SDL_FLIP_NONE);
	for (auto p : player.projectiles) {
		SDL_RenderCopyEx(renderer, p->texture, NULL, &p->destRect, NULL, NULL, SDL_FLIP_NONE);
	}

	SDL_RenderPresent(renderer);
}