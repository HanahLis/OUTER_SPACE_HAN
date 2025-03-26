#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.h"

struct SmallShip {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    int eggCount;

    SmallShip(int x_, int y_, int dx_, int dy_, SDL_Texture* texture_, bool active_, int eggCount_)
        : x(x_), y(y_), dx(dx_), dy(dy_), texture(texture_), active(active_), eggCount(eggCount_) {}
};

struct Egg {
    int x, y;
    int dx, dy;
    bool active;

    Egg(int x_, int y_, int dx_, int dy_, bool active_)
        : x(x_), y(y_), dx(dx_), dy(dy_), active(active_) {}
};

struct SmallEnemy {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    Uint32 lastShotTime;
    bool canShoot;

    SmallEnemy(int x_, int y_, int dx_, int dy_, SDL_Texture* texture_, bool active_, Uint32 lastShotTime_, bool canShoot_)
        : x(x_), y(y_), dx(dx_), dy(dy_), texture(texture_), active(active_), 
          lastShotTime(lastShotTime_), canShoot(canShoot_) {}
};

struct Bullet {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;

    Bullet(int x_, int y_, int dx_, int dy_, SDL_Texture* texture_, bool active_)
        : x(x_), y(y_), dx(dx_), dy(dy_), texture(texture_), active(active_) {}
};

struct Planet {
    int x, y;
    int health;
    SDL_Texture* texture;
    bool active;
    Uint32 lastShotTime;
    Uint32 shootInterval;
    float dx, dy;
    float speed;
    int currentFrame;
    Uint32 lastFrameTime;
    int totalFrames = 6;
    Uint32 frameDelay = 166;
    int frameWidth;

    Planet(int x_, int y_, SDL_Texture* texture_)
        : x(x_), y(y_), health(10), texture(texture_), active(true),
          lastShotTime(0), shootInterval(2000), dx(0), dy(0), speed(1.0f),
          currentFrame(0), lastFrameTime(0), frameWidth(0) {
        int textureWidth;
        SDL_QueryTexture(texture, NULL, NULL, &textureWidth, NULL);
        frameWidth = textureWidth / totalFrames;
    }
};

struct Health {
    int x, y;
    bool active;

    Health(int x_, int y_, bool active_)
        : x(x_), y(y_), active(active_) {}
};

struct Coin {
    int x, y;
    bool active;
    int currentFrame;
    Uint32 lastFrameTime;

    Coin(int x_, int y_)
        : x(x_), y(y_), active(true), currentFrame(0), lastFrameTime(0) {}
};
