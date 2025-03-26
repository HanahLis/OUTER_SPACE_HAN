#include "../include/SoundManager.h"
#include <iostream>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

bool SoundManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer initialization failed: " << Mix_GetError() << endl;
        return false;
    }
    return true;
}

Mix_Chunk* SoundManager::loadSound(const string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        cout << "Failed to load sound " << path << ": " << Mix_GetError() << std::endl;
        return nullptr;
    }
    soundMap[path] = sound;
    return sound;
}

Mix_Music* SoundManager::loadMusic(const string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        cout << "Failed to load music " << path << ": " << Mix_GetError() << std::endl;
        return nullptr;
    }
    musicMap[path] = music;
    return music;
}

bool SoundManager::loadGameSounds() {
    bg_audio = loadSound("infra/sound/bg_audio.wav");
    mouse_click = loadSound("infra/sound/mouse_click.wav");
    explosion_sound = loadSound("infra/sound/loseheart_sound.wav");
    game_over_sound = loadSound("infra/sound/lose_sound.wav");
    lose_sound = loadSound("infra/sound/lose_sound.wav");
    collect_sound = loadSound("infra/sound/coin_sound.wav");
    shoot_sound = loadSound("infra/sound/shoot_sound.wav");
    loseheart_sound = loadSound("infra/sound/loseheart_sound.wav");
    health_sound = loadSound("infra/sound/health_sound.wav");
    coin_sound = loadSound("infra/sound/coin_sound.wav");

    return bg_audio && mouse_click && explosion_sound && game_over_sound &&
           lose_sound && collect_sound && shoot_sound && loseheart_sound &&
           health_sound && coin_sound;
}

void SoundManager::playSound(Mix_Chunk* sound, int channel, int loops) {
    if (sound) {
        Mix_PlayChannel(channel, sound, loops);
    }
}

void SoundManager::stopSound(int channel) {
    Mix_HaltChannel(channel);
}

void SoundManager::setVolume(int volume) {
    Mix_Volume(-1, volume); // Set volume for all channels
}

void SoundManager::cleanup() {
    // Cleanup sounds
    for (auto& pair : soundMap) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
            pair.second = nullptr;
        }
    }
    soundMap.clear();

    // Cleanup music
    for (auto& pair : musicMap) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
            pair.second = nullptr;
        }
    }
    musicMap.clear();

    // Cleanup game sounds
    bg_audio = nullptr;
    mouse_click = nullptr;
    explosion_sound = nullptr;
    game_over_sound = nullptr;
    lose_sound = nullptr;
    collect_sound = nullptr;
    shoot_sound = nullptr;
    loseheart_sound = nullptr;
    health_sound = nullptr;
    coin_sound = nullptr;

    Mix_CloseAudio();
    Mix_Quit();
}
