#include "include/Constants.h"
#include "include/Structs.h"
#include "include/GameUtils.h"
#include "include/SDLManager.h"
#include "include/TextureManager.h"
#include "include/SoundManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
using namespace std;

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize SDL managers
    SDLManager& sdlManager = SDLManager::getInstance();
    TextureManager& textureManager = TextureManager::getInstance();
    SoundManager& soundManager = SoundManager::getInstance();

    if (!sdlManager.init("Outer Space", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }

    textureManager.init(sdlManager.getRenderer());
    soundManager.init();

    // Load game assets
    if (!textureManager.loadGameTextures()) {
        cout << "Failed to load game textures!" << endl;
        return 1;
    }

    if (!soundManager.loadGameSounds()) {
        cout << "Failed to load game sounds!" << endl;
        return 1;
    }

    // Start playing background music in infinite loop (-1)
    soundManager.playSound(soundManager.getBgAudio(), -1, -1);

    bool running = true;
    bool inMenu = true;
    bool inLevelSelect = false;
    bool gameRunning = false;
    bool helpScreen = false;
    bool gameOver = false;
    bool isPaused = false;
    bool inSettings = false;
    bool inStore = false;
    bool isDraggingStar = false;
    bool loseSoundPlayed = false;
    bool invincible = false;
    Uint32 invincibleTime = 0;
    const Uint32 INVINCIBLE_DURATION = 1000;
    GameLevel selectedLevel = NONE;
    SDL_Event event;
    SmallShip ship(WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0,
                   textureManager.getSmallShipTexture(), true, 1);
    vector<Egg> eggs;
    vector<SmallEnemy> enemies;
    vector<Bullet> bullets;
    vector<Planet> planets;
    vector<Health> healthItems;
    vector<Coin> coins;
    int enemyCount = 3;
    int enemiesSpawned = 0;
    int score = 0;
    int coinCount = 0;
    int highScore = loadHighScore();
    int lastPlanetScore = 0;
    int enemySpeed = SPEED;
    Uint32 enemyShootInterval = 2000;
    int lives = 3;
    Uint32 frameTime = 0;
    int currentFrame = 0;
    const Uint32 FRAME_DELAY = 100;
    int planetWave = 1;

    int starX = VOLUME_BAR_X + VOLUME_BAR_WIDTH / 2 - STAR_SIZE / 2;
    int starY = VOLUME_BAR_Y - STAR_SIZE / 2;

    TTF_Font* font = TTF_OpenFont("infra/font/pixel_font.ttf", 24);
    TTF_Font* smallFont = TTF_OpenFont("infra/font/pixel_font.ttf", 18);

    int selectedShip = 0;
    bool shipsPurchased[5] = {true, false, false, false, false};
    int shipPrices[5] = {0, 10, 20, 30, 40};
    int shipEggCounts[5] = {1, 2, 3, 4, 5};

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                int settingButtonX = WINDOW_WIDTH - 50;
                int settingButtonY = WINDOW_HEIGHT - 50;
                int settingButtonWidth = 50;
                int settingButtonHeight = 50;
                if (mouseX >= settingButtonX && mouseX <= settingButtonX + settingButtonWidth &&
                    mouseY >= settingButtonY && mouseY <= settingButtonY + settingButtonHeight) {
                    inSettings = !inSettings;
                    soundManager.playSound(soundManager.getMouseClick());
                }

                int storeButtonX = WINDOW_WIDTH - 100;
                int storeButtonY = WINDOW_HEIGHT - 50;
                int storeButtonWidth = 50;
                int storeButtonHeight = 50;
                if (mouseX >= storeButtonX && mouseX <= storeButtonX + storeButtonWidth &&
                    mouseY >= storeButtonY && mouseY <= storeButtonY + storeButtonHeight) {
                    inStore = !inStore;
                    soundManager.playSound(soundManager.getMouseClick());
                }

                if (inSettings) {
                    if (mouseX >= starX && mouseX <= starX + STAR_SIZE &&
                        mouseY >= starY && mouseY <= starY + STAR_SIZE) {
                        isDraggingStar = true;
                    }
                }

                if (inSettings && !isDraggingStar) {
                    if (mouseX >= 388 && mouseX <= 607 && mouseY >= 290 && mouseY <= 333) {
                        inMenu = true;
                        inLevelSelect = false;
                        gameRunning = false;
                        helpScreen = false;
                        gameOver = false;
                        isPaused = false;
                        inSettings = false;
                        inStore = false;
                        loseSoundPlayed = false;
                        invincible = false;
                        selectedLevel = NONE;
                        ship = SmallShip(WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0,
                                         textureManager.getSmallShipTexture(), true, 1);
                        eggs.clear();
                        bullets.clear();
                        planets.clear();
                        enemies.clear();
                        healthItems.clear();
                        coins.clear();
                        enemyCount = 3;
                        enemiesSpawned = 0;
                        score = 0;
                        lastPlanetScore = 0;
                        enemySpeed = SPEED;
                        enemyShootInterval = 2000;
                        lives = 3;
                        frameTime = 0;
                        currentFrame = 0;
                        planetWave = 1;
                        soundManager.playSound(soundManager.getMouseClick());
                    }
                    else if (mouseX >= 388 && mouseX <= 607 && mouseY >= 350 && mouseY <= 400) {
                        inSettings = false;
                        soundManager.playSound(soundManager.getMouseClick());
                    }
                    else if (mouseX >= 388 && mouseX <= 607 && mouseY >= 416 && mouseY <= 460) {
                        running = false;
                        soundManager.playSound(soundManager.getMouseClick());
                    }
                }

                if (inStore) {
                    int shipX = 360;
                    int shipYStart = 160;
                    int rowHeight = 80;
                    for (int i = 0; i < 5; i++) {
                        if (mouseX >= shipX && mouseX <= shipX + SHIP_WIDTH &&
                            mouseY >= shipYStart + i * rowHeight && mouseY <= shipYStart + i * rowHeight + SHIP_HEIGHT) {
                            if (shipsPurchased[i]) {
                                selectedShip = i;
                                SDL_Texture* ships[] = {textureManager.getSmallShipTexture(), textureManager.getCommonShipTexture(), textureManager.getMasterShipTexture(), textureManager.getBigShipTexture(), textureManager.getManaShipTexture()};
                                ship = SmallShip(ship.x, ship.y, ship.dx, ship.dy, ships[selectedShip], true, shipEggCounts[selectedShip]);
                                soundManager.playSound(soundManager.getMouseClick());
                            } else if (coinCount >= shipPrices[i]) {
                                coinCount -= shipPrices[i];
                                shipsPurchased[i] = true;
                                selectedShip = i;
                                SDL_Texture* ships[] = {textureManager.getSmallShipTexture(), textureManager.getCommonShipTexture(), textureManager.getMasterShipTexture(), textureManager.getBigShipTexture(), textureManager.getManaShipTexture()};
                                ship = SmallShip(ship.x, ship.y, ship.dx, ship.dy, ships[selectedShip], true, shipEggCounts[selectedShip]);
                                soundManager.playSound(soundManager.getMouseClick());
                            }
                            break;
                        }
                    }
                }

                if (!inSettings && !inStore) {
                    if (inMenu) {
                        if (mouseX >= 410 && mouseX <= 575 && mouseY >= 345 && mouseY <= 385) {
                            inMenu = false;
                            inLevelSelect = true;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 405 && mouseY <= 445) {
                            inMenu = false;
                            helpScreen = true;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 465 && mouseY <= 505) {
                            running = false;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                    }
                    else if (inLevelSelect) {
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
                            enemiesSpawned = 0;
                            for (int i = 0; i < enemyCount; ++i) {
                                bool canShoot = false;
                                enemies.push_back(SmallEnemy(rand() % (WINDOW_WIDTH - ENEMY_WIDTH),
                                                            rand() % (WINDOW_HEIGHT / 2),
                                                            (rand() % 3 - 1) * enemySpeed,
                                                            (rand() % 3 - 1) * enemySpeed,
                                                            textureManager.getEnemyTexture(), true, 0, canShoot));
                                enemiesSpawned++;
                            }
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 385 && mouseX <= 615 && mouseY >= 270 && mouseY <= 385) {
                            selectedLevel = WARRIOR;
                            enemySpeed = SPEED;
                            enemyShootInterval = 2000;
                            inLevelSelect = false;
                            gameRunning = true;
                            enemies.clear();
                            planets.clear();
                            healthItems.clear();
                            coins.clear();
                            enemiesSpawned = 0;
                            planetWave = 1;
                            spawnPlanets(planets, planetWave, textureManager.getPlanet1SpriteSheetTexture(), textureManager.getPlanet2SpriteSheetTexture(), textureManager.getPlanet3SpriteSheetTexture());
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 640 && mouseX <= 870 && mouseY >= 270 && mouseY <= 385) {
                            selectedLevel = LEGEND;
                            enemySpeed = SPEED / 2;
                            enemyShootInterval = 3000;
                            inLevelSelect = false;
                            gameRunning = true;
                            enemies.clear();
                            planets.clear();
                            healthItems.clear();
                            coins.clear();
                            enemiesSpawned = 0;
                            enemyCount = 3;
                            for (int i = 0; i < enemyCount; ++i) {
                                bool canShoot = true;
                                enemies.push_back(SmallEnemy(rand() % (WINDOW_WIDTH - ENEMY_WIDTH),
                                                            rand() % (WINDOW_HEIGHT / 2),
                                                            (rand() % 3 - 1) * enemySpeed,
                                                            (rand() % 3 - 1) * enemySpeed,
                                                            textureManager.getEnemyTexture(), true, 0, canShoot));
                                enemiesSpawned++;
                            }
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                    }
                    else if (gameRunning && !gameOver) {
                        int buttonX = 10;
                        int buttonY = 520;
                        int buttonWidth = 40;
                        int buttonHeight = 40;
                        if (mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
                            mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
                            isPaused = !isPaused;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                    }
                    else if (gameOver) {
                        if (mouseX >= 340 && mouseX <= 625 && mouseY >= 260 && mouseY <= 300) {
                            gameOver = false;
                            lives = 3;
                            invincible = false;
                            ship = SmallShip(WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0,
                                             textureManager.getSmallShipTexture(), true, 1);
                            eggs.clear();
                            enemies.clear();
                            bullets.clear();
                            planets.clear();
                            healthItems.clear();
                            coins.clear();
                            enemyCount = 3;
                            enemiesSpawned = 0;
                            score = 0;
                            lastPlanetScore = 0;
                            loseSoundPlayed = false;
                            planetWave = 1;
                            if (selectedLevel == NOVICE) {
                                for (int i = 0; i < enemyCount; ++i) {
                                    bool canShoot = false;
                                    enemies.push_back(SmallEnemy(rand() % (WINDOW_WIDTH - ENEMY_WIDTH),
                                                                rand() % (WINDOW_HEIGHT / 2),
                                                                (rand() % 3 - 1) * enemySpeed,
                                                                (rand() % 3 - 1) * enemySpeed,
                                                                textureManager.getEnemyTexture(), true, 0, canShoot));
                                    enemiesSpawned++;
                                }
                            }
                            else if (selectedLevel == WARRIOR) {
                                spawnPlanets(planets, planetWave, textureManager.getPlanet1SpriteSheetTexture(), textureManager.getPlanet2SpriteSheetTexture(), textureManager.getPlanet3SpriteSheetTexture());
                            }
                            else if (selectedLevel == LEGEND) {
                                enemyCount = 3;
                                for (int i = 0; i < enemyCount; ++i) {
                                    bool canShoot = true;
                                    enemies.push_back(SmallEnemy(rand() % (WINDOW_WIDTH - ENEMY_WIDTH),
                                                                rand() % (WINDOW_HEIGHT / 2),
                                                                (rand() % 3 - 1) * enemySpeed,
                                                                (rand() % 3 - 1) * enemySpeed,
                                                                textureManager.getEnemyTexture(), true, 0, canShoot));
                                    enemiesSpawned++;
                                }
                            }
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 330 && mouseY <= 370) {
                            inMenu = false;
                            inLevelSelect = true;
                            gameRunning = false;
                            gameOver = false;
                            lives = 3;
                            invincible = false;
                            ship = SmallShip(WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0,
                                             textureManager.getSmallShipTexture(), true, 1);
                            eggs.clear();
                            bullets.clear();
                            planets.clear();
                            enemies.clear();
                            healthItems.clear();
                            coins.clear();
                            enemyCount = 3;
                            enemiesSpawned = 0;
                            score = 0;
                            lastPlanetScore = 0;
                            loseSoundPlayed = false;
                            planetWave = 1;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                        else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 400 && mouseY <= 440) {
                            running = false;
                            soundManager.playSound(soundManager.getMouseClick());
                        }
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                isDraggingStar = false;
            }
            else if (event.type == SDL_MOUSEMOTION && isDraggingStar) {
                int mouseX = event.motion.x;
                starX = mouseX - STAR_SIZE / 2;
                if (starX < VOLUME_BAR_X) starX = VOLUME_BAR_X;
                if (starX > VOLUME_BAR_X + VOLUME_BAR_WIDTH - STAR_SIZE) starX = VOLUME_BAR_X + VOLUME_BAR_WIDTH - STAR_SIZE;

                float volumeRatio = static_cast<float>(starX - VOLUME_BAR_X) / (VOLUME_BAR_WIDTH - STAR_SIZE);
                int volume = static_cast<int>(volumeRatio * 128);
                soundManager.setVolume(volume);
            }
            else if (event.type == SDL_KEYDOWN && gameRunning && !gameOver && !isPaused && !inSettings && !inStore) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: ship.dx = -SPEED; break;
                    case SDLK_RIGHT: ship.dx = SPEED; break;
                    case SDLK_UP: ship.dy = -SPEED; break;
                    case SDLK_DOWN: ship.dy = SPEED; break;
                    case SDLK_SPACE:
                        switch (ship.eggCount) {
                            case 1:
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                break;
                            case 2:
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 10, ship.y, 0, -5, true));
                                break;
                            case 3:
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 20, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 20, ship.y, 1, -5, true));
                                break;
                            case 4:
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 30, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 30, ship.y, 1, -5, true));
                                break;
                            case 5:
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 40, ship.y, -2, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 20, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 20, ship.y, 1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 40, ship.y, 2, -5, true));
                                break;
                        }
                        soundManager.playSound(soundManager.getShootSound());
                        break;
                }
            }
            else if (event.type == SDL_KEYUP && gameRunning && !gameOver && !isPaused && !inSettings && !inStore) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT: ship.dx = 0; break;
                    case SDLK_UP:
                    case SDLK_DOWN: ship.dy = 0; break;
                }
            }
            else if (event.type == SDL_KEYDOWN && helpScreen) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    helpScreen = false;
                    inMenu = true;
                }
            }
            else if (event.type == SDL_KEYDOWN && inLevelSelect) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    inLevelSelect = false;
                    inMenu = true;
                }
            }
        }

        if (inMenu) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getMenuBgTexture(), NULL, NULL);
        }

        if (inLevelSelect) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getLevelBgTexture(), NULL, NULL);
        }

        if (gameRunning) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getGameBgTexture(), NULL, NULL);

            if (!isPaused && !inSettings && !inStore && !gameOver) {
                int newX = ship.x + ship.dx;
                int newY = ship.y + ship.dy;
                if (newX >= 0 && newX <= WINDOW_WIDTH - SHIP_WIDTH) ship.x = newX;
                if (newY >= 0 && newY <= WINDOW_HEIGHT - SHIP_HEIGHT) ship.y = newY;

                for (auto& egg : eggs) {
                    if (egg.active) {
                        egg.x += egg.dx;
                        egg.y += egg.dy;
                        if (egg.y < 0 || egg.x < 0 || egg.x > WINDOW_WIDTH - EGG_SIZE) egg.active = false;
                    }
                }
                eggs.erase(remove_if(eggs.begin(), eggs.end(), [](const Egg& e) { return !e.active; }), eggs.end());

                if (selectedLevel == NOVICE || selectedLevel == LEGEND) {
                    for (auto& enemy : enemies) {
                        if (enemy.active) {
                            enemy.x += enemy.dx;
                            enemy.y += enemy.dy;
                            if (enemy.x < 0 || enemy.x > WINDOW_WIDTH - ENEMY_WIDTH) enemy.dx = -enemy.dx;
                            if (enemy.y < 0 || enemy.y > WINDOW_HEIGHT / 2) enemy.dy = -enemy.dy;

                            if (enemy.canShoot && enemyShootInterval > 0) {
                                Uint32 currentTime = SDL_GetTicks();
                                if (currentTime - enemy.lastShotTime >= enemyShootInterval) {
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, textureManager.getBulletTexture(), true));
                                    enemy.lastShotTime = currentTime;
                                }
                            }
                        }
                    }
                    enemies.erase(remove_if(enemies.begin(), enemies.end(), [](const SmallEnemy& e) { return !e.active; }), enemies.end());

                    if (enemies.empty() && enemiesSpawned > 0) {
                        enemyCount += 2;
                        for (int i = 0; i < enemyCount; ++i) {
                            bool canShoot = (selectedLevel == LEGEND);
                            enemies.push_back(SmallEnemy(rand() % (WINDOW_WIDTH - ENEMY_WIDTH),
                                                        rand() % (WINDOW_HEIGHT / 2),
                                                        (rand() % 3 - 1) * enemySpeed,
                                                        (rand() % 3 - 1) * enemySpeed,
                                                        textureManager.getEnemyTexture(), true, 0, canShoot));
                        }
                        enemiesSpawned += enemyCount;
                    }
                }

                for (auto& bullet : bullets) {
                    if (bullet.active) {
                        bullet.x += bullet.dx;
                        bullet.y += bullet.dy;
                        if (bullet.y > WINDOW_HEIGHT || bullet.x < 0 || bullet.x > WINDOW_WIDTH - BULLET_SIZE) bullet.active = false;
                    }
                }
                bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());

                if (selectedLevel != NOVICE) {
                    if (selectedLevel == LEGEND && score >= 20 && (score - lastPlanetScore) >= 20) {
                        spawnPlanets(planets, 1, textureManager.getPlanet1SpriteSheetTexture(), textureManager.getPlanet2SpriteSheetTexture(), textureManager.getPlanet3SpriteSheetTexture());
                        lastPlanetScore = score;
                    }

                    if (selectedLevel == WARRIOR && planets.empty()) {
                        spawnPlanets(planets, planetWave, textureManager.getPlanet1SpriteSheetTexture(), textureManager.getPlanet2SpriteSheetTexture(), textureManager.getPlanet3SpriteSheetTexture());
                        planetWave++;
                    }

                    for (auto& planet : planets) {
                        if (planet.active) {
                            if (selectedLevel == WARRIOR) {
                                planet.speed = min(planet.speed + 0.001f, 3.0f);
                                planet.x += static_cast<int>(planet.dx * planet.speed);
                                planet.y += static_cast<int>(planet.dy * planet.speed);
                                if (planet.x < 0 || planet.x > WINDOW_WIDTH - PLANET_WIDTH) planet.dx = -planet.dx;
                                if (planet.y < 0 || planet.y > WINDOW_HEIGHT / 2) planet.dy = -planet.dy;
                            }

                            Uint32 currentTime = SDL_GetTicks();
                            if (currentTime - planet.lastShotTime >= planet.shootInterval) {
                                int dx = ship.x + SHIP_WIDTH / 2 - (planet.x + PLANET_WIDTH / 2);
                                int dy = ship.y + SHIP_HEIGHT / 2 - (planet.y + PLANET_HEIGHT / 2);
                                float length = sqrt(dx * dx + dy * dy);
                                if (length > 0) {
                                    dx = static_cast<int>(dx * 5 / length);
                                    dy = static_cast<int>(dy * 5 / length);
                                    bullets.push_back(Bullet(planet.x + PLANET_WIDTH / 2 - BULLET_SIZE / 2, planet.y + PLANET_HEIGHT / 2, dx, dy, textureManager.getBulletTexture(), true));
                                }
                                planet.lastShotTime = currentTime;
                            }
                        }
                    }

                    vector<pair<int, int>> destroyedPlanetPositions;
                    for (auto& egg : eggs) {
                        for (auto& planet : planets) {
                            if (egg.active && planet.active) {
                                int eggLeft = egg.x;
                                int eggRight = egg.x + EGG_SIZE;
                                int eggTop = egg.y;
                                int eggBottom = egg.y + EGG_SIZE;
                                int planetLeft = planet.x;
                                int planetRight = planet.x + PLANET_WIDTH;
                                int planetTop = planet.y;
                                int planetBottom = planet.y + PLANET_HEIGHT;

                                if (eggRight > planetLeft && eggLeft < planetRight && eggBottom > planetTop && eggTop < planetBottom) {
                                    egg.active = false;
                                    planet.health--;
                                    if (planet.health <= 0) {
                                        planet.active = false;
                                        score += 5;
                                        destroyedPlanetPositions.push_back({planet.x, planet.y});
                                        for (int i = 0; i < 5; ++i) {
                                            int offsetX = (rand() % 41) - 20;
                                            int offsetY = (rand() % 41) - 20;
                                            coins.push_back(Coin(planet.x + PLANET_WIDTH / 2 - COIN_SIZE / 2 + offsetX,
                                                                planet.y + PLANET_HEIGHT / 2 - COIN_SIZE / 2 + offsetY));
                                        }
                                    }
                                }
                            }
                        }
                    }

                    planets.erase(remove_if(planets.begin(), planets.end(),
                        [](const Planet& p) { return !p.active; }), planets.end());

                    for (const auto& pos : destroyedPlanetPositions) {
                        healthItems.push_back(Health(pos.first + PLANET_WIDTH / 2 - HEALTH_SIZE / 2,
                                                    pos.second + PLANET_HEIGHT / 2 - HEALTH_SIZE / 2, true));
                    }
                }

                for (auto& egg : eggs) {
                    for (auto& enemy : enemies) {
                        if (egg.active && enemy.active) {
                            int eggLeft = egg.x;
                            int eggRight = egg.x + EGG_SIZE;
                            int eggTop = egg.y;
                            int eggBottom = egg.y + EGG_SIZE;
                            int enemyLeft = enemy.x;
                            int enemyRight = enemy.x + ENEMY_WIDTH;
                            int enemyTop = enemy.y;
                            int enemyBottom = enemy.y + ENEMY_HEIGHT;

                            if (eggRight > enemyLeft && eggLeft < enemyRight && eggBottom > enemyTop && eggTop < enemyBottom) {
                                egg.active = false;
                                enemy.active = false;
                                score++;
                                coins.push_back(Coin(enemy.x + ENEMY_WIDTH / 2 - COIN_SIZE / 2, enemy.y + ENEMY_HEIGHT / 2 - COIN_SIZE / 2));
                                if (selectedLevel == NOVICE) {
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, textureManager.getBulletTexture(), true));
                                }
                            }
                        }
                    }
                }

                for (const auto& enemy : enemies) {
                    if (enemy.active && !invincible) {
                        int shipLeft = ship.x;
                        int shipRight = ship.x + SHIP_WIDTH;
                        int shipTop = ship.y;
                        int shipBottom = ship.y + SHIP_HEIGHT;
                        int enemyLeft = enemy.x;
                        int enemyRight = enemy.x + ENEMY_WIDTH;
                        int enemyTop = enemy.y;
                        int enemyBottom = enemy.y + ENEMY_HEIGHT;

                        if (shipRight > enemyLeft && shipLeft < enemyRight && shipBottom > enemyTop && shipTop < enemyBottom) {
                            lives--;
                            soundManager.playSound(soundManager.getLoseHeartSound());
                            invincible = true;
                            invincibleTime = SDL_GetTicks();
                            if (lives <= 0) {
                                gameOver = true;
                            } else {
                                ship.x = WINDOW_WIDTH / 2;
                                ship.y = WINDOW_HEIGHT - SHIP_HEIGHT - 10;
                            }
                            break;
                        }
                    }
                }

                for (const auto& bullet : bullets) {
                    if (bullet.active && !invincible) {
                        int shipLeft = ship.x;
                        int shipRight = ship.x + SHIP_WIDTH;
                        int shipTop = ship.y;
                        int shipBottom = ship.y + SHIP_HEIGHT;
                        int bulletLeft = bullet.x;
                        int bulletRight = bullet.x + BULLET_SIZE;
                        int bulletTop = bullet.y;
                        int bulletBottom = bullet.y + BULLET_SIZE;

                        if (shipRight > bulletLeft && shipLeft < bulletRight && shipBottom > bulletTop && shipTop < bulletBottom) {
                            lives--;
                            soundManager.playSound(soundManager.getLoseHeartSound());
                            invincible = true;
                            invincibleTime = SDL_GetTicks();
                            if (lives <= 0) {
                                gameOver = true;
                            } else {
                                ship.x = WINDOW_WIDTH / 2;
                                ship.y = WINDOW_HEIGHT - SHIP_HEIGHT - 10;
                            }
                            break;
                        }
                    }
                }

                if (invincible) {
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - invincibleTime >= INVINCIBLE_DURATION) {
                        invincible = false;
                    }
                }

                for (auto& health : healthItems) {
                    if (health.active) {
                        int shipLeft = ship.x;
                        int shipRight = ship.x + SHIP_WIDTH;
                        int shipTop = ship.y;
                        int shipBottom = ship.y + SHIP_HEIGHT;
                        int healthLeft = health.x;
                        int healthRight = health.x + HEALTH_SIZE;
                        int healthTop = health.y;
                        int healthBottom = health.y + HEALTH_SIZE;

                        if (shipRight > healthLeft && shipLeft < healthRight && shipBottom > healthTop && shipTop < healthBottom) {
                            health.active = false;
                            if (lives < MAX_LIVES) {
                                lives++;
                                soundManager.playSound(soundManager.getHealthSound());
                            }
                        }
                    }
                }
                healthItems.erase(remove_if(healthItems.begin(), healthItems.end(),
                    [](const Health& h) { return !h.active; }), healthItems.end());

                for (auto& coin : coins) {
                    if (coin.active) {
                        int shipLeft = ship.x;
                        int shipRight = ship.x + SHIP_WIDTH;
                        int shipTop = ship.y;
                        int shipBottom = ship.y + SHIP_HEIGHT;
                        int coinLeft = coin.x;
                        int coinRight = coin.x + COIN_SIZE;
                        int coinTop = coin.y;
                        int coinBottom = coin.y + COIN_SIZE;

                        if (shipRight > coinLeft && shipLeft < coinRight && shipBottom > coinTop && shipTop < coinBottom) {
                            coin.active = false;
                            coinCount++;
                            soundManager.playSound(soundManager.getCoinSound());
                        }
                    }
                }
                coins.erase(remove_if(coins.begin(), coins.end(),
                    [](const Coin& c) { return !c.active; }), coins.end());

                if (gameOver && score > highScore) {
                    highScore = score;
                    saveHighScore(highScore);
                }
            }

            if (ship.active) {
                SDL_Rect shipRect = {ship.x, ship.y, SHIP_WIDTH, SHIP_HEIGHT};
                SDL_RenderCopy(sdlManager.getRenderer(), ship.texture, NULL, &shipRect);
            }

            SDL_Rect eggRect = {0, 0, EGG_SIZE, EGG_SIZE};
            for (const auto& egg : eggs) {
                if (egg.active) {
                    eggRect.x = egg.x;
                    eggRect.y = egg.y;
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getEggTexture(), NULL, &eggRect);
                }
            }

            SDL_Rect enemyRect = {0, 0, ENEMY_WIDTH, ENEMY_HEIGHT};
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    enemyRect.x = enemy.x;
                    enemyRect.y = enemy.y;
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getEnemyTexture(), NULL, &enemyRect);
                }
            }

            SDL_Rect planetSrcRect = {0, 0, 0, PLANET_HEIGHT};
            SDL_Rect planetDestRect = {0, 0, PLANET_WIDTH, PLANET_HEIGHT};
            for (auto& planet : planets) {
                if (planet.active) {
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - planet.lastFrameTime >= planet.frameDelay) {
                        planet.currentFrame = (planet.currentFrame + 1) % planet.totalFrames;
                        planet.lastFrameTime = currentTime;
                    }

                    planetSrcRect.x = planet.currentFrame * planet.frameWidth;
                    planetSrcRect.y = 0;
                    planetSrcRect.w = planet.frameWidth;
                    planetSrcRect.h = PLANET_HEIGHT;

                    planetDestRect.x = planet.x;
                    planetDestRect.y = planet.y;
                    planetDestRect.w = PLANET_WIDTH;
                    planetDestRect.h = PLANET_HEIGHT;

                    SDL_RenderCopy(sdlManager.getRenderer(), planet.texture, &planetSrcRect, &planetDestRect);

                    SDL_Color white = {255, 255, 255, 255};
                    string healthText = to_string(planet.health);
                    SDL_Surface* healthSurface = TTF_RenderText_Solid(font, healthText.c_str(), white);
                    if (healthSurface) {
                        SDL_Texture* healthTexture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), healthSurface);
                        if (healthTexture) {
                            SDL_Rect healthRect = {planet.x + PLANET_WIDTH / 2 - healthSurface->w / 2, planet.y - 20, healthSurface->w, healthSurface->h};
                            SDL_RenderCopy(sdlManager.getRenderer(), healthTexture, NULL, &healthRect);
                            SDL_DestroyTexture(healthTexture);
                        }
                        SDL_FreeSurface(healthSurface);
                    }
                }
            }

            SDL_Rect bulletRect = {0, 0, BULLET_SIZE, BULLET_SIZE};
            for (const auto& bullet : bullets) {
                if (bullet.active) {
                    bulletRect.x = bullet.x;
                    bulletRect.y = bullet.y;
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getBulletTexture(), NULL, &bulletRect);
                }
            }

            SDL_Rect healthRect = {0, 0, HEALTH_SIZE, HEALTH_SIZE};
            for (const auto& health : healthItems) {
                if (health.active) {
                    healthRect.x = health.x;
                    healthRect.y = health.y;
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getHealthTexture(), NULL, &healthRect);
                }
            }

            SDL_Rect coinSrcRect = {0, 0, COIN_SIZE, COIN_SIZE};
            SDL_Rect coinDestRect = {0, 0, COIN_SIZE, COIN_SIZE};
            for (auto& coin : coins) {
                if (coin.active) {
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - coin.lastFrameTime >= 100) {
                        coin.currentFrame = (coin.currentFrame + 1) % COIN_FRAMES;
                        coin.lastFrameTime = currentTime;
                    }
                    coinSrcRect.x = coin.currentFrame * COIN_SIZE;
                    coinDestRect.x = coin.x;
                    coinDestRect.y = coin.y;
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getCoinSpriteSheetTexture(), &coinSrcRect, &coinDestRect);
                }
            }

            Uint32 currentTime = SDL_GetTicks();
            if (!isPaused && !inSettings && !inStore && !gameOver && currentTime - frameTime >= FRAME_DELAY) {
                currentFrame = (currentFrame + 1) % HEART_FRAMES;
                frameTime = currentTime;
            }

            SDL_Rect heartDisplayRect = {0, 0, HEART_SIZE, HEART_SIZE};
            for (int i = 0; i < lives; i++) {
                heartDisplayRect.x = WINDOW_WIDTH - (HEART_SIZE + 20) * (i + 1) - 10;
                heartDisplayRect.y = 10;
                SDL_Rect heartSrc = {currentFrame * HEART_SPRITE_SIZE, 0, HEART_SPRITE_SIZE, HEART_SPRITE_SIZE};
                SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getHeartTexture(), &heartSrc, &heartDisplayRect);
            }

            SDL_Color white = {255, 255, 255, 255};
            string scoreText = "Your score: " + to_string(score);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), scoreSurface);
                if (scoreTexture) {
                    int scoreX = 10;
                    int scoreY = 5;
                    SDL_Rect scoreRect = {scoreX, scoreY, scoreSurface->w, scoreSurface->h};
                    SDL_RenderCopy(sdlManager.getRenderer(), scoreTexture, NULL, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_FreeSurface(scoreSurface);
            }

            string highScoreText = "High score: " + to_string(highScore);
            SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), white);
            if (highScoreSurface) {
                SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), highScoreSurface);
                if (highScoreTexture) {
                    int highScoreX = 10;
                    int highScoreY = 30;
                    SDL_Rect highScoreRect = {highScoreX, highScoreY, highScoreSurface->w, highScoreSurface->h};
                    SDL_RenderCopy(sdlManager.getRenderer(), highScoreTexture, NULL, &highScoreRect);
                    SDL_DestroyTexture(highScoreTexture);
                }
                SDL_FreeSurface(highScoreSurface);
            }

            string coinText = "Coin: " + to_string(coinCount);
            SDL_Surface* coinSurface = TTF_RenderText_Solid(font, coinText.c_str(), white);
            if (coinSurface) {
                SDL_Texture* coinTexture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), coinSurface);
                if (coinTexture) {
                    int coinX = 10;
                    int coinY = 55;
                    SDL_Rect coinRect = {coinX, coinY, coinSurface->w, coinSurface->h};
                    SDL_RenderCopy(sdlManager.getRenderer(), coinTexture, NULL, &coinRect);
                    SDL_DestroyTexture(coinTexture);
                }
                SDL_FreeSurface(coinSurface);
            }

            SDL_Rect buttonRect = {10, 520, 40, 40};
            if (isPaused) {
                SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getContinueTexture(), NULL, &buttonRect);
            } else {
                SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getPauseTexture(), NULL, &buttonRect);
            }

            if (gameOver) {
                SDL_Rect loseRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getLoseTexture(), NULL, &loseRect);

                if (!loseSoundPlayed) {
                    soundManager.playSound(soundManager.getLoseSound());
                    loseSoundPlayed = true;
                }
            }
        }

        if (helpScreen) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getHelpBgTexture(), NULL, NULL);
        }

        if (inStore) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getStoreTexture(), NULL, NULL);

            SDL_Color white = {255, 255, 255, 255};
            string prices[] = {"Free", "10 coins", "20 coins", "30 coins", "40 coins"};
            int priceX = 105;
            int priceYStart = 175;
            int rowHeight = 80;

            for (int i = 0; i < 5; i++) {
                SDL_Surface* priceSurface = TTF_RenderText_Solid(smallFont, prices[i].c_str(), white);
                if (priceSurface) {
                    SDL_Texture* priceTexture = SDL_CreateTextureFromSurface(sdlManager.getRenderer(), priceSurface);
                    if (priceTexture) {
                        SDL_Rect priceRect = {priceX, priceYStart + i * rowHeight, priceSurface->w, priceSurface->h};
                        SDL_RenderCopy(sdlManager.getRenderer(), priceTexture, NULL, &priceRect);
                        SDL_DestroyTexture(priceTexture);
                    }
                    SDL_FreeSurface(priceSurface);
                }
            }

            SDL_Texture* ships[] = {textureManager.getSmallShipTexture(), textureManager.getCommonShipTexture(), textureManager.getMasterShipTexture(), textureManager.getBigShipTexture(), textureManager.getManaShipTexture()};
            int shipX = 360;
            int shipYStart = 160;

            for (int i = 0; i < 5; i++) {
                SDL_Rect shipRect = {shipX, shipYStart + i * rowHeight, SHIP_WIDTH, SHIP_HEIGHT};
                SDL_RenderCopy(sdlManager.getRenderer(), ships[i], NULL, &shipRect);

                if (i == selectedShip && shipsPurchased[i]) {
                    SDL_Rect chosenRect = {shipX + SHIP_WIDTH, shipYStart + i * rowHeight, 32, 32};
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getChosenTexture(), NULL, &chosenRect);
                }

                if (shipsPurchased[i]) {
                    SDL_Rect soldRect = {shipX, shipYStart + i * rowHeight, SHIP_WIDTH, SHIP_HEIGHT};
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getSoldTexture(), NULL, &soldRect);
                }
            }

            int eggX = 600;
            int eggYStart = 160;
            int eggCounts[] = {1, 2, 3, 4, 5};

            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < eggCounts[i]; j++) {
                    SDL_Rect eggRect = {eggX + j * (EGG_SIZE + 5), eggYStart + i * rowHeight, EGG_SIZE, EGG_SIZE};
                    SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getEggTexture(), NULL, &eggRect);
                }
            }
        }

        if (inSettings) {
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getSettingTexture(), NULL, NULL);
            SDL_Rect starRect = {starX, starY, STAR_SIZE, STAR_SIZE};
            SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getStarTexture(), NULL, &starRect);
        }

        SDL_Rect settingButtonRect = {WINDOW_WIDTH - 50, WINDOW_HEIGHT - 50, 50, 50};
        SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getSettingStarTexture(), NULL, &settingButtonRect);

        SDL_Rect storeButtonRect = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - 50, 50, 50};
        SDL_RenderCopy(sdlManager.getRenderer(), textureManager.getStoreIconTexture(), NULL, &storeButtonRect);

        SDL_RenderPresent(sdlManager.getRenderer());
        SDL_Delay(16);
    }

    if (score > highScore) {
        highScore = score;
        saveHighScore(highScore);
    }

    soundManager.cleanup();
    textureManager.cleanup();
    sdlManager.cleanup();
    TTF_CloseFont(font);
    TTF_CloseFont(smallFont);
    TTF_Quit();
    return 0;
}
