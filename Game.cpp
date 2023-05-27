#include "Game.h"
#include "SDL_image.h"
#include <string>
//#include <format>
#include "iostream"

const int SCORE_PER_INVADER = 10;
const int PLAYER_PROJECTILE_SPEED = 450;
const int INVADER_PROJECTILE_SPEED = 500;

Game::Game(const char* title, int width, int height)
{
	winWidth = width;
	winHeight = height;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	TTF_Init();
	font = TTF_OpenFont("assets/SF_Speakeasy.ttf", 30);

	init(0);
}

void Game::init(int s)
{
	// Place player lives text in bottom left of window and below the player
	playerLivesRect.w = 0.25 * winWidth;
	playerLivesRect.h = 0.04 * winHeight;
	playerLivesRect.x = 0.05 * winWidth;
	playerLivesRect.y = 0.93 * winHeight;
	// Initialise score with a parameter so that it can be carried over when we re-initiliase for e.g. a new wave of invaders
	score = s;
	// Place score text in centre of window and below the player
	scoreRect.w = 0.25 * winWidth;
	scoreRect.h = 0.04 * winHeight;
	scoreRect.x = 0.7 * winWidth;
	scoreRect.y = 0.93 * winHeight;
	// Set the y coordinate value for the line we will render to separate the text from the player, bunkers and invaders above
	yTextBoundaryLine = 0.9 * winHeight;

	SDL_Texture* playerTex = loadTexture("assets/player.png");
	SDL_Texture* projectileTex = loadTexture("assets/player_projectile.png");
	player = Player(0, 0.85*winHeight, 64, 32, 125, PLAYER_PROJECTILE_SPEED, playerTex, projectileTex);

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

void Game::handlePlayerInput(SDL_Event &event)
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
					score += SCORE_PER_INVADER;
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
		// Delete this projectile if it has hit the text boundary line
		if (projectileRect.y + projectileRect.h > yTextBoundaryLine) {
			deleteProjectile = true;
		}
		// Check if this projectile has hit the player
		else if (AABBcollision(projectileRect, player.destRect)) {
			player.loseLife();
			deleteProjectile = true;
			if (player.getNumLives() == 0) {
				// End the game if player has lost all their lives
				gameOver = true;
			}
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

	// Check for projectile-bunker collisions
	for (int i = 0; i < bunkerManager.getNumBunkers(); ++i) {
		for (int j = 0; j < bunkerManager.getBunkerRows(i); ++j) {
			for (int k = 0; k < bunkerManager.getBunkerCols(i, j); ++k) {
				bool blockHit = false;
				// Player projectiles check
				for (int p = 0; p < player.getNumProjectiles(); ++p) {
					if (AABBcollision(player.getProjectileRect(p), bunkerManager.getBlockRect(i, j, k))) {
						blockHit = true;
						player.deleteProjectile(p);
					}
				}
				// Invader projectiles check
				for (int p = 0; p < invaderManager.getNumProjectiles(); ++p) {
					if (AABBcollision(invaderManager.getProjectileRect(p), bunkerManager.getBlockRect(i, j, k))) {
						blockHit = true;
						invaderManager.deleteProjectile(p);
					}
				}
				if (blockHit) {
					bunkerManager.deleteBlock(i, j, k);
				}
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
	SDL_Color white = { 255,255,255 };

	// Render text which displays the current number of player lives
	std::string livesStr = "Lives: " + std::to_string(player.getNumLives());
	SDL_Surface* livesSurf = TTF_RenderUTF8_Solid(font, livesStr.c_str(), white);
	SDL_Texture* livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurf);
	SDL_FreeSurface(livesSurf);
	SDL_RenderCopy(renderer, livesTexture, NULL, &playerLivesRect);

	// Render text which displays the current player score
	std::string scoreStr = "Score: " + std::to_string(score);
	SDL_Surface* scoreSurf = TTF_RenderUTF8_Solid(font, scoreStr.c_str(), white);
	SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurf);
	SDL_FreeSurface(scoreSurf);
	SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

	// Render the line which separates the text at the bottom from the player, bunker and invaders above
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, 0, yTextBoundaryLine, winWidth, yTextBoundaryLine);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	player.render(renderer);
	invaderManager.render(renderer);
	bunkerManager.render(renderer);

	SDL_RenderPresent(renderer);
}