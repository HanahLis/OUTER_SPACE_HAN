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
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 563;
const int SHIP_WIDTH = 64;
const int SHIP_HEIGHT = 64;
const int ENEMY_WIDTH = 64;
const int ENEMY_HEIGHT = 64;
const int EGG_SIZE = 10;
const int BULLET_SIZE = 32;
const int SPEED = 5;
const int SCORE_TO_UPGRADE = 20;  // Nâng cấp từ small_ship thành common_ship
const int SCORE_TO_SUPER = 40;    // Nâng cấp từ common_ship thành super_ship

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
    bool upgraded;    // Nâng cấp thành common_ship
    bool super;       // Nâng cấp thành super_ship
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
    Uint32 lastShotTime; // Thời gian bắn đạn cuối cùng
    bool canShoot;       // Kẻ địch có thể bắn đạn hay không
};

struct Bullet {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;

    Bullet(int x_, int y_, int dx_, int dy_, SDL_Texture* texture_, bool active_)
        : x(x_), y(y_), dx(dx_), dy(dy_), texture(texture_), active(active_) {}
};

int main(int argc, char* argv[]) {
    // Seed random number generator
    srand(static_cast<unsigned int>(std::time(nullptr)));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "SDL initialization failed: " << SDL_GetError() << endl;
    }

    if (TTF_Init() < 0) {
        cout << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer initialization failed: " << Mix_GetError() << endl;
    }

    SDL_Window* window = SDL_CreateWindow("Outer Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window creation failed: " << SDL_GetError() << endl;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer creation failed: " << SDL_GetError() << endl;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image initialization failed: " << IMG_GetError() << endl;
    }

    // Load menu background
    SDL_Surface* menuBgSurface = IMG_Load("img/menu.png");
    if (!menuBgSurface) {
        cout << "Failed to load menu background: " << IMG_GetError() << endl;
    }
    SDL_Texture* menuBgTexture = SDL_CreateTextureFromSurface(renderer, menuBgSurface);
    SDL_FreeSurface(menuBgSurface);
    if (!menuBgTexture) {
        cout << "Failed to create menu background texture: " << SDL_GetError() << endl;
    }

    // Load game level background
    SDL_Surface* levelBgSurface = IMG_Load("img/level.png");
    if (!levelBgSurface) {
        cout << "Failed to load game level background: " << IMG_GetError() << endl;
    }
    SDL_Texture* levelBgTexture = SDL_CreateTextureFromSurface(renderer, levelBgSurface);
    SDL_FreeSurface(levelBgSurface);
    if (!levelBgTexture) {
        cout << "Failed to create game level background texture: " << SDL_GetError() << endl;
    }

    // Load game background
    SDL_Surface* gameBgSurface = IMG_Load("img/background.png");
    if (!gameBgSurface) {
        cout << "Failed to load game background: " << IMG_GetError() << endl;
    }
    SDL_Texture* gameBgTexture = SDL_CreateTextureFromSurface(renderer, gameBgSurface);
    SDL_FreeSurface(gameBgSurface);
    if (!gameBgTexture) {
        cout << "Failed to create game background texture: " << SDL_GetError() << endl;
    }

    // Load help background
    SDL_Surface* helpBgSurface = IMG_Load("img/help.png");
    if (!helpBgSurface) {
        cout << "Failed to load help background: " << IMG_GetError() << endl;
    }
    SDL_Texture* helpBgTexture = SDL_CreateTextureFromSurface(renderer, helpBgSurface);
    SDL_FreeSurface(helpBgSurface);
    if (!helpBgTexture) {
        cout << "Failed to create help background texture: " << SDL_GetError() << endl;
    }

    // Load small ship image
    SDL_Surface* smallShipSurface = IMG_Load("char/small_ship.png");
    if (!smallShipSurface) {
        cout << "Failed to load small ship image: " << IMG_GetError() << endl;
    }
    SDL_Texture* smallShipTexture = SDL_CreateTextureFromSurface(renderer, smallShipSurface);
    SDL_FreeSurface(smallShipSurface);
    if (!smallShipTexture) {
        cout << "Failed to create small ship texture: " << SDL_GetError() << endl;
    }

    // Load common ship image
    SDL_Surface* commonShipSurface = IMG_Load("char/common_ship.png");
    if (!commonShipSurface) {
        cout << "Failed to load common ship image: " << IMG_GetError() << endl;
    }
    SDL_Texture* commonShipTexture = SDL_CreateTextureFromSurface(renderer, commonShipSurface);
    SDL_FreeSurface(commonShipSurface);
    if (!commonShipTexture) {
        cout << "Failed to create common ship texture: " << SDL_GetError() << endl;
    }

    // Load super ship image
    SDL_Surface* superShipSurface = IMG_Load("char/super_ship.png");
    if (!superShipSurface) {
        cout << "Failed to load super ship image: " << IMG_GetError() << endl;
    }
    SDL_Texture* superShipTexture = SDL_CreateTextureFromSurface(renderer, superShipSurface);
    SDL_FreeSurface(superShipSurface);
    if (!superShipTexture) {
        cout << "Failed to create super ship texture: " << SDL_GetError() << endl;
    }

    // Load small enemy image
    SDL_Surface* enemySurface = IMG_Load("char/small_enemy.png");
    if (!enemySurface) {
        cout << "Failed to load enemy image: " << IMG_GetError() << endl;
    }
    SDL_Texture* enemyTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);
    SDL_FreeSurface(enemySurface);
    if (!enemyTexture) {
        cout << "Failed to create enemy texture: " << SDL_GetError() << endl;
    }

    // Load bullet image
    SDL_Surface* bulletSurface = IMG_Load("char/bullet.png");
    if (!bulletSurface) {
        cout << "Failed to load bullet image: " << IMG_GetError() << endl;
    }
    SDL_Texture* bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);
    SDL_FreeSurface(bulletSurface);
    if (!bulletTexture) {
        cout << "Failed to create bullet texture: " << SDL_GetError() << endl;
    }

    // Load egg image (placeholder)
    SDL_Surface* eggSurface = SDL_CreateRGBSurface(0, EGG_SIZE, EGG_SIZE, 32, 0, 0, 0, 0);
    SDL_FillRect(eggSurface, NULL, SDL_MapRGB(eggSurface->format, 255, 255, 255));
    SDL_Texture* eggTexture = SDL_CreateTextureFromSurface(renderer, eggSurface);
    SDL_FreeSurface(eggSurface);
    if (!eggTexture) {
        cout << "Failed to create egg texture: " << SDL_GetError() << endl;
    }

    // Load sound effects
    Mix_Chunk* bg_audio = Mix_LoadWAV("sound/bg_audio.wav");
    if (!bg_audio) {
        cout << "Failed to load bg_audio.wav: " << Mix_GetError() << endl;
    }

    Mix_Chunk* mouse_click = Mix_LoadWAV("sound/mouse_click.wav");
    if (!mouse_click) {
        cout << "Failed to load mouse_click.wav: " << Mix_GetError() << endl;
    }

    Mix_Chunk* lose_sound = Mix_LoadWAV("sound/lose_sound.wav");
    if (!lose_sound) {
        cout << "Failed to load lose_sound.wav: " << Mix_GetError() << endl;
    }

    Mix_Chunk* shoot_sound = Mix_LoadWAV("sound/shoot_sound.wav");
    if (!shoot_sound) {
        cout << "Failed to load shoot_sound.wav: " << Mix_GetError() << endl;
    }

    // Play background audio (looped)
    Mix_PlayChannel(-1, bg_audio, -1);

    // Game variables
    bool running = true;
    bool inMenu = true;
    bool inLevelSelect = false;
    bool gameRunning = false;
    bool helpScreen = false;
    bool gameOver = false;
    GameLevel selectedLevel = NONE;
    SDL_Event event;
    SmallShip ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, false, false};
    vector<Egg> eggs;
    vector<SmallEnemy> enemies;
    vector<Bullet> bullets;
    int enemyCount = 5;
    int enemiesSpawned = 0;
    int score = 0;
    int enemySpeed = SPEED; // Tốc độ của enemy, sẽ thay đổi theo level
    Uint32 enemyShootInterval = 2000; // Khoảng thời gian bắn đạn (ms)

    // Initialize font
    TTF_Font* font = TTF_OpenFont("font/pixel_font.ttf", 48);
    if (!font) {
        cout << "Failed to load font: " << TTF_GetError() << endl;
    }

    // Spawn initial 5 enemies
    for (int i = 0; i < enemyCount; ++i) {
        bool canShoot = false;
        if (selectedLevel == WARRIOR || selectedLevel == LEGEND) {
            canShoot = (rand() % 2 == 0); // 50% kẻ địch có thể bắn
        }
        enemies.push_back({rand() % (WINDOW_WIDTH - ENEMY_WIDTH), std::rand() % (WINDOW_HEIGHT / 2),
                          (rand() % 3 - 1) * SPEED, (std::rand() % 3 - 1) * SPEED, enemyTexture, true, 0, canShoot});
        enemiesSpawned++;
    }

    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (inMenu) {
                    // Check PLAY button
                    if (mouseX >= WINDOW_WIDTH / 2 - 75 && mouseX <= WINDOW_WIDTH / 2 + 75 &&
                        mouseY >= 300 && mouseY <= 350) {
                        inMenu = false;
                        inLevelSelect = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    // Check LEVEL button
                    else if (mouseX >= WINDOW_WIDTH / 2 - 75 && mouseX <= WINDOW_WIDTH / 2 + 75 &&
                             mouseY >= 360 && mouseY <= 410) {
                        inMenu = false;
                        inLevelSelect = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    // Check HELP button
                    else if (mouseX >= WINDOW_WIDTH / 2 - 75 && mouseX <= WINDOW_WIDTH / 2 + 75 &&
                             mouseY >= 420 && mouseY <= 470) {
                        inMenu = false;
                        helpScreen = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    // Check EXIT button
                    else if (mouseX >= WINDOW_WIDTH / 2 - 75 && mouseX <= WINDOW_WIDTH / 2 + 75 &&
                             mouseY >= 480 && mouseY <= 530) {
                        running = false;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                }
                else if (inLevelSelect) {
                    // NOVICE: Khoảng (200, 400, 300, 500)
                    if (mouseX >= 200 && mouseX <= 300 && mouseY >= 400 && mouseY <= 500) {
                        selectedLevel = NOVICE;
                        enemySpeed = SPEED / 2; // Tốc độ chậm
                        enemyShootInterval = 0; // Không bắn định kỳ
                        inLevelSelect = false;
                        gameRunning = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    // WARRIOR: Khoảng (450, 300, 550, 400)
                    else if (mouseX >= 450 && mouseX <= 550 && mouseY >= 300 && mouseY <= 400) {
                        selectedLevel = WARRIOR;
                        enemySpeed = SPEED; // Tốc độ trung bình
                        enemyShootInterval = 2000; // Bắn 1 đạn mỗi 2 giây
                        inLevelSelect = false;
                        gameRunning = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                    // LEGEND: Khoảng (700, 200, 800, 300)
                    else if (mouseX >= 700 && mouseX <= 800 && mouseY >= 200 && mouseY <= 300) {
                        selectedLevel = LEGEND;
                        enemySpeed = SPEED * 2; // Tốc độ nhanh
                        enemyShootInterval = 1500; // Bắn 2 đạn mỗi 1.5 giây
                        inLevelSelect = false;
                        gameRunning = true;
                        Mix_PlayChannel(-1, mouse_click, 0);
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN && gameRunning && !gameOver) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        ship.dx = -SPEED;
                        break;
                    case SDLK_RIGHT:
                        ship.dx = SPEED;
                        break;
                    case SDLK_UP:
                        ship.dy = -SPEED;
                        break;
                    case SDLK_DOWN:
                        ship.dy = SPEED;
                        break;
                    case SDLK_SPACE:
                        if (!ship.upgraded && !ship.super) {
                            eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                        } else if (ship.upgraded && !ship.super) {
                            eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 0, -5, true));
                            eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, -3, -3, true));
                            eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, 3, -3, true));
                        } else if (ship.super) {
                            for (int i = -2; i <= 2; ++i) {
                                eggs.push_back(Egg(ship.x + SHIP_WIDTH / 2 - EGG_SIZE / 2, ship.y, i * 2, -5 + std::abs(i), true));
                            }
                        }
                        Mix_PlayChannel(-1, shoot_sound, 0);
                        break;
                }
            }
            else if (event.type == SDL_KEYUP && gameRunning && !gameOver) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        ship.dx = 0;
                        break;
                    case SDLK_UP:
                    case SDLK_DOWN:
                        ship.dy = 0;
                        break;
                }
            }
            else if (event.type == SDL_KEYDOWN && helpScreen) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
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
            else if (event.type == SDL_KEYDOWN && gameOver) {
                if (event.key.keysym.sym == SDLK_h) {
                    // Reset game
                    gameOver = false;
                    ship = {WINDOW_WIDTH / 2, WINDOW_HEIGHT - SHIP_HEIGHT - 10, 0, 0, smallShipTexture, true, false, false};
                    eggs.clear();
                    enemies.clear();
                    bullets.clear();
                    enemyCount = 5;
                    enemiesSpawned = 0;
                    score = 0;
                    for (int i = 0; i < enemyCount; ++i) {
                        bool canShoot = false;
                        if (selectedLevel == WARRIOR || selectedLevel == LEGEND) {
                            canShoot = (std::rand() % 2 == 0); // 50% kẻ địch có thể bắn
                        }
                        enemies.push_back({std::rand() % (WINDOW_WIDTH - ENEMY_WIDTH), std::rand() % (WINDOW_HEIGHT / 2),
                                          (std::rand() % 3 - 1) * enemySpeed, (std::rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                        enemiesSpawned++;
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    running = false;
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

            // Move ship
            int newX = ship.x + ship.dx;
            int newY = ship.y + ship.dy;
            if (newX >= 0 && newX <= WINDOW_WIDTH - SHIP_WIDTH) ship.x = newX;
            if (newY >= 0 && newY <= WINDOW_HEIGHT - SHIP_HEIGHT) ship.y = newY;

            // Update eggs
            for (auto& egg : eggs) {
                if (egg.active) {
                    egg.x += egg.dx;
                    egg.y += egg.dy;
                    if (egg.y < 0 || egg.x < 0 || egg.x > WINDOW_WIDTH - EGG_SIZE) egg.active = false;
                }
            }
            eggs.erase(remove_if(eggs.begin(), eggs.end(), [](const Egg& e) { return !e.active; }), eggs.end());

            // Update enemies
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    enemy.x += enemy.dx;
                    enemy.y += enemy.dy;
                    if (enemy.x < 0 || enemy.x > WINDOW_WIDTH - ENEMY_WIDTH) enemy.dx = -enemy.dx;
                    if (enemy.y < 0 || enemy.y > WINDOW_HEIGHT / 2) enemy.dy = -enemy.dy;

                    // Enemy shoots based on level
                    if (enemy.canShoot && enemyShootInterval > 0) {
                        Uint32 currentTime = SDL_GetTicks();
                        if (currentTime - enemy.lastShotTime >= enemyShootInterval) {
                            if (selectedLevel == WARRIOR) {
                                // Bắn 1 viên đạn thẳng xuống
                                bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
                            }
                            else if (selectedLevel == LEGEND) {
                                // Bắn 2 viên đạn theo 2 hướng
                                bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, -3, 5, bulletTexture, true)); // Hướng trái dưới
                                bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 3, 5, bulletTexture, true));  // Hướng phải dưới
                            }
                            enemy.lastShotTime = currentTime;
                        }
                    }
                }
            }
            enemies.erase(remove_if(enemies.begin(), enemies.end(), [](const SmallEnemy& e) { return !e.active; }), enemies.end());

            // Spawn new enemies if all are defeated
            if (enemies.empty() && enemiesSpawned > 0) {
                enemyCount += 3;
                for (int i = 0; i < enemyCount; ++i) {
                    bool canShoot = false;
                    if (selectedLevel == WARRIOR || selectedLevel == LEGEND) {
                        canShoot = (std::rand() % 2 == 0); // 50% kẻ địch có thể bắn
                    }
                    enemies.push_back({std::rand() % (WINDOW_WIDTH - ENEMY_WIDTH), std::rand() % (WINDOW_HEIGHT / 2),
                                      (std::rand() % 3 - 1) * enemySpeed, (std::rand() % 3 - 1) * enemySpeed, enemyTexture, true, 0, canShoot});
                }
                enemiesSpawned += enemyCount;
            }

            // Update bullets
            for (auto& bullet : bullets) {
                if (bullet.active) {
                    bullet.x += bullet.dx;
                    bullet.y += bullet.dy;
                    if (bullet.y > WINDOW_HEIGHT || bullet.x < 0 || bullet.x > WINDOW_WIDTH - BULLET_SIZE) bullet.active = false;
                }
            }
            bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());

            // Collision detection (egg vs enemy)
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
                            if (score >= SCORE_TO_UPGRADE && !ship.upgraded) {
                                ship.texture = commonShipTexture;
                                ship.upgraded = true;
                            }
                            if (score >= SCORE_TO_SUPER && ship.upgraded && !ship.super) {
                                ship.texture = superShipTexture;
                                ship.super = true;
                            }
                            // Ở cấp độ Novice, kẻ địch rơi bullet khi bị tiêu diệt
                            if (selectedLevel == NOVICE) {
                                bullets.push_back(Bullet(enemy.x + ENEMY_WIDTH / 2 - BULLET_SIZE / 2, enemy.y, 0, 5, bulletTexture, true));
                            }
                        }
                    }
                }
            }

            // Collision detection (ship vs enemy or bullet)
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    int shipLeft = ship.x;
                    int shipRight = ship.x + SHIP_WIDTH;
                    int shipTop = ship.y;
                    int shipBottom = ship.y + SHIP_HEIGHT;
                    int enemyLeft = enemy.x;
                    int enemyRight = enemy.x + ENEMY_WIDTH;
                    int enemyTop = enemy.y;
                    int enemyBottom = enemy.y + ENEMY_HEIGHT;

                    if (shipRight > enemyLeft && shipLeft < enemyRight && shipBottom > enemyTop && shipTop < enemyBottom) {
                        gameOver = true;
                    }
                }
            }
            for (const auto& bullet : bullets) {
                if (bullet.active) {
                    int shipLeft = ship.x;
                    int shipRight = ship.x + SHIP_WIDTH;
                    int shipTop = ship.y;
                    int shipBottom = ship.y + SHIP_HEIGHT;
                    int bulletLeft = bullet.x;
                    int bulletRight = bullet.x + BULLET_SIZE;
                    int bulletTop = bullet.y;
                    int bulletBottom = bullet.y + BULLET_SIZE;

                    if (shipRight > bulletLeft && shipLeft < bulletRight && shipBottom > bulletTop && shipTop < bulletBottom) {
                        gameOver = true;
                    }
                }
            }

            // Render ship
            if (ship.active && !gameOver) {
                SDL_Rect shipRect = {ship.x, ship.y, SHIP_WIDTH, SHIP_HEIGHT};
                SDL_RenderCopy(renderer, ship.texture, NULL, &shipRect);
            }

            // Render eggs
            SDL_Rect eggRect = {0, 0, EGG_SIZE, EGG_SIZE};
            for (const auto& egg : eggs) {
                if (egg.active) {
                    eggRect.x = egg.x;
                    eggRect.y = egg.y;
                    SDL_RenderCopy(renderer, eggTexture, NULL, &eggRect);
                }
            }

            // Render enemies
            SDL_Rect enemyRect = {0, 0, ENEMY_WIDTH, ENEMY_HEIGHT};
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    enemyRect.x = enemy.x;
                    enemyRect.y = enemy.y;
                    SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyRect);
                }
            }

            // Render bullets
            SDL_Rect bulletRect = {0, 0, BULLET_SIZE, BULLET_SIZE};
            for (const auto& bullet : bullets) {
                if (bullet.active) {
                    bulletRect.x = bullet.x;
                    bulletRect.y = bullet.y;
                    SDL_RenderCopy(renderer, bulletTexture, NULL, &bulletRect);
                }
            }

            // Render score
            SDL_Color white = {255, 255, 255, 255};
            string scoreText = "Score: " + std::to_string(score);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                if (scoreTexture) {
                    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
                    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_FreeSurface(scoreSurface);
            }

            // Render game over
            if (gameOver) {
                SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "Game Over\nPress H to Replay, Enter to Exit", white);
                if (gameOverSurface) {
                    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
                    if (gameOverTexture) {
                        SDL_Rect gameOverRect = {WINDOW_WIDTH / 2 - gameOverSurface->w / 2, WINDOW_HEIGHT / 2 - gameOverSurface->h / 2,
                                               gameOverSurface->w, gameOverSurface->h};
                        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
                        SDL_DestroyTexture(gameOverTexture);
                    }
                    SDL_FreeSurface(gameOverSurface);
                    Mix_PlayChannel(-1, lose_sound, 0);
                }
            }
        }

        if (helpScreen) {
            SDL_RenderCopy(renderer, helpBgTexture, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    Mix_FreeChunk(bg_audio);
    Mix_FreeChunk(mouse_click);
    Mix_FreeChunk(lose_sound);
    Mix_FreeChunk(shoot_sound);
    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(menuBgTexture);
    SDL_DestroyTexture(levelBgTexture);
    SDL_DestroyTexture(gameBgTexture);
    SDL_DestroyTexture(helpBgTexture);
    SDL_DestroyTexture(smallShipTexture);
    SDL_DestroyTexture(commonShipTexture);
    SDL_DestroyTexture(superShipTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(eggTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

