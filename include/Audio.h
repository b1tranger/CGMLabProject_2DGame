#pragma once

#include "Common.h"
#include <vector>
#include <cstdint>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void init();
    void play(SFXType type);
    void updateCooldowns(float deltaTime);

    // Boss & transformation sequenced audio
    void playSlimeDeathSplat();
    void playAcidBubbling();
    void playMonsterRoar();
    void playSmashLandingBang();
    void stopTransformationAudio();

    float stepCooldown;
    float climbCooldown;
    float slideCooldown;

private:
    std::vector<uint8_t> sfxBuffers[SFX_COUNT];
    std::vector<uint8_t> buildWav(const std::vector<int16_t>& samples, int sampleRate = 22050);
};

extern AudioManager audioMgr;
