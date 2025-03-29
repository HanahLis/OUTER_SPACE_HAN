#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "../include/constants.h"
#include "../include/render.h"
#include "../include/gameutils.h"
#include "../include/fileutils.h"
#include "../include/menuutils.h"
#include "../include/structs.h"
using namespace std;

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!initializeSDL(window, renderer)) return 1;

    SDL_Texture* menuBgTexture = loadTexture(renderer, "infra/img/bg/menu.png");
    SDL_Texture* levelBgTexture = loadTexture(renderer, "infra/img/bg/level.png");
    SDL_Texture* gameBgTexture = loadTexture(renderer, "infra/img/bg/bg2.png");
    SDL_Texture* helpBgTexture = loadTexture(renderer, "infra/img/bg/help.png");
    SDL_Texture* loseTexture = loadTexture(renderer, "infra/img/bg/lose.png");
    SDL_Texture* exitOptionTexture = loadTexture(renderer, "infra/img/bg/exit_option.png");
    SDL_Texture* smallShipTexture = loadTexture(renderer, "infra/img/ship/small_ship.png");
    SDL_Texture* commonShipTexture = loadTexture(renderer, "infra/img/ship/common_ship.png");
    SDL_Texture* masterShipTexture = loadTexture(renderer, "infra/img/ship/super_ship.png");
    SDL_Texture* bigShipTexture = loadTexture(renderer, "infra/img/ship/big_ship.png");
    SDL_Texture* manaShipTexture = loadTexture(renderer, "infra/img/ship/mana_ship.png");
    SDL_Texture* enemyTexture = loadTexture(renderer, "infra/img/ship/small_enemy.png");
    SDL_Texture* bulletTexture = loadTexture(renderer, "infra/img/object/bullet.png");
    SDL_Texture* heartTexture = loadTexture(renderer, "infra/img/object/heart.png");
    SDL_Texture* eggTexture = loadTexture(renderer, "infra/img/object/egg.png");
    SDL_Texture* healthTexture = loadTexture(renderer, "infra/img/object/health.png");
    SDL_Texture* pauseTexture = loadTexture(renderer, "infra/img/object/pause.png");
    SDL_Texture* continueTexture = loadTexture(renderer, "infra/img/object/continue.png");
    SDL_Texture* starTexture = loadTexture(renderer, "infra/img/object/star.png");
    SDL_Texture* settingStarTexture = loadTexture(renderer, "infra/img/object/setting_star.png");
    SDL_Texture* settingTexture = loadTexture(renderer, "infra/img/bg/setting.png");
    SDL_Texture* storeIconTexture = loadTexture(renderer, "infra/img/object/store_icon.png");
    SDL_Texture* storeTexture = loadTexture(renderer, "infra/img/bg/store.png");
    SDL_Texture* planet1SpriteSheetTexture = loadTexture(renderer, "infra/img/object/planet1_spritesheet.png");
    SDL_Texture* planet2SpriteSheetTexture = loadTexture(renderer, "infra/img/object/planet2_spritesheet.png");
    SDL_Texture* planet3SpriteSheetTexture = loadTexture(renderer, "infra/img/object/planet3_spritesheet.png");
    SDL_Texture* coinSpriteSheetTexture = loadTexture(renderer, "infra/img/object/coin.png");
    SDL_Texture* chosenTexture = loadTexture(renderer, "infra/img/object/chosen.png");
    SDL_Texture* soldTexture = loadTexture(renderer, "infra/img/object/sold.png");
    SDL_Texture* spreadShotTexture = loadTexture(renderer, "infra/img/object/spread_shot.png");

    Mix_Chunk* bg_audio = loadSound("infra/sound/bg_audio.wav");
    Mix_Chunk* mouse_click = loadSound("infra/sound/mouse_click.wav");
    Mix_Chunk* lose_sound = loadSound("infra/sound/lose_sound.wav");
    Mix_Chunk* shoot_sound = loadSound("infra/sound/shoot_sound.wav");
    Mix_Chunk* health_sound = loadSound("infra/sound/health_sound.wav");
    Mix_Chunk* coin_sound = loadSound("infra/sound/coin_sound.wav");
    Mix_Chunk* loseheart_sound = loadSound("infra/sound/loseheart_sound.wav");

    Mix_PlayChannel(-1, bg_audio, -1);

    bool running = true;
    bool inMenu = true;
    bool inLevelSelect = false;
    bool gameRunning = false;
    bool helpScreen = false;
    bool gameOver = false;
    bool isPaused = false;
    bool inSettings = false;
    bool inStore = false;
    bool inExitOption = false;
    bool isDraggingStar = false;
    bool loseSoundPlayed = false;
    bool invincible = false;
    bool justLoaded = false;
    Uint32 invincibleTime = 0;
    GameLevel selectedLevel = NONE;

    SmallShip ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
    vector<Egg> eggs;
    vector<SmallEnemy> enemies;
    vector<Bullet> bullets;
    vector<Planet> planets;
    vector<Health> healthItems;
    vector<Coin> coins;
    vector<SpreadShot> spreadShots;

    int enemyCount = DEFAULT_ENEMY_COUNT;
    int enemiesSpawned = 0;
    int score = 0;
    int coinCount = 0;
    int highScore = loadHighScore();
    int lastPlanetScore = 0;
    int enemySpeed = SPEED;
    Uint32 enemyShootInterval = DEFAULT_ENEMY_SHOOT_INTERVAL;
    int lives = MAX_LIVES;
    Uint32 frameTime = 0;
    int currentFrame = 0;
    int planetWave = 1;
    int enemyKillCount = 0;
    int spreadShotCount = 0;

    int starX = VOLUME_BAR_X + VOLUME_BAR_WIDTH / 2 - STAR_SIZE / 2;
    int starY = VOLUME_BAR_Y - STAR_SIZE / 2;

    TTF_Font* font = TTF_OpenFont("infra/font/Backso.otf", 20);
    TTF_Font* smallFont = TTF_OpenFont("infra/font/Backso.otf", 20);

    int selectedShip = 0;
    bool shipsPurchased[5] = {true, false, false, false, false};
    int shipPrices[5] = {0, 10, 20, 30, 40};
    int shipEggCounts[5] = {1, 2, 3, 4, 5};

    if (loadGame(coinCount, selectedLevel, gameRunning, lives, selectedShip, spreadShotCount, shipsPurchased)) {
        SDL_Texture* ships[] = {smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture, manaShipTexture};
        ship.texture = ships[selectedShip];
        ship.eggCount = shipEggCounts[selectedShip];

        if (gameRunning) {
            inMenu = false;
            enemies.clear();
            planets.clear();
            healthItems.clear();
            coins.clear();
            spreadShots.clear();
            enemiesSpawned = 0;
            enemyCount = DEFAULT_ENEMY_COUNT;
            score = 0;
            justLoaded = true;

            if (selectedLevel == NOVICE) {
                enemySpeed = SPEED / 2;
                enemyShootInterval = 0;
                for (int i = 0; i < enemyCount; ++i) {
                    enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                       (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, false});
                    enemiesSpawned++;
                }
            } else if (selectedLevel == WARRIOR) {
                enemySpeed = SPEED;
                enemyShootInterval = DEFAULT_ENEMY_SHOOT_INTERVAL;
                enemyCount = 0;
                if (planets.empty()) spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
            } else if (selectedLevel == LEGEND) {
                enemySpeed = SPEED / 2;
                enemyShootInterval = 3000;
                for (int i = 0; i < enemyCount; ++i) {
                    enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                       (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, true});
                    enemiesSpawned++;
                }
            }
        }
    }

    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                inExitOption = true;
                inMenu = inLevelSelect = gameRunning = helpScreen = gameOver = isPaused = inSettings = inStore = false;
                cout << "Switched to Exit Option screen (SDL_QUIT)" << endl;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (mouseX >= WINDOW_WIDTH - 50 && mouseX <= WINDOW_WIDTH && mouseY >= WINDOW_HEIGHT - 50 && mouseY <= WINDOW_HEIGHT) {
                    inSettings = !inSettings;
                    Mix_PlayChannel(-1, mouse_click, 0);
                } else if (mouseX >= WINDOW_WIDTH - 100 && mouseX <= WINDOW_WIDTH - 50 && mouseY >= WINDOW_HEIGHT - 50 && mouseY <= WINDOW_HEIGHT) {
                    inStore = !inStore;
                    Mix_PlayChannel(-1, mouse_click, 0);
                }

                if (inSettings && !isDraggingStar) {
                    handleSettings(inMenu, inLevelSelect, gameRunning, helpScreen, gameOver, isPaused, inSettings, inStore, inExitOption,
                                   loseSoundPlayed, invincible, selectedLevel, ship, eggs, bullets, planets, enemies, healthItems, coins,
                                   spreadShots, enemyCount, enemiesSpawned, score, lastPlanetScore, enemySpeed, enemyShootInterval,
                                   lives, frameTime, currentFrame, planetWave, enemyKillCount, smallShipTexture, mouseX, mouseY, mouse_click);
                } else if (inStore) {
                    handleStore(coinCount, selectedShip, shipsPurchased, shipPrices, shipEggCounts, ship, smallShipTexture, commonShipTexture,
                                masterShipTexture, bigShipTexture, manaShipTexture, mouseX, mouseY, mouse_click);
                } else if (inMenu) {
                    handleMenu(inMenu, inLevelSelect, helpScreen, inExitOption, mouseX, mouseY, mouse_click);
                } else if (inLevelSelect) {
                    handleLevelSelect(inMenu, inLevelSelect, gameRunning, selectedLevel, enemySpeed, enemyShootInterval, enemyCount, planetWave,
                                      enemies, planets, healthItems, coins, spreadShots, enemiesSpawned, enemyTexture, planet1SpriteSheetTexture,
                                      planet2SpriteSheetTexture, planet3SpriteSheetTexture, mouseX, mouseY, mouse_click, event);
                } else if (gameRunning && !gameOver) {
                    if (mouseX >= 10 && mouseX <= 50 && mouseY >= 520 && mouseY <= 560) {
                        isPaused = !isPaused;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                } else if (gameOver) {
                    if (mouseX >= 340 && mouseX <= 625 && mouseY >= 260 && mouseY <= 300) {
                        gameOver = false;
                        lives = MAX_LIVES;
                        invincible = false;
                        ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, ship.texture, true, ship.eggCount};
                        eggs.clear();
                        enemies.clear();
                        bullets.clear();
                        planets.clear();
                        healthItems.clear();
                        coins.clear();
                        spreadShots.clear();
                        enemiesSpawned = score = lastPlanetScore = enemyKillCount = 0;
                        loseSoundPlayed = false;
                        planetWave = 1;
                        if (selectedLevel == NOVICE) {
                            enemyCount = DEFAULT_ENEMY_COUNT;
                            for (int i = 0; i < enemyCount; ++i) {
                                enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                   (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, false});
                                enemiesSpawned++;
                            }
                        } else if (selectedLevel == WARRIOR) {
                            enemyCount = 0;
                            spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
                        } else if (selectedLevel == LEGEND) {
                            enemyCount = DEFAULT_ENEMY_COUNT;
                            for (int i = 0; i < enemyCount; ++i) {
                                enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                   (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, true});
                                enemiesSpawned++;
                            }
                        }
                        Mix_PlayChannel(-1, mouse_click, 0);
                    } else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 330 && mouseY <= 370) {
                        inLevelSelect = true;
                        gameRunning = gameOver = false;
                        lives = MAX_LIVES;
                        invincible = false;
                        ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
                        eggs.clear();
                        bullets.clear();
                        planets.clear();
                        enemies.clear();
                        healthItems.clear();
                        coins.clear();
                        spreadShots.clear();
                        enemyCount = DEFAULT_ENEMY_COUNT;
                        enemiesSpawned = score = lastPlanetScore = enemyKillCount = 0;
                        loseSoundPlayed = false;
                        planetWave = 1;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    } else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 400 && mouseY <= 440) {
                        inExitOption = true;
                        inMenu = inLevelSelect = gameRunning = helpScreen = gameOver = isPaused = inSettings = inStore = false;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                } else if (inExitOption) {
                    handleExitOption(running, inMenu, inLevelSelect, gameRunning, helpScreen, gameOver, isPaused, inSettings, inStore, inExitOption,
                                     coinCount, selectedLevel, lives, selectedShip, spreadShotCount, score, enemyKillCount, shipsPurchased, ship,
                                     eggs, bullets, planets, enemies, healthItems, coins, spreadShots, enemyCount, enemiesSpawned, lastPlanetScore,
                                     enemySpeed, enemyShootInterval, frameTime, currentFrame, planetWave, smallShipTexture, mouseX, mouseY, mouse_click);
                }

                if (inSettings && mouseX >= starX && mouseX <= starX + STAR_SIZE && mouseY >= starY && mouseY <= starY + STAR_SIZE) {
                    isDraggingStar = true;
                }
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                isDraggingStar = false;
            } else if (event.type == SDL_MOUSEMOTION && isDraggingStar) {
                int mouseX = event.motion.x;
                starX = mouseX - STAR_SIZE / 2;
                starX = max(VOLUME_BAR_X, min(starX, VOLUME_BAR_X + VOLUME_BAR_WIDTH - STAR_SIZE));
                float volumeRatio = static_cast<float>(starX - VOLUME_BAR_X) / (VOLUME_BAR_WIDTH - STAR_SIZE);
                int volume = static_cast<int>(volumeRatio * 128);
                Mix_Volume(-1, volume);
                Mix_VolumeChunk(bg_audio, volume);
                Mix_VolumeChunk(mouse_click, volume);
                Mix_VolumeChunk(lose_sound, volume);
                Mix_VolumeChunk(shoot_sound, volume);
                Mix_VolumeChunk(health_sound, volume);
                Mix_VolumeChunk(coin_sound, volume);
                Mix_VolumeChunk(loseheart_sound, volume);
            } else if (event.type == SDL_KEYDOWN && gameRunning && !gameOver && !isPaused && !inSettings && !inStore) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: ship.dx = -SPEED; break;
                    case SDLK_RIGHT: ship.dx = SPEED; break;
                    case SDLK_UP: ship.dy = -SPEED; break;
                    case SDLK_DOWN: ship.dy = SPEED; break;
                    case SDLK_SPACE:
                        switch (ship.eggCount) {
                            case 1: eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true)); break;
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
                        Mix_PlayChannel(-1, shoot_sound, 0);
                        break;
                    case SDLK_x:
                        if (spreadShotCount > 0) {
                            spreadShotCount--;
                            for (int direction = 0; direction < 20; direction++) {
                                float angle = direction * (2 * M_PI / 20);
                                float dx = cos(angle) * 5;
                                float dy = sin(angle) * 5;
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y + SHIP_HEIGHT / 2 - EGG_SIZE / 2, static_cast<int>(dx), static_cast<int>(dy), true));
                            }
                            Mix_PlayChannel(-1, shoot_sound, 0);
                        }
                        break;
                }
            } else if (event.type == SDL_KEYUP && gameRunning && !gameOver && !isPaused && !inSettings && !inStore) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT: ship.dx = 0; break;
                    case SDLK_UP:
                    case SDLK_DOWN: ship.dy = 0; break;
                }
            } else if (helpScreen) {
                handleHelp(inMenu, helpScreen, event);
            }
        }

        if (gameRunning && !isPaused && !inSettings && !inStore && !gameOver) {
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
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
                                    enemy.lastShotTime = currentTime;
                                }
                            }
                        }
                    }
                    enemies.erase(remove_if(enemies.begin(), enemies.end(), [](const SmallEnemy& e) { return !e.active; }), enemies.end());

                    if (enemies.empty() && !justLoaded) {
                        enemyCount += 2;
                        enemiesSpawned = enemyCount;
                        for (int i = 0; i < enemyCount; ++i) {
                            enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                           (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, selectedLevel == LEGEND});
                        }
                    }
                justLoaded = false;
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
                        spawnPlanets(planets, 1, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
                        lastPlanetScore = score;
                } else if (selectedLevel == WARRIOR && planets.empty()) {
                        spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
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
                                    bullets.push_back(Bullet(planet.x + PLANET_WIDTH / 2 - BULLET_SIZE / 2, planet.y + PLANET_HEIGHT / 2, dx, dy, bulletTexture, true));
                                }
                                planet.lastShotTime = currentTime;
                            }
                        }
                    }

                    vector<pair<int, int>> destroyedPlanetPositions;
                    for (auto& egg : eggs) {
                        for (auto& planet : planets) {
                        if (egg.active && planet.active &&
                            egg.x + EGG_SIZE > planet.x && egg.x < planet.x + PLANET_WIDTH &&
                            egg.y + EGG_SIZE > planet.y && egg.y < planet.y + PLANET_HEIGHT) {
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
                planets.erase(remove_if(planets.begin(), planets.end(), [](const Planet& p) { return !p.active; }), planets.end());

                    for (const auto& pos : destroyedPlanetPositions) {
                    healthItems.push_back(Health(pos.first + PLANET_WIDTH / 2 - HEALTH_SIZE / 2, pos.second + PLANET_HEIGHT / 2 - HEALTH_SIZE / 2, true));
                    }
                }

                for (auto& egg : eggs) {
                    for (auto& enemy : enemies) {
                    if (egg.active && enemy.active &&
                        egg.x + EGG_SIZE > enemy.x && egg.x < enemy.x + ENEMY_WIDTH &&
                        egg.y + EGG_SIZE > enemy.y && egg.y < enemy.y + ENEMY_HEIGHT) {
                        egg.active = enemy.active = false;
                                score++;
                                enemyKillCount++;
                                coins.push_back(Coin(enemy.x + ENEMY_WIDTH / 2 - COIN_SIZE / 2, enemy.y + ENEMY_HEIGHT / 2 - COIN_SIZE / 2));
                                if (selectedLevel == NOVICE) {
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
                                }
                                if (enemyKillCount >= 20) {
                                    spreadShots.push_back(SpreadShot(rand() % (WINDOW_WIDTH - EGG_SIZE), 0, true));
                                    enemyKillCount = 0;
                            }
                        }
                    }
                }

                for (auto& spreadShot : spreadShots) {
                    if (spreadShot.active) {
                        spreadShot.y += 2;
                    if (spreadShot.y > WINDOW_HEIGHT) spreadShot.active = false;
                    if (spreadShotCount < MAX_SPREAD_SHOTS &&
                        ship.x + SHIP_WIDTH > spreadShot.x && ship.x < spreadShot.x + EGG_SIZE &&
                        ship.y + SHIP_HEIGHT > spreadShot.y && ship.y < spreadShot.y + EGG_SIZE) {
                                spreadShotCount++;
                                spreadShot.active = false;
                                Mix_PlayChannel(-1, coin_sound, 0);
                            }
                        }
                    }
            spreadShots.erase(remove_if(spreadShots.begin(), spreadShots.end(), [](const SpreadShot& s) { return !s.active; }), spreadShots.end());

                for (const auto& enemy : enemies) {
                if (enemy.active && !invincible &&
                    ship.x + SHIP_WIDTH > enemy.x && ship.x < enemy.x + ENEMY_WIDTH &&
                    ship.y + SHIP_HEIGHT > enemy.y && ship.y < enemy.y + ENEMY_HEIGHT) {
                            lives--;
                            Mix_PlayChannel(-1, loseheart_sound, 0);
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

            for (const auto& planet : planets) {
                if (planet.active && !invincible &&
                    ship.x + SHIP_WIDTH > planet.x && ship.x < planet.x + PLANET_WIDTH &&
                    ship.y + SHIP_HEIGHT > planet.y && ship.y < planet.y + PLANET_HEIGHT) {
                            lives--;
                            Mix_PlayChannel(-1, loseheart_sound, 0);
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

            for (const auto& bullet : bullets) {
                if (bullet.active && !invincible &&
                    ship.x + SHIP_WIDTH > bullet.x && ship.x < bullet.x + BULLET_SIZE &&
                    ship.y + SHIP_HEIGHT > bullet.y && ship.y < bullet.y + BULLET_SIZE) {
                    lives--;
                    Mix_PlayChannel(-1, loseheart_sound, 0);
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

            if (invincible && SDL_GetTicks() - invincibleTime >= INVINCIBLE_DURATION) invincible = false;

            for (auto& health : healthItems) {
                if (health.active &&
                    ship.x + SHIP_WIDTH > health.x && ship.x < health.x + HEALTH_SIZE &&
                    ship.y + SHIP_HEIGHT > health.y && ship.y < health.y + HEALTH_SIZE) {
                            if (lives < MAX_LIVES) {
                                lives++;
                                Mix_PlayChannel(-1, health_sound, 0);
                            }
                    health.active = false;
                        }
                    }
            healthItems.erase(remove_if(healthItems.begin(), healthItems.end(), [](const Health& h) { return !h.active; }), healthItems.end());

                for (auto& coin : coins) {
                if (coin.active &&
                    ship.x + SHIP_WIDTH > coin.x && ship.x < coin.x + COIN_SIZE &&
                    ship.y + SHIP_HEIGHT > coin.y && ship.y < coin.y + COIN_SIZE) {
                            coin.active = false;
                            coinCount++;
                            Mix_PlayChannel(-1, coin_sound, 0);
                        }
                    }
            coins.erase(remove_if(coins.begin(), coins.end(), [](const Coin& c) { return !c.active; }), coins.end());

                if (gameOver && score > highScore) {
                    highScore = score;
                    saveHighScore(highScore);
                }
            }

        if (inMenu) {
            renderMenu(renderer, menuBgTexture);
        } else if (inLevelSelect) {
            renderLevelSelect(renderer, levelBgTexture);
        } else if (gameRunning) {
            renderGame(renderer, ship, eggs, enemies, bullets, planets, healthItems, coins, spreadShots,
                       gameBgTexture, eggTexture, enemyTexture, bulletTexture, heartTexture, healthTexture,
                       coinSpriteSheetTexture, spreadShotTexture, pauseTexture, continueTexture, loseTexture,
                       font, lives, score, highScore, coinCount, spreadShotCount, frameTime, currentFrame,
                       isPaused, gameOver, loseSoundPlayed, lose_sound);
        } else if (helpScreen) {
            renderHelp(renderer, helpBgTexture);
        } else if (inExitOption) {
            renderExitOption(renderer, exitOptionTexture);
        }

        if (inStore) {
            renderStore(renderer, storeTexture, smallShipTexture, commonShipTexture, masterShipTexture,
                        bigShipTexture, manaShipTexture, eggTexture, chosenTexture, soldTexture, smallFont,
                        selectedShip, shipsPurchased);
        } else if (inSettings) {
            renderSettings(renderer, settingTexture, starTexture, starX, starY);
        }

        renderUIButtons(renderer, settingStarTexture, storeIconTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (score > highScore) saveHighScore(score);

    cleanup(window, renderer, menuBgTexture, levelBgTexture, gameBgTexture, helpBgTexture, loseTexture,
            exitOptionTexture, smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture,
            manaShipTexture, enemyTexture, bulletTexture, heartTexture, eggTexture, healthTexture,
            pauseTexture, continueTexture, starTexture, settingStarTexture, settingTexture, storeIconTexture,
            storeTexture, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture,
            coinSpriteSheetTexture, chosenTexture, soldTexture, spreadShotTexture,
            bg_audio, mouse_click, lose_sound, shoot_sound, health_sound, coin_sound, loseheart_sound,
            font, smallFont);

    return 0;
}
