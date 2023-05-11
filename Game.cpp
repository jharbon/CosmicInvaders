#include "Game.h"
#include "SDL_image.h"
#include "iostream"

const int PLAYER_PROJECTILE_SPEED = 450;
const int INVADER_PROJECTILE_SPEED = 500;

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
	player = Player(0, 0.9*winHeight, 64, 32, 125, PLAYER_PROJECTILE_SPEED, playerTex, projectileTex);

	SDL_Texture* invaderTex = loadTexture("assets/invader.png");
	SDL_Texture* invaderProjectileTex = loadTexture("assets/invader_projectile.png");
	invaderManager = InvaderManager(5, 8, winWidth, 0.5, 32, INVADER_PROJECTILE_SPEED, invaderTex, invaderProjectileTex);

	SDL_Texture* bunkerBlockTex = loadTexture("assets/bunker_block.png");
	bunkerManager = BunkerManager(0.75*winHeight, 0.1*winWidth, 4, 1, winWidth, bunkerBlockTex);
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

	for (int i = 0; i < player.getNumProjectiles(); ++i) {
		SDL_Rect projectileRect = player.getProjectileRect(i);
		bool deleteProjectile = false;
		// Delete this projectile if it has moved out of bounds
		if (projectileRect.y < 0) {
			deleteProjectile = true;
		}
		// Delete an invader if it has been hit by this projectile
		for (int j = 0; j < invaderManager.getNumRows(); ++j) {
			for (int k = 0; k < invaderManager.getNumCols(j); ++k) {
				if (AABBcollision(projectileRect, invaderManager.getInvaderRect(j, k))) {
					invaderManager.deleteInvader(j, k);
					deleteProjectile = true;
				}
			}
		}

		if (deleteProjectile) {
			player.deleteProjectile(i);
		}
	}

	invaderManager.update(player.destRect);

	for (int i = 0; i < invaderManager.getNumProjectiles(); ++i) {
		SDL_Rect projectileRect = invaderManager.getProjectileRect(i);
		bool deleteProjectile = false;
		// Delete this projectile if it has moved out of bounds
		if (projectileRect.y > winHeight) {
			deleteProjectile = true;
		}
		// Check if this projectile has hit the player
		else if (AABBcollision(projectileRect, player.destRect)) {
			deleteProjectile = true;
		}

		if (deleteProjectile) {
			invaderManager.deleteProjectile(i);
		}
	}

	// Check for projectile-projectile collisions
	for (int i = 0; i < player.getNumProjectiles(); ++i) {
		for (int j = 0; j < invaderManager.getNumProjectiles(); ++j) {
			if (AABBcollision(player.getProjectileRect(i), invaderManager.getProjectileRect(j))) {
				player.deleteProjectile(i);
				invaderManager.deleteProjectile(j);
			}
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

	player.render(renderer);
	invaderManager.render(renderer);
	bunkerManager.render(renderer);

	SDL_RenderPresent(renderer);
}