#include "../include/TextureManager.h"
#include <iostream>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

bool TextureManager::init(SDL_Renderer* renderer) {
    this->renderer = renderer;
    return true;
}

SDL_Texture* TextureManager::loadTexture(const string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        cout << "Failed to load image " << path << ": " << IMG_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        cout << "Failed to create texture from " << path << ": " << SDL_GetError() << endl;
        return nullptr;
    }

    textureMap[path] = texture;
    return texture;
}

SDL_Texture* TextureManager::getTexture(const string& key) {
    auto it = textureMap.find(key);
    return (it != textureMap.end()) ? it->second : nullptr;
}

bool TextureManager::loadGameTextures() {
    // Menu and Background textures
    menuBgTexture = loadTexture("infra/img/bg/menu.png");
    levelBgTexture = loadTexture("infra/img/bg/level.png");
    gameBgTexture = loadTexture("infra/img/bg/bg2.png");
    helpBgTexture = loadTexture("infra/img/bg/help.png");
    settingTexture = loadTexture("infra/img/bg/setting.png");
    storeTexture = loadTexture("infra/img/bg/store.png");
    loseTexture = loadTexture("infra/img/bg/lose.png");

    // Ship textures
    smallShipTexture = loadTexture("infra/img/ship/small_ship.png");
    commonShipTexture = loadTexture("infra/img/ship/common_ship.png");
    masterShipTexture = loadTexture("infra/img/ship/super_ship.png");
    bigShipTexture = loadTexture("infra/img/ship/big_ship.png");
    manaShipTexture = loadTexture("infra/img/ship/mana_ship.png");

    // Game object textures
    enemyTexture = loadTexture("infra/img/ship/small_enemy.png");
    eggTexture = loadTexture("infra/img/object/egg.png");
    bulletTexture = loadTexture("infra/img/object/bullet.png");
    heartTexture = loadTexture("infra/img/object/heart.png");
    healthTexture = loadTexture("infra/img/object/health.png");
    starTexture = loadTexture("infra/img/object/star.png");
    settingStarTexture = loadTexture("infra/img/object/settingstar.png");
    storeIconTexture = loadTexture("infra/img/object/storeicon.png");
    pauseTexture = loadTexture("infra/img/object/pause.png");
    continueTexture = loadTexture("infra/img/object/continue.png");
    chosenTexture = loadTexture("infra/img/object/chosen.png");
    soldTexture = loadTexture("infra/img/object/sold.png");

    // Planet and coin textures
    planet1SpriteSheetTexture = loadTexture("infra/img/object/planet1_spritesheet.png");
    planet2SpriteSheetTexture = loadTexture("infra/img/object/planet2_spritesheet.png");
    planet3SpriteSheetTexture = loadTexture("infra/img/object/planet3_spritesheet.png");
    coinSpriteSheetTexture = loadTexture("infra/img/object/coin.png");

    // Check if all textures loaded successfully
    return menuBgTexture && levelBgTexture && gameBgTexture && helpBgTexture &&
           settingTexture && storeTexture && loseTexture && smallShipTexture &&
           commonShipTexture && masterShipTexture && bigShipTexture && manaShipTexture &&
           enemyTexture && eggTexture && bulletTexture && heartTexture && healthTexture &&
           starTexture && settingStarTexture && storeIconTexture && pauseTexture &&
           continueTexture && chosenTexture && soldTexture && planet1SpriteSheetTexture &&
           planet2SpriteSheetTexture && planet3SpriteSheetTexture && coinSpriteSheetTexture;
}

void TextureManager::cleanup() {
    for (auto& pair : textureMap) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
            pair.second = nullptr;
        }
    }
    textureMap.clear();

    // Cleanup all texture pointers
    menuBgTexture = nullptr;
    levelBgTexture = nullptr;
    gameBgTexture = nullptr;
    helpBgTexture = nullptr;
    settingTexture = nullptr;
    storeTexture = nullptr;
    loseTexture = nullptr;
    smallShipTexture = nullptr;
    commonShipTexture = nullptr;
    masterShipTexture = nullptr;
    bigShipTexture = nullptr;
    manaShipTexture = nullptr;
    enemyTexture = nullptr;
    eggTexture = nullptr;
    bulletTexture = nullptr;
    heartTexture = nullptr;
    healthTexture = nullptr;
    starTexture = nullptr;
    settingStarTexture = nullptr;
    storeIconTexture = nullptr;
    pauseTexture = nullptr;
    continueTexture = nullptr;
    chosenTexture = nullptr;
    soldTexture = nullptr;
    planet1SpriteSheetTexture = nullptr;
    planet2SpriteSheetTexture = nullptr;
    planet3SpriteSheetTexture = nullptr;
    coinSpriteSheetTexture = nullptr;
}
