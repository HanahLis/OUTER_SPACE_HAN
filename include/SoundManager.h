#pragma once
#include <SDL_mixer.h>
#include <string>
#include <map>
using namespace std;

class SoundManager {
public:
    static SoundManager& getInstance();
    bool init();
    void cleanup();

    // Sound loading
    Mix_Chunk* loadSound(const string& path);
    Mix_Music* loadMusic(const string& path);

    // Game specific sound loaders
    bool loadGameSounds();

    // Sound getters
    Mix_Chunk* getBgAudio() { return bg_audio; }
    Mix_Chunk* getMouseClick() { return mouse_click; }
    Mix_Chunk* getExplosionSound() { return explosion_sound; }
    Mix_Chunk* getGameOverSound() { return game_over_sound; }
    Mix_Chunk* getLoseSound() { return lose_sound; }
    Mix_Chunk* getCollectSound() { return collect_sound; }
    Mix_Chunk* getShootSound() { return shoot_sound; }
    Mix_Chunk* getLoseHeartSound() { return loseheart_sound; }
    Mix_Chunk* getHealthSound() { return health_sound; }
    Mix_Chunk* getCoinSound() { return coin_sound; }

    // Sound control
    void playSound(Mix_Chunk* sound, int channel = -1, int loops = 0);
    void stopSound(int channel = -1);
    void setVolume(int volume); // 0 to 128

private:
    SoundManager() {}
    ~SoundManager() {}
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Game sounds
    Mix_Chunk* bg_audio = nullptr;
    Mix_Chunk* mouse_click = nullptr;
    Mix_Chunk* explosion_sound = nullptr;
    Mix_Chunk* game_over_sound = nullptr;
    Mix_Chunk* lose_sound = nullptr;
    Mix_Chunk* collect_sound = nullptr;
    Mix_Chunk* shoot_sound = nullptr;
    Mix_Chunk* loseheart_sound = nullptr;
    Mix_Chunk* health_sound = nullptr;
    Mix_Chunk* coin_sound = nullptr;

    map<string, Mix_Chunk*> soundMap;
    map<string, Mix_Music*> musicMap;
};
