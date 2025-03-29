#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <fstream>
#include <iostream>
#include <vector>
#include "constants.h"
#include "gameutils.h"

using namespace std;

void saveGame(int coinCount, GameLevel selectedLevel, bool gameRunning, int lives, int selectedShip, int spreadShotCount, bool shipsPurchased[5]);

bool loadGame(int& coinCount, GameLevel& selectedLevel, bool& gameRunning, int& lives, int& selectedShip, int& spreadShotCount, bool shipsPurchased[5]);

void saveHighScore(int highScore);

int loadHighScore();

#endif
