#include "../include/SDLManager.h"
#include <iostream>

SDLManager& SDLManager::getInstance() {
    static SDLManager instance;
    return instance;
}

bool SDLManager::init(const string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "SDL initialization failed: " << SDL_GetError() << endl;
        return false;
    }

    if (TTF_Init() < 0) {
        cout << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer initialization failed: " << Mix_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window creation failed: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer creation failed: " << SDL_GetError() << endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image initialization failed: " << IMG_GetError() << endl;
        return false;
    }

    return true;
}

void SDLManager::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
