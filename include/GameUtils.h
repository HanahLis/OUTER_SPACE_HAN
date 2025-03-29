#ifndef GAMEUTILS_H
#define GAMEUTILS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <iostream>
#include "constants.h"
#include "structs.h"

using namespace std;

bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer);

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);

Mix_Chunk* loadSound(const char* path);

void spawnPlanets(vector<Planet>& planets, int count, SDL_Texture* planet1SpriteSheetTexture,
                  SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture);

void cleanup(SDL_Window* window, SDL_Renderer* renderer,
             SDL_Texture* menuBgTexture, SDL_Texture* levelBgTexture, SDL_Texture* gameBgTexture,
             SDL_Texture* helpBgTexture, SDL_Texture* loseTexture, SDL_Texture* exitOptionTexture,
             SDL_Texture* smallShipTexture, SDL_Texture* commonShipTexture, SDL_Texture* masterShipTexture,
             SDL_Texture* bigShipTexture, SDL_Texture* manaShipTexture, SDL_Texture* enemyTexture,
             SDL_Texture* bulletTexture, SDL_Texture* heartTexture, SDL_Texture* eggTexture,
             SDL_Texture* healthTexture, SDL_Texture* pauseTexture, SDL_Texture* continueTexture,
             SDL_Texture* starTexture, SDL_Texture* settingStarTexture, SDL_Texture* settingTexture,
             SDL_Texture* storeIconTexture, SDL_Texture* storeTexture, SDL_Texture* planet1SpriteSheetTexture,
             SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture,
             SDL_Texture* coinSpriteSheetTexture, SDL_Texture* chosenTexture, SDL_Texture* soldTexture,
             SDL_Texture* spreadShotTexture,
             Mix_Chunk* bg_audio, Mix_Chunk* mouse_click, Mix_Chunk* lose_sound,
             Mix_Chunk* shoot_sound, Mix_Chunk* health_sound, Mix_Chunk* coin_sound,
             Mix_Chunk* loseheart_sound, TTF_Font* font, TTF_Font* smallFont);

#endif
