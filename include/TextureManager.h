#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <map>
using namespace std;

class TextureManager {
public:
    static TextureManager& getInstance();
    bool init(SDL_Renderer* renderer);
    void cleanup();

    SDL_Texture* loadTexture(const string& path);
    SDL_Texture* getTexture(const string& key);

    // Game specific texture loaders
    bool loadGameTextures();

    // Menu and Background textures
    SDL_Texture* getMenuBgTexture() { return menuBgTexture; }
    SDL_Texture* getLevelBgTexture() { return levelBgTexture; }
    SDL_Texture* getGameBgTexture() { return gameBgTexture; }
    SDL_Texture* getHelpBgTexture() { return helpBgTexture; }
    SDL_Texture* getSettingTexture() { return settingTexture; }
    SDL_Texture* getStoreTexture() { return storeTexture; }
    SDL_Texture* getLoseTexture() { return loseTexture; }

    // Ship textures
    SDL_Texture* getSmallShipTexture() { return smallShipTexture; }
    SDL_Texture* getCommonShipTexture() { return commonShipTexture; }
    SDL_Texture* getMasterShipTexture() { return masterShipTexture; }
    SDL_Texture* getBigShipTexture() { return bigShipTexture; }
    SDL_Texture* getManaShipTexture() { return manaShipTexture; }

    // Game object textures
    SDL_Texture* getEnemyTexture() { return enemyTexture; }
    SDL_Texture* getEggTexture() { return eggTexture; }
    SDL_Texture* getBulletTexture() { return bulletTexture; }
    SDL_Texture* getHeartTexture() { return heartTexture; }
    SDL_Texture* getHealthTexture() { return healthTexture; }
    SDL_Texture* getStarTexture() { return starTexture; }
    SDL_Texture* getSettingStarTexture() { return settingStarTexture; }
    SDL_Texture* getStoreIconTexture() { return storeIconTexture; }
    SDL_Texture* getPauseTexture() { return pauseTexture; }
    SDL_Texture* getContinueTexture() { return continueTexture; }
    SDL_Texture* getChosenTexture() { return chosenTexture; }
    SDL_Texture* getSoldTexture() { return soldTexture; }

    // Planet textures
    SDL_Texture* getPlanet1SpriteSheetTexture() { return planet1SpriteSheetTexture; }
    SDL_Texture* getPlanet2SpriteSheetTexture() { return planet2SpriteSheetTexture; }
    SDL_Texture* getPlanet3SpriteSheetTexture() { return planet3SpriteSheetTexture; }
    SDL_Texture* getCoinSpriteSheetTexture() { return coinSpriteSheetTexture; }

private:
    TextureManager() {}
    ~TextureManager() {}
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    SDL_Renderer* renderer = nullptr;
    map<string, SDL_Texture*> textureMap;

    // Menu and Background textures
    SDL_Texture* menuBgTexture = nullptr;
    SDL_Texture* levelBgTexture = nullptr;
    SDL_Texture* gameBgTexture = nullptr;
    SDL_Texture* helpBgTexture = nullptr;
    SDL_Texture* settingTexture = nullptr;
    SDL_Texture* storeTexture = nullptr;
    SDL_Texture* loseTexture = nullptr;

    // Ship textures
    SDL_Texture* smallShipTexture = nullptr;
    SDL_Texture* commonShipTexture = nullptr;
    SDL_Texture* masterShipTexture = nullptr;
    SDL_Texture* bigShipTexture = nullptr;
    SDL_Texture* manaShipTexture = nullptr;

    // Game object textures
    SDL_Texture* enemyTexture = nullptr;
    SDL_Texture* eggTexture = nullptr;
    SDL_Texture* bulletTexture = nullptr;
    SDL_Texture* heartTexture = nullptr;
    SDL_Texture* healthTexture = nullptr;
    SDL_Texture* starTexture = nullptr;
    SDL_Texture* settingStarTexture = nullptr;
    SDL_Texture* storeIconTexture = nullptr;
    SDL_Texture* pauseTexture = nullptr;
    SDL_Texture* continueTexture = nullptr;
    SDL_Texture* chosenTexture = nullptr;
    SDL_Texture* soldTexture = nullptr;

    // Planet and coin textures
    SDL_Texture* planet1SpriteSheetTexture = nullptr;
    SDL_Texture* planet2SpriteSheetTexture = nullptr;
    SDL_Texture* planet3SpriteSheetTexture = nullptr;
    SDL_Texture* coinSpriteSheetTexture = nullptr;
};
