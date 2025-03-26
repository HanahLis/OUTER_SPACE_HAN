#include "../include/GameUtils.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

void spawnPlanets(vector<Planet>& planets, int count, SDL_Texture* planet1SpriteSheetTexture,
                 SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture) {
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
