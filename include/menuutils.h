#ifndef MENUUTILS_H
#define MENUUTILS_H

#include <SDL.h>
#include <SDL_mixer.h>
#include "constants.h"
#include "render.h"
#include "gameutils.h"
#include "fileutils.h"
#include <iostream>

using namespace std;

void handleMenu(bool& inMenu, bool& inLevelSelect, bool& helpScreen, bool& inExitOption, int mouseX, int mouseY, Mix_Chunk* mouse_click);

void handleLevelSelect(bool& inMenu, bool& inLevelSelect, bool& gameRunning, GameLevel& selectedLevel, int& enemySpeed, Uint32& enemyShootInterval,
                       int& enemyCount, int& planetWave, vector<SmallEnemy>& enemies, vector<Planet>& planets, vector<Health>& healthItems,
                       vector<Coin>& coins, vector<SpreadShot>& spreadShots, int& enemiesSpawned, SDL_Texture* enemyTexture,
                       SDL_Texture* planet1SpriteSheetTexture, SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture,
                       int mouseX, int mouseY, Mix_Chunk* mouse_click, SDL_Event& event);

void handleSettings(bool& inMenu, bool& inLevelSelect, bool& gameRunning, bool& helpScreen, bool& gameOver, bool& isPaused,
                    bool& inSettings, bool& inStore, bool& inExitOption, bool& loseSoundPlayed, bool& invincible,
                    GameLevel& selectedLevel, SmallShip& ship, vector<Egg>& eggs, vector<Bullet>& bullets, vector<Planet>& planets,
                    vector<SmallEnemy>& enemies, vector<Health>& healthItems, vector<Coin>& coins, vector<SpreadShot>& spreadShots,
                    int& enemyCount, int& enemiesSpawned, int& score, int& lastPlanetScore, int& enemySpeed, Uint32& enemyShootInterval,
                    int& lives, Uint32& frameTime, int& currentFrame, int& planetWave, int& enemyKillCount,
                    SDL_Texture* smallShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click);

void handleStore(int& coinCount, int& selectedShip, bool shipsPurchased[5], int shipPrices[5], int shipEggCounts[5],
                 SmallShip& ship, SDL_Texture* smallShipTexture, SDL_Texture* commonShipTexture, SDL_Texture* masterShipTexture,
                 SDL_Texture* bigShipTexture, SDL_Texture* manaShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click);

void handleExitOption(bool& running, bool& inMenu, bool& inLevelSelect, bool& gameRunning, bool& helpScreen, bool& gameOver,
                      bool& isPaused, bool& inSettings, bool& inStore, bool& inExitOption, int& coinCount, GameLevel& selectedLevel,
                      int& lives, int& selectedShip, int& spreadShotCount, int& score, int& enemyKillCount, bool shipsPurchased[5],
                      SmallShip& ship, vector<Egg>& eggs, vector<Bullet>& bullets, vector<Planet>& planets, vector<SmallEnemy>& enemies,
                      vector<Health>& healthItems, vector<Coin>& coins, vector<SpreadShot>& spreadShots, int& enemyCount,
                      int& enemiesSpawned, int& lastPlanetScore, int& enemySpeed, Uint32& enemyShootInterval, Uint32& frameTime,
                      int& currentFrame, int& planetWave, SDL_Texture* smallShipTexture, int mouseX, int mouseY, Mix_Chunk* mouse_click);

void handleHelp(bool& inMenu, bool& helpScreen, SDL_Event& event);

#endif
