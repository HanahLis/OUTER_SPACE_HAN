#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "constants.h"
#include "structs.h"
using namespace std;

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
                Mix_Chunk* lose_sound);

void renderMenu(SDL_Renderer* renderer, SDL_Texture* menuBgTexture);

void renderLevelSelect(SDL_Renderer* renderer, SDL_Texture* levelBgTexture);

void renderHelp(SDL_Renderer* renderer, SDL_Texture* helpBgTexture);

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
                 const bool shipsPurchased[5]);

void renderSettings(SDL_Renderer* renderer,
                    SDL_Texture* settingTexture,
                    SDL_Texture* starTexture,
                    int starX,
                    int starY);

void renderExitOption(SDL_Renderer* renderer, SDL_Texture* exitOptionTexture);

void renderUIButtons(SDL_Renderer* renderer,
                     SDL_Texture* settingStarTexture,
                     SDL_Texture* storeIconTexture);

#endif
