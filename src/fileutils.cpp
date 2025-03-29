#include "../include/fileutils.h"

void saveGame(int coinCount, GameLevel selectedLevel, bool gameRunning, int lives, int selectedShip, int spreadShotCount, bool shipsPurchased[5]) {
    ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        saveFile << coinCount << endl
                 << static_cast<int>(selectedLevel) << endl
                 << gameRunning << endl
                 << lives << endl
                 << selectedShip << endl
                 << spreadShotCount << endl;
        for (int i = 0; i < 5; i++) {
            saveFile << shipsPurchased[i] << " ";
        }
        saveFile << endl;
        saveFile.close();
        cout << "Game saved successfully!" << endl;
    } else {
        cout << "Unable to open savegame.txt for writing!" << endl;
    }
}

bool loadGame(int& coinCount, GameLevel& selectedLevel, bool& gameRunning, int& lives, int& selectedShip, int& spreadShotCount, bool shipsPurchased[5]) {
    ifstream loadFile("savegame.txt");
    if (loadFile.is_open()) {
        int level;
        loadFile >> coinCount >> level >> gameRunning >> lives >> selectedShip >> spreadShotCount;
        selectedLevel = static_cast<GameLevel>(level);
        for (int i = 0; i < 5; i++) {
            loadFile >> shipsPurchased[i];
        }
        loadFile.close();
        return true;
    }
    return false;
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

int loadHighScore() {
    ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

