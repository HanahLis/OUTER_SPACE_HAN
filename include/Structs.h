#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL.h>

struct SmallShip {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    int eggCount;
};

struct Egg {
    int x, y;
    int dx, dy;
    bool active;
    Egg(int x_, int y_, int dx_, int dy_, bool active_) : x(x_), y(y_), dx(dx_), dy(dy_), active(active_) {}
};

struct SmallEnemy {
    int x, y;
    int dx, dy;
    SDL_Texture* texture;
    bool active;
    Uint32 lastShotTime;
    bool canShoot;
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
    int totalFrames = PLANET_TOTAL_FRAMES;
    Uint32 frameDelay = PLANET_FRAME_DELAY;
    int frameWidth;
    Planet(int x_, int y_, SDL_Texture* texture_)
        : x(x_), y(y_), health(10), texture(texture_), active(true),
          lastShotTime(0), shootInterval(DEFAULT_PLANET_SHOOT_INTERVAL), dx(0), dy(0), speed(1.0f),
          currentFrame(0), lastFrameTime(0), frameWidth(0) {
        int textureWidth;
        SDL_QueryTexture(texture, NULL, NULL, &textureWidth, NULL);
        frameWidth = textureWidth / totalFrames;
    }
};

struct Health {
    int x, y;
    bool active;
    Health(int x_, int y_, bool active_) : x(x_), y(y_), active(active_) {}
};

struct Coin {
    int x, y;
    bool active;
    int currentFrame;
    Uint32 lastFrameTime;
    Coin(int x_, int y_) : x(x_), y(y_), active(true), currentFrame(0), lastFrameTime(0) {}
};

struct SpreadShot {
    int x, y;
    bool active;
    SpreadShot(int x_, int y_, bool active_) : x(x_), y(y_), active(active_) {}
};

#endif
