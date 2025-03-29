#include "../include/menuutils.h"

void handleMenu(bool& inMenu, bool& inLevelSelect, bool& helpScreen, bool& inExitOption, int mouseX, int mouseY, Mix_Chunk* mouse_click) {
    if (mouseX >= 410 && mouseX <= 575 && mouseY >= 345 && mouseY <= 385) {
        inMenu = false;
        inLevelSelect = true;
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 405 && mouseY <= 445) {
        inMenu = false;
        helpScreen = true;
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 465 && mouseY <= 505) {
        inExitOption = true;
        inMenu = false;
        Mix_PlayChannel(-1, mouse_click, 0);
    }
}

void handleLevelSelect(bool& inMenu, bool& inLevelSelect, bool& gameRunning, GameLevel& selectedLevel, int& enemySpeed, Uint32& enemyShootInterval,
                       int& enemyCount, int& planetWave, vector<SmallEnemy>& enemies, vector<Planet>& planets, vector<Health>& healthItems,
                       vector<Coin>& coins, vector<SpreadShot>& spreadShots, int& enemiesSpawned, SDL_Texture* enemyTexture,
                       SDL_Texture* planet1SpriteSheetTexture, SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture,
                       int mouseX, int mouseY, Mix_Chunk* mouse_click, SDL_Event& event) {
    if (mouseX >= 130 && mouseX <= 360 && mouseY >= 270 && mouseY <= 385) {
        selectedLevel = NOVICE;
        enemySpeed = SPEED / 2;
        enemyShootInterval = 0;
        inLevelSelect = false;
        gameRunning = true;
        enemies.clear();
        planets.clear();
        healthItems.clear();
        coins.clear();
        spreadShots.clear();
        enemiesSpawned = 0;
        enemyCount = DEFAULT_ENEMY_COUNT;
        for (int i = 0; i < enemyCount; ++i) {
            enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                               (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, false});
            enemiesSpawned++;
        }
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 385 && mouseX <= 615 && mouseY >= 270 && mouseY <= 385) {
        selectedLevel = WARRIOR;
        enemySpeed = SPEED;
        enemyShootInterval = DEFAULT_ENEMY_SHOOT_INTERVAL;
        inLevelSelect = false;
        gameRunning = true;
        enemies.clear();
        planets.clear();
        healthItems.clear();
        coins.clear();
        spreadShots.clear();
        enemyCount = 0;
        planetWave = 1;
        spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 640 && mouseX <= 870 && mouseY >= 270 && mouseY <= 385) {
        selectedLevel = LEGEND;
        enemySpeed = SPEED / 2;
        enemyShootInterval = 3000;
        inLevelSelect = false;
        gameRunning = true;
        enemies.clear();
        planets.clear();
        healthItems.clear();
        coins.clear();
        spreadShots.clear();
        enemiesSpawned = 0;
        enemyCount = DEFAULT_ENEMY_COUNT;
        for (int i = 0; i < enemyCount; ++i) {
            enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                               (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, true});
            enemiesSpawned++;
        }
        Mix_PlayChannel(-1, mouse_click, 0);
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        inLevelSelect = false;
        inMenu = true;
    }
}

void handleSettings(bool& inMenu, bool& inLevelSelect, bool& gameRunning, bool& helpScreen, bool& gameOver, bool& isPaused,
                    bool& inSettings, bool& inStore, bool& inExitOption, bool& loseSoundPlayed, bool& invincible,
                    GameLevel& selectedLevel, SmallShip& ship, vector<Egg>& eggs, vector<Bullet>& bullets, vector<Planet>& planets,
                    vector<SmallEnemy>& enemies, vector<Health>& healthItems, vector<Coin>& coins, vector<SpreadShot>& spreadShots,
                    int& enemyCount, int& enemiesSpawned, int& score, int& lastPlanetScore, int& enemySpeed, Uint32& enemyShootInterval,
                    int& lives, Uint32& frameTime, int& currentFrame, int& planetWave, int& enemyKillCount,
                    SDL_Texture* smallShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click) {
    if (mouseX >= 390 && mouseX <= 600 && mouseY >= 290 && mouseY <= 340) {
        inMenu = true;
        inLevelSelect = gameRunning = helpScreen = gameOver = isPaused = inSettings = inStore = false;
        loseSoundPlayed = invincible = false;
        selectedLevel = NONE;
        ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
        eggs.clear();
        bullets.clear();
        planets.clear();
        enemies.clear();
        healthItems.clear();
        coins.clear();
        spreadShots.clear();
        enemyCount = DEFAULT_ENEMY_COUNT;
        enemiesSpawned = score = lastPlanetScore = 0;
        enemySpeed = SPEED;
        enemyShootInterval = DEFAULT_ENEMY_SHOOT_INTERVAL;
        lives = MAX_LIVES;
        frameTime = currentFrame = planetWave = enemyKillCount = 0;
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 390 && mouseX <= 600 && mouseY >= 356 && mouseY <= 402) {
        inSettings = false;
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 390 && mouseX <= 600 && mouseY >= 420 && mouseY <= 470) {
        inExitOption = true;
        inMenu = inLevelSelect = gameRunning = helpScreen = gameOver = isPaused = inSettings = inStore = false;
        Mix_PlayChannel(-1, mouse_click, 0);
    }
}

void handleStore(int& coinCount, int& selectedShip, bool shipsPurchased[5], int shipPrices[5], int shipEggCounts[5],
                 SmallShip& ship, SDL_Texture* smallShipTexture, SDL_Texture* commonShipTexture, SDL_Texture* masterShipTexture,
                 SDL_Texture* bigShipTexture, SDL_Texture* manaShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click) {
    int shipX = 360, shipYStart = 160, rowHeight = 80;
    for (int i = 0; i < 5; i++) {
        if (mouseX >= shipX && mouseX <= shipX + SHIP_WIDTH &&
            mouseY >= shipYStart + i * rowHeight && mouseY <= shipYStart + i * rowHeight + SHIP_HEIGHT) {
            if (shipsPurchased[i] || coinCount >= shipPrices[i]) {
                if (!shipsPurchased[i]) {
                    coinCount -= shipPrices[i];
                    shipsPurchased[i] = true;
                }
                selectedShip = i;
                SDL_Texture* ships[] = {smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture, manaShipTexture};
                ship.texture = ships[selectedShip];
                ship.eggCount = shipEggCounts[selectedShip];
                Mix_PlayChannel(-1, mouse_click, 0);
            }
            break;
        }
    }
}

void handleExitOption(bool& running, bool& inMenu, bool& inLevelSelect, bool& gameRunning, bool& helpScreen, bool& gameOver,
                      bool& isPaused, bool& inSettings, bool& inStore, bool& inExitOption, int& coinCount, GameLevel& selectedLevel,
                      int& lives, int& selectedShip, int& spreadShotCount, int& score, int& enemyKillCount, bool shipsPurchased[5],
                      SmallShip& ship, vector<Egg>& eggs, vector<Bullet>& bullets, vector<Planet>& planets, vector<SmallEnemy>& enemies,
                      vector<Health>& healthItems, vector<Coin>& coins, vector<SpreadShot>& spreadShots, int& enemyCount,
                      int& enemiesSpawned, int& lastPlanetScore, int& enemySpeed, Uint32& enemyShootInterval, Uint32& frameTime,
                      int& currentFrame, int& planetWave, SDL_Texture* smallShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click) {
    if (mouseX >= 165 && mouseX <= 400 && mouseY >= 280 && mouseY <= 350) {
        if (gameOver) {
            lives = MAX_LIVES;
            gameOver = false;
            gameRunning = true;
        }
        saveGame(coinCount, selectedLevel, gameRunning, lives, selectedShip, spreadShotCount, shipsPurchased);
        running = false;
        Mix_PlayChannel(-1, mouse_click, 0);
    } else if (mouseX >= 610 && mouseX <= 840 && mouseY >= 385 && mouseY <= 460) {
        coinCount = score = spreadShotCount = enemyKillCount = 0;
        selectedLevel = NONE;
        gameRunning = false;
        lives = MAX_LIVES;
        selectedShip = 0;
        for (int i = 0; i < 5; i++) shipsPurchased[i] = (i == 0);
        ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
        eggs.clear();
        bullets.clear();
        planets.clear();
        enemies.clear();
        healthItems.clear();
        coins.clear();
        spreadShots.clear();
        enemyCount = DEFAULT_ENEMY_COUNT;
        enemiesSpawned = lastPlanetScore = 0;
        enemySpeed = SPEED;
        enemyShootInterval = DEFAULT_ENEMY_SHOOT_INTERVAL;
        frameTime = currentFrame = planetWave = 0;
        remove("savegame.txt");
        running = false;
        Mix_PlayChannel(-1, mouse_click, 0);
    }
}

void handleHelp(bool& inMenu, bool& helpScreen, SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
        helpScreen = false;
        inMenu = true;
    }
}

