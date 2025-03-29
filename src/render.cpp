#include "../include/render.h"
#include <SDL_image.h>
#include <string>

void renderGame(SDL_Renderer* renderer,
                const SmallShip& ship,
                const vector<Egg>& eggs,
                const vector<SmallEnemy>& enemies,
                const vector<Bullet>& bullets,
                const vector<Planet>& planets,
                const vector<Health>& healthItems,
                vector<Coin>& coins,
                const vector<SpreadShot>& spreadShots,
                SDL_Texture* gameBgTexture,
                SDL_Texture* eggTexture,
                SDL_Texture* enemyTexture,
                SDL_Texture* bulletTexture,
                SDL_Texture* heartTexture,
                SDL_Texture* healthTexture,
                SDL_Texture* coinSpriteSheetTexture,
                SDL_Texture* spreadShotTexture,
                SDL_Texture* pauseTexture,
                SDL_Texture* continueTexture,
                SDL_Texture* loseTexture,
                TTF_Font* font,
                int lives,
                int score,
                int highScore,
                int coinCount,
                int spreadShotCount,
                Uint32& frameTime,
                int& currentFrame,
                bool isPaused,
                bool gameOver,
                bool& loseSoundPlayed,
                Mix_Chunk* lose_sound) {
    SDL_RenderCopy(renderer, gameBgTexture, NULL, NULL);

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
    for (const auto& planet : planets) {
        if (planet.active) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - planet.lastFrameTime >= planet.frameDelay) {
                const_cast<Planet&>(planet).currentFrame = (planet.currentFrame + 1) % planet.totalFrames;
                const_cast<Planet&>(planet).lastFrameTime = currentTime;
            }

            planetSrcRect.x = planet.currentFrame * planet.frameWidth;
            planetSrcRect.w = planet.frameWidth;
            planetDestRect.x = planet.x;
            planetDestRect.y = planet.y;

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

    SDL_Rect spreadShotRect = {0, 0, EGG_SIZE, EGG_SIZE};
    for (const auto& spreadShot : spreadShots) {
        if (spreadShot.active) {
            spreadShotRect.x = spreadShot.x;
            spreadShotRect.y = spreadShot.y;
            SDL_RenderCopy(renderer, spreadShotTexture, NULL, &spreadShotRect);
        }
    }

    Uint32 currentTime = SDL_GetTicks();
    if (!isPaused && !gameOver && currentTime - frameTime >= FRAME_DELAY) {
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

    SDL_Rect spreadShotDisplayRect = {0, 0, HEART_SIZE, HEART_SIZE};
    for (int i = 0; i < spreadShotCount; i++) {
        spreadShotDisplayRect.x = WINDOW_WIDTH - (HEART_SIZE + 20) * (i + 1) - 10;
        spreadShotDisplayRect.y = 50;
        SDL_RenderCopy(renderer, spreadShotTexture, NULL, &spreadShotDisplayRect);
    }

    SDL_Color white = {255, 255, 255, 255};
    string scoreText = "Your score: " + to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    if (scoreSurface) {
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        if (scoreTexture) {
            SDL_Rect scoreRect = {10, 5, scoreSurface->w, scoreSurface->h};
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
            SDL_Rect highScoreRect = {10, 30, highScoreSurface->w, highScoreSurface->h};
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
            SDL_Rect coinRect = {10, 55, coinSurface->w, coinSurface->h};
            SDL_RenderCopy(renderer, coinTexture, NULL, &coinRect);
            SDL_DestroyTexture(coinTexture);
        }
        SDL_FreeSurface(coinSurface);
    }

    SDL_Rect buttonRect = {10, 520, 40, 40};
    SDL_RenderCopy(renderer, isPaused ? continueTexture : pauseTexture, NULL, &buttonRect);

    if (gameOver) {
        SDL_Rect loseRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, loseTexture, NULL, &loseRect);
        if (!loseSoundPlayed) {
            Mix_PlayChannel(-1, lose_sound, 0);
            loseSoundPlayed = true;
        }
    }
}

void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuBgTexture) {
    SDL_RenderCopy(renderer, menuBgTexture, NULL, NULL);
}

void renderLevelSelect(SDL_Renderer* renderer, SDL_Texture* levelBgTexture) {
    SDL_RenderCopy(renderer, levelBgTexture, NULL, NULL);
}

void renderHelp(SDL_Renderer* renderer, SDL_Texture* helpBgTexture) {
    SDL_RenderCopy(renderer, helpBgTexture, NULL, NULL);
}

void renderStore(SDL_Renderer* renderer,
                 SDL_Texture* storeTexture,
                 SDL_Texture* smallShipTexture,
                 SDL_Texture* commonShipTexture,
                 SDL_Texture* masterShipTexture,
                 SDL_Texture* bigShipTexture,
                 SDL_Texture* manaShipTexture,
                 SDL_Texture* eggTexture,
                 SDL_Texture* chosenTexture,
                 SDL_Texture* soldTexture,
                 TTF_Font* smallFont,
                 int selectedShip,
                 const bool shipsPurchased[5]) {
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

void renderSettings(SDL_Renderer* renderer,
                    SDL_Texture* settingTexture,
                    SDL_Texture* starTexture,
                    int starX,
                    int starY) {
    SDL_RenderCopy(renderer, settingTexture, NULL, NULL);
    SDL_Rect starRect = {starX, starY, STAR_SIZE, STAR_SIZE};
    SDL_RenderCopy(renderer, starTexture, NULL, &starRect);
}

void renderExitOption(SDL_Renderer* renderer, SDL_Texture* exitOptionTexture) {
    SDL_RenderCopy(renderer, exitOptionTexture, NULL, NULL);
}

void renderUIButtons(SDL_Renderer* renderer,
                     SDL_Texture* settingStarTexture,
                     SDL_Texture* storeIconTexture) {
    SDL_Rect settingButtonRect = {WINDOW_WIDTH - 50, WINDOW_HEIGHT - 50, 50, 50};
    SDL_RenderCopy(renderer, settingStarTexture, NULL, &settingButtonRect);

    SDL_Rect storeButtonRect = {WINDOW_WIDTH - 100, WINDOW_HEIGHT - 50, 50, 50};
    SDL_RenderCopy(renderer, storeIconTexture, NULL, &storeButtonRect);
}
