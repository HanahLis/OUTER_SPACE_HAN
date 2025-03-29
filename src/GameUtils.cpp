#include "../include/gameutils.h"

using namespace std;

bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || TTF_Init() < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL initialization failed: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Outer Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!window || !renderer || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Resource creation failed: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "Failed to load " << path << ": " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        cout << "Failed to create texture from " << path << ": " << SDL_GetError() << endl;
    }
    SDL_FreeSurface(surface);
    return texture;
}

Mix_Chunk* loadSound(const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (!sound) cout << "Failed to load " << path << ": " << Mix_GetError() << endl;
    return sound;
}

void spawnPlanets(vector<Planet>& planets, int count, SDL_Texture* planet1SpriteSheetTexture, SDL_Texture* planet2SpriteSheetTexture, SDL_Texture* planet3SpriteSheetTexture) {
    for (int i = 0; i < count; ++i) {
        int planetX = rand() % (WINDOW_WIDTH - PLANET_WIDTH);
        int planetY = rand() % (WINDOW_HEIGHT / 2);
        SDL_Texture* planetTexture = (rand() % 3 == 0) ? planet1SpriteSheetTexture : (rand() % 2 == 0) ? planet2SpriteSheetTexture : planet3SpriteSheetTexture;
        planets.push_back(Planet(planetX, planetY, planetTexture));
        planets.back().dx = (rand() % 3 - 1) * 1.0f;
        planets.back().dy = (rand() % 3 - 1) * 1.0f;
    }
}

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
             Mix_Chunk* loseheart_sound, TTF_Font* font, TTF_Font* smallFont) {
    Mix_FreeChunk(bg_audio);
    Mix_FreeChunk(mouse_click);
    Mix_FreeChunk(lose_sound);
    Mix_FreeChunk(shoot_sound);
    Mix_FreeChunk(health_sound);
    Mix_FreeChunk(coin_sound);
    Mix_FreeChunk(loseheart_sound);
    Mix_CloseAudio();

    TTF_CloseFont(font);
    TTF_CloseFont(smallFont);
    TTF_Quit();

    SDL_DestroyTexture(menuBgTexture);
    SDL_DestroyTexture(levelBgTexture);
    SDL_DestroyTexture(gameBgTexture);
    SDL_DestroyTexture(helpBgTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyTexture(exitOptionTexture);
    SDL_DestroyTexture(smallShipTexture);
    SDL_DestroyTexture(commonShipTexture);
    SDL_DestroyTexture(masterShipTexture);
    SDL_DestroyTexture(bigShipTexture);
    SDL_DestroyTexture(manaShipTexture);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(eggTexture);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(healthTexture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyTexture(continueTexture);
    SDL_DestroyTexture(starTexture);
    SDL_DestroyTexture(settingStarTexture);
    SDL_DestroyTexture(settingTexture);
    SDL_DestroyTexture(storeIconTexture);
    SDL_DestroyTexture(storeTexture);
    SDL_DestroyTexture(planet1SpriteSheetTexture);
    SDL_DestroyTexture(planet2SpriteSheetTexture);
    SDL_DestroyTexture(planet3SpriteSheetTexture);
    SDL_DestroyTexture(coinSpriteSheetTexture);
    SDL_DestroyTexture(chosenTexture);
    SDL_DestroyTexture(soldTexture);
    SDL_DestroyTexture(spreadShotTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}
