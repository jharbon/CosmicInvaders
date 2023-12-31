#include "Game.h"
#include "SDL_image.h"
#include <string>
#include <algorithm>
#include "iostream"

const int NUM_INVADER_ROWS = 5;
const int NUM_INVADER_COLS = 8;
const int SCORE_PER_INVADER = 10;
const float PLAYER_RESPAWN_PERIOD = 2.0;
const float INVADER_WAVE_RESPAWN_PERIOD = 2.5;
const int PLAYER_PROJECTILE_SPEED = 450;
const int INVADER_PROJECTILE_SPEED = 500;

Game::Game(const char* title, int width, int height)
{
	winWidth = width;
	winHeight = height;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	backgroundTexture = loadTexture("assets/images/space_background.png");
	// Set the y coordinate value for the line we will render to separate the text from the player, bunkers and invaders above
	yTextBoundaryLine = 0.9 * winHeight;
	// Place player lives text in bottom left of window and below the player
	playerLivesRect.w = 0.25 * winWidth;
	playerLivesRect.h = 0.04 * winHeight;
	playerLivesRect.x = 0.05 * winWidth;
	playerLivesRect.y = 0.93 * winHeight;
	// Place score text in centre of window and below the player
	scoreRect.w = 0.25 * winWidth;
	scoreRect.h = 0.04 * winHeight;
	scoreRect.x = 0.7 * winWidth;
	scoreRect.y = 0.93 * winHeight;

	TTF_Init();
	font = TTF_OpenFont("assets/SF_Speakeasy.ttf", 30);

	Mix_OpenAudioDevice(48000, AUDIO_S16SYS, 2, 2048, NULL, 0);
	newInvadersWaveAudio = Mix_LoadWAV("assets/audio/new_invaders_wave.wav");
	invaderDeathAudio = Mix_LoadWAV("assets/audio/invader_death.wav");
	playerDeathAudio = Mix_LoadWAV("assets/audio/player_death.wav");
	gameOverAudio = Mix_LoadWAV("assets/audio/game_over.wav");

	invaderWave = 0;
	running = true;
	playing = false;
	gameOver = false;
}

void Game::init(int s, int wave)
{
	// Initialise score with a parameter so that it can be reset to zero after game over or carried over when we re-initiliase after defeating a wave of invaders
	score = s;

	SDL_Texture* playerTex = loadTexture("assets/images/player.png");
	SDL_Texture* projectileTex = loadTexture("assets/images/player_projectile.png");
	player = Player(0, 0.85*winHeight, 32, 32, 125, PLAYER_PROJECTILE_SPEED, playerTex, projectileTex);

	// Invader wave number can either be reset to zero or carried over for the next wave 
	invaderWave = wave;
	int yOffset = (0.1 + 0.02*invaderWave) * winHeight;
	if (yOffset > 0.25 * winHeight) {
		// Set a maximum y offset for the invaders
		yOffset = 0.25 * winHeight; 
	}
	++invaderWave;
	SDL_Texture* invaderTex1 = loadTexture("assets/images/invader_1.png");
	SDL_Texture* invaderTex2 = loadTexture("assets/images/invader_2.png");
	SDL_Texture* invaderTex3 = loadTexture("assets/images/invader_3.png");
	std::vector<SDL_Texture*> invaderTexVec{ invaderTex1, invaderTex2, invaderTex3 };
	SDL_Texture* invaderProjectileTex = loadTexture("assets/images/invader_projectile.png");
	invaderManager = InvaderManager(NUM_INVADER_ROWS, NUM_INVADER_COLS, 0.5, winWidth, yOffset, 32, INVADER_PROJECTILE_SPEED, invaderTexVec, invaderProjectileTex);

	SDL_Texture* bunkerBlockTex = loadTexture("assets/images/bunker_block.png");
	bunkerManager = BunkerManager(0.75*winHeight, 0.1*winWidth, 4, 1, winWidth, bunkerBlockTex);

	Mix_PlayChannel(-1, newInvadersWaveAudio, 0);
}

SDL_Texture* Game::loadTexture(const char* imgPath)
{
	SDL_Surface* tmpSurface = IMG_Load(imgPath);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	return tex;
}

void Game::handleMenuInput(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
		if (!playing) {
			// Start the game from the main menu
			playing = true;
			init(0, 0);
		}
		else if (gameOver) {
			// Restart the game after game over
			gameOver = false;
			init(0, 0);
		}
	}
}

void Game::handlePlayerInput(SDL_Event &event)
{
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_LEFT:
			player.setVelocityDirection(-1);
			break;
		case SDLK_RIGHT:
			player.setVelocityDirection(1);
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
			player.setVelocityDirection(0);
			break;
		case SDLK_RIGHT:
			player.setVelocityDirection(0);
		default:
			break;
		}
	}
}

void Game::update()
{
	if (playerRespawning) {
		if (static_cast<float>(SDL_GetTicks() - playerDeathTimeStamp) / 1000 < PLAYER_RESPAWN_PERIOD) {
			// Do not update anything until the respawn time period is over
			return;
		}
		else {
			playerRespawning = false;
			player.setPosition(0);
			player.setVelocityDirection(0);
			// Small delay before invaders start shooting again
			invaderManager.resetShotTimeStamp();
		}
	}

	player.update();
	SDL_Rect playerRect = player.getRect();
	// Check if player has moved out of bounds and move them back if necessary
	if (playerRect.x < 0) {
		player.setPosition(0);
	}
	else if (playerRect.x + playerRect.w > winWidth) {
		player.setPosition(winWidth - playerRect.w);
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
					Mix_PlayChannel(-1, invaderDeathAudio, 0);
					score += SCORE_PER_INVADER;
				}
			}
		}

		if (deleteProjectile) {
			player.deleteProjectile(i);
		}
	}

	if (invaderWaveRespawning) {
		if (static_cast<float>(SDL_GetTicks() - invaderWaveEndTimestamp) / 1000 < INVADER_WAVE_RESPAWN_PERIOD) {
			// End the updating here until the invaders respawn time period is over
			return;
		}
		else {
			invaderWaveRespawning = false;
			// Initialise a new wave
			init(score, invaderWave);
		}
	}

	invaderManager.update(playerRect);

	// Check if player has destroyed all invaders
	if (invaderManager.getNumInvaders() == 0) {
		invaderWaveRespawning = true;
		invaderWaveEndTimestamp = SDL_GetTicks();
		return;
	}

	// Check if any invaders in the bottom row have collided with the player or crossed the text boundary line
	int numInvaderRows = invaderManager.getNumRows();
	int numInvaderBottomRowCols = invaderManager.getNumCols(numInvaderRows - 1);
	for (int c = 0; c < numInvaderBottomRowCols; ++c) {
		SDL_Rect invaderRect = invaderManager.getInvaderRect(numInvaderRows - 1, c);
		if (AABBcollision(player.getRect(), invaderRect)) {
			gameOver = true;
		}
		else if (invaderRect.y + invaderRect.h > yTextBoundaryLine) {
			gameOver = true;
		}
	}

	for (int i = 0; i < invaderManager.getNumProjectiles(); ++i) {
		SDL_Rect projectileRect = invaderManager.getProjectileRect(i);
		bool deleteProjectile = false;
		// Delete this projectile if it has hit the text boundary line
		if (projectileRect.y + projectileRect.h > yTextBoundaryLine) {
			deleteProjectile = true;
		}
		// Check if this projectile has hit the player
		else if (AABBcollision(projectileRect, playerRect)) {
			player.loseLife();
			playerDeathTimeStamp = SDL_GetTicks();
			playerRespawning = true;
			deleteProjectile = true;
			if (player.getNumLives() == 0) {
				// End the game if player has lost all their lives
				gameOver = true;
				Mix_PlayChannel(-1, gameOverAudio, 0);
			}
			else {
				Mix_PlayChannel(-1, playerDeathAudio, 0);
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

	// Check for projectile-bunker and invader-bunker collisions
	std::vector<int> bunkerIndsForDeletion;
	for (int i = 0; i < bunkerManager.getNumBunkers(); ++i) {
		bool invaderHitBunker = false;
		for (int j = 0; j < bunkerManager.getBunkerRows(i); ++j) {
			for (int k = 0; k < bunkerManager.getBunkerCols(i, j); ++k) {
				SDL_Rect blockRect = bunkerManager.getBlockRect(i, j, k);

				// Delete this whole bunker if any invader in the bottom row is touching the current block
				int numInvaderRows = invaderManager.getNumRows();
				for (int c = 0; c < invaderManager.getNumCols(numInvaderRows - 1); ++c) {
					SDL_Rect invaderRect = invaderManager.getInvaderRect(numInvaderRows - 1, c);
					if (AABBcollision(invaderRect, blockRect)) {
						invaderHitBunker = true;
						break;
					}
				}

				bool blockHit = false;
				// Player projectiles check
				for (int p = 0; p < player.getNumProjectiles(); ++p) {
					if (AABBcollision(player.getProjectileRect(p), blockRect)) {
						blockHit = true;
						player.deleteProjectile(p);
					}
				}
				// Invader projectiles check
				for (int p = 0; p < invaderManager.getNumProjectiles(); ++p) {
					if (AABBcollision(invaderManager.getProjectileRect(p), blockRect)) {
						blockHit = true;
						invaderManager.deleteProjectile(p);
					}
				}
				if (blockHit) {
					bunkerManager.deleteBlock(i, j, k);
				}
			}
		}
		if (invaderHitBunker) {
			bunkerIndsForDeletion.push_back(i);
		}
	}
	// Delete any bunkers which were marked for deletion
	std::sort(bunkerIndsForDeletion.begin(), bunkerIndsForDeletion.end(), std::greater<int>()); // Sort in descending order
	for (int i : bunkerIndsForDeletion) {
		bunkerManager.deleteBunker(i);
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

	// Render background
	SDL_Rect backgroundRect = { 0, 0, winWidth, yTextBoundaryLine };
	SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
	// Render the line which separates the text at the bottom from the player, bunker and invaders above
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, 0, yTextBoundaryLine, winWidth, yTextBoundaryLine);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	if (!playing) {
		SDL_Surface* titleSurf = TTF_RenderUTF8_Solid(font, "Cosmic Invaders", white);
		SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurf);
		SDL_Rect titleRect = { 0.05 * winWidth, 0.325 * winHeight, 0.9 * winWidth, 0.2 * winHeight };
		SDL_FreeSurface(titleSurf);
		SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

		SDL_Surface* instructSurf = TTF_RenderUTF8_Solid(font, "Press space to play", white);
		SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(renderer, instructSurf);
		SDL_Rect instructRect = { 0.2 * winWidth, 0.6 * winHeight, 0.6 * winWidth, 0.025 * winHeight };
		SDL_FreeSurface(instructSurf);
		SDL_RenderCopy(renderer, instructTexture, NULL, &instructRect);
	}
	else if (gameOver) {
		SDL_Surface* gameOverSurf = TTF_RenderUTF8_Solid(font, "Game Over", white);
		SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurf);
		SDL_Rect gameOverRect = { 0.05 * winWidth, 0.375 * winHeight, 0.9 * winWidth, 0.15 * winHeight };
		SDL_FreeSurface(gameOverSurf);
		SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

		SDL_Surface* instructSurf = TTF_RenderUTF8_Solid(font, "Press space to play again", white);
		SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(renderer, instructSurf);
		SDL_Rect instructRect = { 0.1 * winWidth, 0.6 * winHeight, 0.8 * winWidth, 0.025 * winHeight };
		SDL_FreeSurface(instructSurf);
		SDL_RenderCopy(renderer, instructTexture, NULL, &instructRect);
	}
	else {
		// Render text which displays the current number of player lives
		std::string livesStr = "Lives: " + std::to_string(player.getNumLives());
		SDL_Surface* livesSurf = TTF_RenderUTF8_Solid(font, livesStr.c_str(), white);
		SDL_Texture* livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurf);
		SDL_FreeSurface(livesSurf);
		SDL_RenderCopy(renderer, livesTexture, NULL, &playerLivesRect);

		if (!playerRespawning) {
			player.render(renderer);
		}
		invaderManager.render(renderer);
		bunkerManager.render(renderer);
	}

	if (playing) {
		// During either an active wave or the game over screen, render text which displays the current player score
		std::string scoreStr = "Score: " + std::to_string(score);
		SDL_Surface* scoreSurf = TTF_RenderUTF8_Solid(font, scoreStr.c_str(), white);
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurf);
		SDL_FreeSurface(scoreSurf);
		SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
	}
	
	SDL_RenderPresent(renderer);
}