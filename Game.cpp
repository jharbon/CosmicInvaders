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

	SDL_Texture* invaderTex = loadTexture("assets/invader.png");
	SDL_Texture* invaderProjectileTex = loadTexture("assets/invader_projectile.png");
	invaderManager = InvaderManager(5, 8, winWidth, 0.5, 32, invaderTex, invaderProjectileTex);
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
	// Check if player has moved out of bounds and move them back if necessary
	if (player.destRect.x < 0) {
		player.xpos = player.destRect.x = 0;
	}
	else if (player.destRect.x + player.destRect.w > winWidth) {
		player.xpos = player.destRect.x = winWidth - player.destRect.w;
	}

	for (int i = 0; i < player.projectiles.size(); ++i) {
		Projectile* ptr = player.projectiles[i];
		// Delete any projectile if it has moved out of bounds
		if (ptr->destRect.y < 0) {
			delete ptr;
			player.projectiles.erase(player.projectiles.begin() + i);
		}
	}
}

bool Game::AABBcollision(SDL_Rect projectileRect, SDL_Rect targetRect)
{
	if (projectileRect.x < targetRect.x + targetRect.w &&
		projectileRect.x + projectileRect.w > targetRect.x &&
		projectileRect.y < targetRect.y + targetRect.h &&
		projectileRect.y + projectileRect.h > targetRect.y) {
		return true;
	}
	else {
		return false;
	}
	
}

void Game::render()
{
	SDL_RenderClear(renderer);

	SDL_RenderCopyEx(renderer, player.texture, &player.srcRect, &player.destRect, NULL, NULL, SDL_FLIP_NONE);
	for (auto p : player.projectiles) {
		SDL_RenderCopyEx(renderer, p->texture, NULL, &p->destRect, NULL, NULL, SDL_FLIP_NONE);
	}

	invaderManager.render(renderer);

	SDL_RenderPresent(renderer);
}