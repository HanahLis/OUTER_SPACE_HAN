#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
using namespace std;

class SDLManager {
public:
    static SDLManager& getInstance();
    bool init(const string& title, int width, int height);
    void cleanup();

    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }

private:
    SDLManager() : window(nullptr), renderer(nullptr) {}
    ~SDLManager() {}
    SDLManager(const SDLManager&) = delete;
    SDLManager& operator=(const SDLManager&) = delete;

    SDL_Window* window;
    SDL_Renderer* renderer;
};
