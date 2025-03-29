#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>

enum GameLevel {
    NONE,
    NOVICE,
    WARRIOR,
    LEGEND
};

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 563;

const int SHIP_WIDTH = 64;
const int SHIP_HEIGHT = 64;
const int ENEMY_WIDTH = 64;
const int ENEMY_HEIGHT = 64;
const int PLANET_WIDTH = 128;
const int PLANET_HEIGHT = 128;
const int EGG_SIZE = 64;
const int BULLET_SIZE = 32;
const int HEART_SIZE = 32;
const int HEART_SPRITE_SIZE = 16;
const int HEALTH_SIZE = 32;
const int COIN_SIZE = 20;
const int STAR_SIZE = 50;

const int HEART_FRAMES = 5;
const int COIN_FRAMES = 9;
const int PLANET_TOTAL_FRAMES = 6;

const int SPEED = 5;
const Uint32 FRAME_DELAY = 100;
const Uint32 PLANET_FRAME_DELAY = 166;
const Uint32 INVINCIBLE_DURATION = 1000;

const int MAX_LIVES = 3;
const int MAX_SPREAD_SHOTS = 3;

const int VOLUME_BAR_X = 300;
const int VOLUME_BAR_Y = 200;
const int VOLUME_BAR_WIDTH = 400;
const int VOLUME_BAR_HEIGHT = 20;

const int DEFAULT_ENEMY_COUNT = 3;
const Uint32 DEFAULT_ENEMY_SHOOT_INTERVAL = 2000;
const Uint32 DEFAULT_PLANET_SHOOT_INTERVAL = 2000;

#endif
