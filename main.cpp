#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <fstream>
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 563;
const int SHIP_WIDTH = 64;
const int SHIP_HEIGHT = 64;
const int ENEMY_WIDTH = 64;
const int ENEMY_HEIGHT = 64;
const int PLANET_WIDTH = 128;
const int PLANET_HEIGHT = 128;
const int EGG_SIZE = 64;
const int BULLET_SIZE = 64;
const int SPEED = 5;
const int HEART_SIZE = 32;
const int HEART_SPRITE_SIZE = 16;
const int HEART_FRAMES = 5;
const int HEALTH_SIZE = 32;
const int MAX_LIVES = 3;
const int COIN_SIZE = 20;
const int COIN_FRAMES = 9;

const int VOLUME_BAR_X = 300;
const int VOLUME_BAR_Y = 200;
const int VOLUME_BAR_WIDTH = 400;
const int VOLUME_BAR_HEIGHT = 20;
const int STAR_SIZE = 50;

enum GameLevel {
    NONE,
    NOVICE,
    WARRIOR,
    LEGEND
};

struct SmallShip {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    int eggCount; // Number of eggs this ship fires
};

struct Egg {
    int x, y;
    int dx, dy;
    bool active;

    Egg(int x_, int y_, int dx_, int dy_, bool active_)
        : x(x_), y(y_), dx(dx_), dy(dy_), active(active_) {}
};

struct SmallEnemy {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    Uint32 lastShotTime;
    bool canShoot;
};

struct Bullet {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;

    Bullet(int x_, int y_, int dx_, int dy_, SDL_Texture* texture_, bool active_)
        : x(x_), y(y_), dx(dx_), dy(dy_), texture(texture_), active(active_) {}
};

struct Planet {
    int x, y;
    int health;
    SDL_Texture* texture;
    bool active;
    Uint32 lastShotTime;
    Uint32 shootInterval;
    float dx, dy;
    float speed;
    int currentFrame;
    Uint32 lastFrameTime;
    int totalFrames = 6;
    Uint32 frameDelay = 166;
    int frameWidth;

    Planet(int x_, int y_, SDL_Texture* texture_)
        : x(x_), y(y_), health(10), texture(texture_), active(true),
          lastShotTime(0), shootInterval(2000), dx(0), dy(0), speed(1.0f),
          currentFrame(0), lastFrameTime(0), frameWidth(0) {
        int textureWidth;
        SDL_QueryTexture(texture, NULL, NULL, &textureWidth, NULL);
        frameWidth = textureWidth / totalFrames;
    }
};

struct Health {
    int x, y;
    bool active;

    Health(int x_, int y_, bool active_)
        : x(x_), y(y_), active(active_) {}
};

struct Coin {
    int x, y;
    bool active;
    int currentFrame;
    Uint32 lastFrameTime;

    Coin(int x_, int y_)
        : x(x_), y(y_), active(true), currentFrame(0), lastFrameTime(0) {}
};

void spawnPlanets(vector<Planet>& planets, int count, SDL_Texture* planet1SpriteSheetTexture, SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture) {
    for (int i = 0; i < count; ++i) {
        int planetX = rand() % (WINDOW_WIDTH - PLANET_WIDTH);
        int planetY = rand() % (WINDOW_HEIGHT / 2);
        SDL_Texture* planetTexture;
        int planetType = rand() % 3;
        if (planetType == 0) planetTexture = planet1SpriteSheetTexture;
        else if (planetType == 1) planetTexture = planet2SpriteSheetTexture;
        else planetTexture = planet3SpriteSheetTexture;
        planets.push_back(Planet(planetX, planetY, planetTexture));
        planets.back().dx = (rand() % 3 - 1) * 1.0f;
        planets.back().dy = (rand() % 3 - 1) * 1.0f;
    }
}

int loadHighScore() {
    ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

void saveHighScore(int highScore) {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    } else {
        cout << "Unable to open highscore.txt for writing!" << endl;
    }
}

int loadCoinCount() {
    ifstream file("coin_count.txt");
    int coinCount = 0;
    if (file.is_open()) {
        file >> coinCount;
        file.close();
    }
    return coinCount;
}

void saveCoinCount(int coinCount) {
    ofstream file("coin_count.txt");
    if (file.is_open()) {
        file << coinCount;
        file.close();
    } else {
        cout << "Unable to open coin_count.txt for writing!" << endl;
    }
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(std::time(nullptr)));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "SDL initialization failed: " << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        cout << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer initialization failed: " << Mix_GetError() << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Outer Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window creation failed: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer creation failed: " << SDL_GetError() << endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image initialization failed: " << IMG_GetError() << endl;
        return 1;
    }

    SDL_Surface* menuBgSurface = IMG_Load("img/bg/menu.png");
    if (!menuBgSurface) cout << "Failed to load menu.png: " << IMG_GetError() << endl;
    SDL_Texture* menuBgTexture = SDL_CreateTextureFromSurface(renderer, menuBgSurface);
    SDL_FreeSurface(menuBgSurface);

    SDL_Surface* levelBgSurface = IMG_Load("img/bg/level.png");
    if (!levelBgSurface) cout << "Failed to load level.png: " << IMG_GetError() << endl;
    SDL_Texture* levelBgTexture = SDL_CreateTextureFromSurface(renderer, levelBgSurface);
    SDL_FreeSurface(levelBgSurface);

    SDL_Surface* gameBgSurface = IMG_Load("img/bg/bg2.png");
    if (!gameBgSurface) cout << "Failed to load bg2.png: " << IMG_GetError() << endl;
    SDL_Texture* gameBgTexture = SDL_CreateTextureFromSurface(renderer, gameBgSurface);
    SDL_FreeSurface(gameBgSurface);

    SDL_Surface* helpBgSurface = IMG_Load("img/bg/help.png");
    if (!helpBgSurface) cout << "Failed to load help.png: " << IMG_GetError() << endl;
    SDL_Texture* helpBgTexture = SDL_CreateTextureFromSurface(renderer, helpBgSurface);
    SDL_FreeSurface(helpBgSurface);

    SDL_Surface* loseSurface = IMG_Load("img/bg/lose.png");
    if (!loseSurface) cout << "Failed to load lose.png: " << IMG_GetError() << endl;
    SDL_Texture* loseTexture = SDL_CreateTextureFromSurface(renderer, loseSurface);
    SDL_FreeSurface(loseSurface);

    SDL_Surface* smallShipSurface = IMG_Load("img/ship/small_ship.png");
    if (!smallShipSurface) cout << "Failed to load small_ship.png: " << IMG_GetError() << endl;
    SDL_Texture* smallShipTexture = SDL_CreateTextureFromSurface(renderer, smallShipSurface);
    SDL_FreeSurface(smallShipSurface);

    SDL_Surface* commonShipSurface = IMG_Load("img/ship/common_ship.png");
    if (!commonShipSurface) cout << "Failed to load common_ship.png: " << IMG_GetError() << endl;
    SDL_Texture* commonShipTexture = SDL_CreateTextureFromSurface(renderer, commonShipSurface);
    SDL_FreeSurface(commonShipSurface);

    SDL_Surface* masterShipSurface = IMG_Load("img/ship/super_ship.png");
    if (!masterShipSurface) cout << "Failed to load super_ship.png: " << IMG_GetError() << endl;
    SDL_Texture* masterShipTexture = SDL_CreateTextureFromSurface(renderer, masterShipSurface);
    SDL_FreeSurface(masterShipSurface);

    SDL_Surface* bigShipSurface = IMG_Load("img/ship/big_ship.png");
    if (!bigShipSurface) cout << "Failed to load big_ship.png: " << IMG_GetError() << endl;
    SDL_Texture* bigShipTexture = SDL_CreateTextureFromSurface(renderer, bigShipSurface);
    SDL_FreeSurface(bigShipSurface);

    SDL_Surface* manaShipSurface = IMG_Load("img/ship/mana_ship.png");
    if (!manaShipSurface) cout << "Failed to load mana_ship.png: " << IMG_GetError() << endl;
    SDL_Texture* manaShipTexture = SDL_CreateTextureFromSurface(renderer, manaShipSurface);
    SDL_FreeSurface(manaShipSurface);

    SDL_Surface* enemySurface = IMG_Load("img/ship/small_enemy.png");
    if (!enemySurface) cout << "Failed to load small_enemy.png: " << IMG_GetError() << endl;
    SDL_Texture* enemyTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);
    SDL_FreeSurface(enemySurface);

    SDL_Surface* bulletSurface = IMG_Load("img/object/bullet.png");
    if (!bulletSurface) cout << "Failed to load bullet.png: " << IMG_GetError() << endl;
    SDL_Texture* bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);
    SDL_FreeSurface(bulletSurface);

    SDL_Surface* heartSurface = IMG_Load("img/object/heart.png");
    if (!heartSurface) cout << "Failed to load heart.png: " << IMG_GetError() << endl;
    SDL_Texture* heartTexture = SDL_CreateTextureFromSurface(renderer, heartSurface);
    SDL_FreeSurface(heartSurface);

    SDL_Surface* eggSurface = IMG_Load("img/object/egg.png");
    if (!eggSurface) cout << "Failed to load egg.png: " << IMG_GetError() << endl;
    SDL_Texture* eggTexture = SDL_CreateTextureFromSurface(renderer, eggSurface);
    SDL_FreeSurface(eggSurface);

    SDL_Surface* healthSurface = IMG_Load("img/object/health.png");
    if (!healthSurface) cout << "Failed to load health.png: " << IMG_GetError() << endl;
    SDL_Texture* healthTexture = SDL_CreateTextureFromSurface(renderer, healthSurface);
    SDL_FreeSurface(healthSurface);

    SDL_Surface* pauseSurface = IMG_Load("img/object/pause.png");
    if (!pauseSurface) cout << "Failed to load pause.png: " << IMG_GetError() << endl;
    SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    SDL_FreeSurface(pauseSurface);

    SDL_Surface* continueSurface = IMG_Load("img/object/continue.png");
    if (!continueSurface) cout << "Failed to load continue.png: " << IMG_GetError() << endl;
    SDL_Texture* continueTexture = SDL_CreateTextureFromSurface(renderer, continueSurface);
    SDL_FreeSurface(continueSurface);

    SDL_Surface* starSurface = IMG_Load("img/object/star.png");
    if (!starSurface) cout << "Failed to load star.png: " << IMG_GetError() << endl;
    SDL_Texture* starTexture = SDL_CreateTextureFromSurface(renderer, starSurface);
    SDL_FreeSurface(starSurface);

    SDL_Surface* settingStarSurface = IMG_Load("img/object/settingstar.png");
    if (!settingStarSurface) cout << "Failed to load settingstar.png: " << IMG_GetError() << endl;
    SDL_Texture* settingStarTexture = SDL_CreateTextureFromSurface(renderer, settingStarSurface);
    SDL_FreeSurface(settingStarSurface);

    SDL_Surface* settingSurface = IMG_Load("img/bg/setting.png");
    if (!settingSurface) cout << "Failed to load setting.png: " << IMG_GetError() << endl;
    SDL_Texture* settingTexture = SDL_CreateTextureFromSurface(renderer, settingSurface);
    SDL_FreeSurface(settingSurface);

    SDL_Surface* storeIconSurface = IMG_Load("img/object/storeicon.png");
    if (!storeIconSurface) cout << "Failed to load storeicon.png: " << IMG_GetError() << endl;
    SDL_Texture* storeIconTexture = SDL_CreateTextureFromSurface(renderer, storeIconSurface);
    SDL_FreeSurface(storeIconSurface);

    SDL_Surface* storeSurface = IMG_Load("img/bg/store.png");
    if (!storeSurface) cout << "Failed to load store.png: " << IMG_GetError() << endl;
    SDL_Texture* storeTexture = SDL_CreateTextureFromSurface(renderer, storeSurface);
    SDL_FreeSurface(storeSurface);

    SDL_Surface* planet1SpriteSheetSurface = IMG_Load("img/object/planet1_spritesheet.png");
    if (!planet1SpriteSheetSurface) cout << "Failed to load planet1_spritesheet.png: " << IMG_GetError() << endl;
    SDL_Texture* planet1SpriteSheetTexture = SDL_CreateTextureFromSurface(renderer, planet1SpriteSheetSurface);
    SDL_FreeSurface(planet1SpriteSheetSurface);

    SDL_Surface* planet2SpriteSheetSurface = IMG_Load("img/object/planet2_spritesheet.png");
    if (!planet2SpriteSheetSurface) cout << "Failed to load planet2_spritesheet.png: " << IMG_GetError() << endl;
    SDL_Texture* planet2SpriteSheetTexture = SDL_CreateTextureFromSurface(renderer, planet2SpriteSheetSurface);
    SDL_FreeSurface(planet2SpriteSheetSurface);

    SDL_Surface* planet3SpriteSheetSurface = IMG_Load("img/object/planet3_spritesheet.png");
    if (!planet3SpriteSheetSurface) cout << "Failed to load planet3_spritesheet.png: " << IMG_GetError() << endl;
    SDL_Texture* planet3SpriteSheetTexture = SDL_CreateTextureFromSurface(renderer, planet3SpriteSheetSurface);
    SDL_FreeSurface(planet3SpriteSheetSurface);

    SDL_Surface* coinSpriteSheetSurface = IMG_Load("img/object/coin.png");
    if (!coinSpriteSheetSurface) cout << "Failed to load coin.png: " << IMG_GetError() << endl;
    SDL_Texture* coinSpriteSheetTexture = SDL_CreateTextureFromSurface(renderer, coinSpriteSheetSurface);
    SDL_FreeSurface(coinSpriteSheetSurface);

    SDL_Surface* chosenSurface = IMG_Load("img/object/chosen.png");
    if (!chosenSurface) cout << "Failed to load chosen.png: " << IMG_GetError() << endl;
    SDL_Texture* chosenTexture = SDL_CreateTextureFromSurface(renderer, chosenSurface);
    SDL_FreeSurface(chosenSurface);

    SDL_Surface* soldSurface = IMG_Load("img/object/sold.png");
    if (!soldSurface) cout << "Failed to load sold.png: " << IMG_GetError() << endl;
    SDL_Texture* soldTexture = SDL_CreateTextureFromSurface(renderer, soldSurface);
    SDL_FreeSurface(soldSurface);

    Mix_Chunk* bg_audio = Mix_LoadWAV("sound/bg_audio.wav");
    Mix_Chunk* mouse_click = Mix_LoadWAV("sound/mouse_click.wav");
    Mix_Chunk* lose_sound = Mix_LoadWAV("sound/lose_sound.wav");
    Mix_Chunk* shoot_sound = Mix_LoadWAV("sound/shoot_sound.wav");
    Mix_Chunk* health_sound = Mix_LoadWAV("sound/health_sound.wav");
    Mix_Chunk* coin_sound = Mix_LoadWAV("sound/coin_sound.wav");
    if (!coin_sound) cout << "Failed to load coin_sound.wav: " << Mix_GetError() << endl;
    Mix_Chunk* loseheart_sound = Mix_LoadWAV("sound/loseheart_sound.wav");
    if (!loseheart_sound) cout << "Failed to load loseheart_sound.wav: " << Mix_GetError() << endl;

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
    bool isDraggingStar = false;
    bool loseSoundPlayed = false;
    bool invincible = false; // Thêm trạng thái miễn nhiễm
    Uint32 invincibleTime = 0; // Thời gian bắt đầu miễn nhiễm
    const Uint32 INVINCIBLE_DURATION = 1000; // 1 giây miễn nhiễm
    GameLevel selectedLevel = NONE;
    SDL_Event event;
    SmallShip ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
    vector<Egg> eggs;
    vector<SmallEnemy> enemies;
    vector<Bullet> bullets;
    vector<Planet> planets;
    vector<Health> healthItems;
    vector<Coin> coins;
    int enemyCount = 3;
    int enemiesSpawned = 0;
    int score = 0;
    int coinCount = loadCoinCount();
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

    TTF_Font* font = TTF_OpenFont("font/pixel_font.ttf", 24);
    TTF_Font* smallFont = TTF_OpenFont("font/pixel_font.ttf", 18);

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

                int settingButtonX = WINDOW_WIDTH - 60;
                int settingButtonY = WINDOW_HEIGHT - 60;
                int settingButtonWidth = 30;
                int settingButtonHeight = 30;
                if (mouseX >= settingButtonX && mouseX <= settingButtonX + settingButtonWidth &&
                    mouseY >= settingButtonY && mouseY <= settingButtonY + settingButtonHeight) {
                    inSettings = !inSettings;
                    Mix_PlayChannel(-1, mouse_click, 0);
                }

                int storeButtonX = WINDOW_WIDTH - 100;
                int storeButtonY = WINDOW_HEIGHT - 60;
                int storeButtonWidth = 30;
                int storeButtonHeight = 30;
                if (mouseX >= storeButtonX && mouseX <= storeButtonX + storeButtonWidth &&
                    mouseY >= storeButtonY && mouseY <= storeButtonY + storeButtonHeight) {
                    inStore = !inStore;
                    Mix_PlayChannel(-1, mouse_click, 0);
                }

                if (inSettings) {
                    if (mouseX >= starX && mouseX <= starX + STAR_SIZE &&
                        mouseY >= starY && mouseY <= starY + STAR_SIZE) {
                        isDraggingStar = true;
                    }
                }

                if (inSettings && !isDraggingStar) {
                    if (mouseX >= 400 && mouseX <= 600 && mouseY >= 300 && mouseY <= 350) {
                        inMenu = true;
                        inLevelSelect = false;
                        gameRunning = false;
                        helpScreen = false;
                        gameOver = false;
                        isPaused = false;
                        inSettings = false;
                        inStore = false;
                        loseSoundPlayed = false;
                        invincible = false; // Reset miễn nhiễm
                        selectedLevel = NONE;
                        ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
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
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    else if (mouseX >= 400 && mouseX <= 600 && mouseY >= 360 && mouseY <= 410) {
                        inSettings = false;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    else if (mouseX >= 400 && mouseX <= 600 && mouseY >= 420 && mouseY <= 470) {
                        running = false;
                        Mix_PlayChannel(-1, mouse_click, 0);
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
                                SDL_Texture* ships[] = {smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture, manaShipTexture};
                                ship.texture = ships[selectedShip];
                                ship.eggCount = shipEggCounts[selectedShip];
                                Mix_PlayChannel(-1, mouse_click, 0);
                            } else if (coinCount >= shipPrices[i]) {
                                coinCount -= shipPrices[i];
                                shipsPurchased[i] = true;
                                selectedShip = i;
                                SDL_Texture* ships[] = {smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture, manaShipTexture};
                                ship.texture = ships[selectedShip];
                                ship.eggCount = shipEggCounts[selectedShip];
                                saveCoinCount(coinCount);
                                Mix_PlayChannel(-1, mouse_click, 0);
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
                            Mix_PlayChannel(-1, mouse_click, 0);
                        }
                        else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 405 && mouseY <= 445) {
                            inMenu = false;
                            helpScreen = true;
                            Mix_PlayChannel(-1, mouse_click, 0);
                        }
                        else if (mouseX >= 410 && mouseX <= 575 && mouseY >= 465 && mouseY <= 505) {
                            running = false;
                            Mix_PlayChannel(-1, mouse_click, 0);
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
                                enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                  (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                                enemiesSpawned++;
                            }
                            Mix_PlayChannel(-1, mouse_click, 0);
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
                            spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
                            Mix_PlayChannel(-1, mouse_click, 0);
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
                                enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                  (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                                enemiesSpawned++;
                            }
                            Mix_PlayChannel(-1, mouse_click, 0);
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
                            Mix_PlayChannel(-1, mouse_click, 0);
                        }
                    }
                    else if (gameOver) {
                        if (mouseX >= 340 && mouseX <= 625 && mouseY >= 260 && mouseY <= 300) {
                            gameOver = false;
                            lives = 3;
                            invincible = false; // Reset miễn nhiễm khi chơi lại
                            ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, ship.texture, true, ship.eggCount};
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
                                    enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                      (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                                    enemiesSpawned++;
                                }
                            }
                            else if (selectedLevel == WARRIOR) {
                                spawnPlanets(planets, planetWave, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
                            }
                            else if (selectedLevel == LEGEND) {
                                enemyCount = 3;
                                for (int i = 0; i < enemyCount; ++i) {
                                    bool canShoot = true;
                                    enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                                      (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                                    enemiesSpawned++;
                                }
                            }
                            Mix_PlayChannel(-1, mouse_click, 0);
                        }
                        else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 330 && mouseY <= 370) {
                            inMenu = false;
                            inLevelSelect = true;
                            gameRunning = false;
                            gameOver = false;
                            lives = 3;
                            invincible = false; // Reset miễn nhiễm
                            ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, 1};
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
                            Mix_PlayChannel(-1, mouse_click, 0);
                        }
                        else if (mouseX >= 340 && mouseX <= 625 && mouseY >= 400 && mouseY <= 440) {
                            running = false;
                            Mix_PlayChannel(-1, mouse_click, 0);
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
                Mix_Volume(-1, volume);
                Mix_VolumeChunk(bg_audio, volume);
                Mix_VolumeChunk(mouse_click, volume);
                Mix_VolumeChunk(lose_sound, volume);
                Mix_VolumeChunk(shoot_sound, volume);
                Mix_VolumeChunk(health_sound, volume);
                Mix_VolumeChunk(coin_sound, volume);
                Mix_VolumeChunk(loseheart_sound, volume);
            }
            else if (event.type == SDL_KEYDOWN && gameRunning && !gameOver && !isPaused && !inSettings && !inStore) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: ship.dx = -SPEED; break;
                    case SDLK_RIGHT: ship.dx = SPEED; break;
                    case SDLK_UP: ship.dy = -SPEED; break;
                    case SDLK_DOWN: ship.dy = SPEED; break;
                    case SDLK_SPACE:
                        switch (ship.eggCount) {
                            case 1: // small_ship
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                break;
                            case 2: // common_ship
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 10, ship.y, 0, -5, true));
                                break;
                            case 3: // super_ship
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 20, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 20, ship.y, 1, -5, true));
                                break;
                            case 4: // big_ship
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 30, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 10, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 30, ship.y, 1, -5, true));
                                break;
                            case 5: // mana_ship
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 40, ship.y, -2, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 - 20, ship.y, -1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 20, ship.y, 1, -5, true));
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2 + 40, ship.y, 2, -5, true));
                                break;
                        }
                        Mix_PlayChannel(-1, shoot_sound, 0);
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
            SDL_RenderCopy(renderer, menuBgTexture, NULL, NULL);
        }

        if (inLevelSelect) {
            SDL_RenderCopy(renderer, levelBgTexture, NULL, NULL);
        }

        if (gameRunning) {
            SDL_RenderCopy(renderer, gameBgTexture, NULL, NULL);

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
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
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
                            enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), rand() % (WINDOW_HEIGHT / 2),
                                              (rand() % 3 - 1) * enemySpeed, (rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
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
                        spawnPlanets(planets, 1, planet1SpriteSheetTexture, planet2SpriteSheetTexture, planet3SpriteSheetTexture);
                        lastPlanetScore = score;
                    }

                    if (selectedLevel == WARRIOR && planets.empty()) {
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
                                    bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
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
                            Mix_PlayChannel(-1, loseheart_sound, 0);
                            invincible = true;
                            invincibleTime = SDL_GetTicks();
                            if (lives <= 0) {
                                gameOver = true;
                            } else {
                                ship.x = WINDOW_WIDTH / 2;
                                ship.y = WINDOW_HEIGHT - SHIP_HEIGHT - 10;
                            }
                            break; // Thoát vòng lặp sau khi mất mạng
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
                            Mix_PlayChannel(-1, loseheart_sound, 0);
                            invincible = true;
                            invincibleTime = SDL_GetTicks();
                            if (lives <= 0) {
                                gameOver = true;
                            } else {
                                ship.x = WINDOW_WIDTH / 2;
                                ship.y = WINDOW_HEIGHT - SHIP_HEIGHT - 10;
                            }
                            break; // Thoát vòng lặp sau khi mất mạng
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
                                Mix_PlayChannel(-1, health_sound, 0);
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
                            Mix_PlayChannel(-1, coin_sound, 0);
                            saveCoinCount(coinCount);
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
                SDL_RenderCopy(renderer, ship.texture, NULL, &shipRect);
            }

            SDL_Rect eggRect = {0, 0, EGG_SIZE, EGG_SIZE};
            for (const auto& egg : eggs) {
                if (egg.active) {
                    eggRect.x = egg.x;
                    eggRect.y = egg.y;
                    SDL_RenderCopy(renderer, eggTexture, NULL, &eggRect);
                }
            }

            SDL_Rect enemyRect = {0, 0, ENEMY_WIDTH, ENEMY_HEIGHT};
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    enemyRect.x = enemy.x;
                    enemyRect.y = enemy.y;
                    SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyRect);
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

                    SDL_RenderCopy(renderer, planet.texture, &planetSrcRect, &planetDestRect);

                    SDL_Color white = {255, 255, 255, 255};
                    string healthText = to_string(planet.health);
                    SDL_Surface* healthSurface = TTF_RenderText_Solid(font, healthText.c_str(), white);
                    if (healthSurface) {
                        SDL_Texture* healthTexture = SDL_CreateTextureFromSurface(renderer, healthSurface);
                        if (healthTexture) {
                            SDL_Rect healthRect = {planet.x + PLANET_WIDTH / 2 - healthSurface->w / 2, planet.y - 20, healthSurface->w, healthSurface->h};
                            SDL_RenderCopy(renderer, healthTexture, NULL, &healthRect);
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
                    SDL_RenderCopy(renderer, bulletTexture, NULL, &bulletRect);
                }
            }

            SDL_Rect healthRect = {0, 0, HEALTH_SIZE, HEALTH_SIZE};
            for (const auto& health : healthItems) {
                if (health.active) {
                    healthRect.x = health.x;
                    healthRect.y = health.y;
                    SDL_RenderCopy(renderer, healthTexture, NULL, &healthRect);
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
                    SDL_RenderCopy(renderer, coinSpriteSheetTexture, &coinSrcRect, &coinDestRect);
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
                SDL_RenderCopy(renderer, heartTexture, &heartSrc, &heartDisplayRect);
            }

            SDL_Color white = {255, 255, 255, 255};
            string scoreText = "Your score: " + to_string(score);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                if (scoreTexture) {
                    int scoreX = 10;
                    int scoreY = 5;
                    SDL_Rect scoreRect = {scoreX, scoreY, scoreSurface->w, scoreSurface->h};
                    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_FreeSurface(scoreSurface);
            }

            string highScoreText = "High score: " + to_string(highScore);
            SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), white);
            if (highScoreSurface) {
                SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
                if (highScoreTexture) {
                    int highScoreX = 10;
                    int highScoreY = 30;
                    SDL_Rect highScoreRect = {highScoreX, highScoreY, highScoreSurface->w, highScoreSurface->h};
                    SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);
                    SDL_DestroyTexture(highScoreTexture);
                }
                SDL_FreeSurface(highScoreSurface);
            }

            string coinText = "Coin: " + to_string(coinCount);
            SDL_Surface* coinSurface = TTF_RenderText_Solid(font, coinText.c_str(), white);
            if (coinSurface) {
                SDL_Texture* coinTexture = SDL_CreateTextureFromSurface(renderer, coinSurface);
                if (coinTexture) {
                    int coinX = 10;
                    int coinY = 55;
                    SDL_Rect coinRect = {coinX, coinY, coinSurface->w, coinSurface->h};
                    SDL_RenderCopy(renderer, coinTexture, NULL, &coinRect);
                    SDL_DestroyTexture(coinTexture);
                }
                SDL_FreeSurface(coinSurface);
            }

            SDL_Rect buttonRect = {10, 520, 40, 40};
            if (isPaused) {
                SDL_RenderCopy(renderer, continueTexture, NULL, &buttonRect);
            } else {
                SDL_RenderCopy(renderer, pauseTexture, NULL, &buttonRect);
            }

            if (gameOver) {
                SDL_Rect loseRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                SDL_RenderCopy(renderer, loseTexture, NULL, &loseRect);

                if (!loseSoundPlayed) {
                    Mix_PlayChannel(-1, lose_sound, 0);
                    loseSoundPlayed = true;
                }
            }
        }

        if (helpScreen) {
            SDL_RenderCopy(renderer, helpBgTexture, NULL, NULL);
        }

        if (inStore) {
            SDL_RenderCopy(renderer, storeTexture, NULL, NULL);

            SDL_Color white = {255, 255, 255, 255};
            string prices[] = {"Free", "10 coins", "20 coins", "30 coins", "40 coins"};
            int priceX = 105;
            int priceYStart = 175;
            int rowHeight = 80;

            for (int i = 0; i < 5; i++) {
                SDL_Surface* priceSurface = TTF_RenderText_Solid(smallFont, prices[i].c_str(), white);
                if (priceSurface) {
                    SDL_Texture* priceTexture = SDL_CreateTextureFromSurface(renderer, priceSurface);
                    if (priceTexture) {
                        SDL_Rect priceRect = {priceX, priceYStart + i * rowHeight, priceSurface->w, priceSurface->h};
                        SDL_RenderCopy(renderer, priceTexture, NULL, &priceRect);
                        SDL_DestroyTexture(priceTexture);
                    }
                    SDL_FreeSurface(priceSurface);
                }
            }

            SDL_Texture* ships[] = {smallShipTexture, commonShipTexture, masterShipTexture, bigShipTexture, manaShipTexture};
            int shipX = 360;
            int shipYStart = 160;

            for (int i = 0; i < 5; i++) {
                SDL_Rect shipRect = {shipX, shipYStart + i * rowHeight, SHIP_WIDTH, SHIP_HEIGHT};
                SDL_RenderCopy(renderer, ships[i], NULL, &shipRect);

                if (i == selectedShip && shipsPurchased[i]) {
                    SDL_Rect chosenRect = {shipX + SHIP_WIDTH, shipYStart + i * rowHeight, 32, 32};
                    SDL_RenderCopy(renderer, chosenTexture, NULL, &chosenRect);
                }

                if (shipsPurchased[i]) {
                    SDL_Rect soldRect = {shipX, shipYStart + i * rowHeight, SHIP_WIDTH, SHIP_HEIGHT};
                    SDL_RenderCopy(renderer, soldTexture, NULL, &soldRect);
                }
            }

            int eggX = 600;
            int eggYStart = 160;
            int eggCounts[] = {1, 2, 3, 4, 5};

            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < eggCounts[i]; j++) {
                    SDL_Rect eggRect = {eggX + j * (EGG_SIZE + 5), eggYStart + i * rowHeight, EGG_SIZE, EGG_SIZE};
                    SDL_RenderCopy(renderer, eggTexture, NULL, &eggRect);
                }
            }
        }

        if (inSettings) {
            SDL_RenderCopy(renderer, settingTexture, NULL, NULL);
            SDL_Rect starRect = {starX, starY, STAR_SIZE, STAR_SIZE};
            SDL_RenderCopy(renderer, starTexture, NULL, &starRect);
        }

        SDL_Rect settingButtonRect = {WINDOW_WIDTH - 60, WINDOW_HEIGHT - 60, 50, 50};
        SDL_RenderCopy(renderer, settingStarTexture, NULL, &settingButtonRect);

        SDL_Rect storeButtonRect = {WINDOW_WIDTH - 120, WINDOW_HEIGHT - 60, 50, 50};
        SDL_RenderCopy(renderer, storeIconTexture, NULL, &storeButtonRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (score > highScore) {
        highScore = score;
        saveHighScore(highScore);
    }
    saveCoinCount(coinCount);

    Mix_FreeChunk(bg_audio);
    Mix_FreeChunk(mouse_click);
    Mix_FreeChunk(lose_sound);
    Mix_FreeChunk(shoot_sound);
    Mix_FreeChunk(health_sound);
    Mix_FreeChunk(coin_sound);
    Mix_FreeChunk(loseheart_sound);
    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_CloseFont(smallFont);
    TTF_Quit();
    SDL_DestroyTexture(menuBgTexture);
    SDL_DestroyTexture(levelBgTexture);
    SDL_DestroyTexture(gameBgTexture);
    SDL_DestroyTexture(helpBgTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyTexture(smallShipTexture);
    SDL_DestroyTexture(commonShipTexture);
    SDL_DestroyTexture(masterShipTexture);
    SDL_DestroyTexture(bigShipTexture);
    SDL_DestroyTexture(manaShipTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(eggTexture);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(healthTexture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyTexture(continueTexture);
    SDL_DestroyTexture(starTexture);
    SDL_DestroyTexture(settingStarTexture);
    SDL_DestroyTexture(settingTexture);
    SDL_DestroyTexture(storeIconTexture);
    SDL_DestroyTexture(storeTexture);
    SDL_DestroyTexture(planet1SpriteSheetTexture);
    SDL_DestroyTexture(planet2SpriteSheetTexture);
    SDL_DestroyTexture(planet3SpriteSheetTexture);
    SDL_DestroyTexture(coinSpriteSheetTexture);
    SDL_DestroyTexture(chosenTexture);
    SDL_DestroyTexture(soldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
