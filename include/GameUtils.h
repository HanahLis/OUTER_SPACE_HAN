#pragma once
#include <vector>
#include <SDL.h>
#include "Structs.h"
using namespace std;

void spawnPlanets(vector<Planet>& planets, int count, SDL_Texture* planet1SpriteSheetTexture,
                 SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture);
int loadHighScore();
void saveHighScore(int highScore);
