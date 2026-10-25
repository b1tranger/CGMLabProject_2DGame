#pragma once

#include "Common.h"
#include <vector>
#include <cstdint>

class AudioManager {
public:
    AudioManager();
    ~AudioManager() = default;

    void init();
    void play(SFXType type);
    void updateCooldowns(float deltaTime);

    float stepCooldown;
    float climbCooldown;
    float slideCooldown;

private:
    std::vector<uint8_t> sfxBuffers[SFX_COUNT];
    std::vector<uint8_t> buildWav(const std::vector<int16_t>& samples, int sampleRate = 22050);
};

extern AudioManager audioMgr;
