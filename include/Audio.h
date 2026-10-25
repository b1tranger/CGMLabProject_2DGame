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
    void playBossDyingRoar();
    void stopTransformationAudio();

    // Bringer of Death Thunder Spell SFX
    void playThunderSpell();

    // Background Music (Quarter_in_the_Slot.mp3)
    void startBGM();
    void stopBGM();
    void toggleBgmMute();
    bool isMuted() const { return isBgmMuted; }

    float stepCooldown;
    float climbCooldown;
    float slideCooldown;

private:
    bool isBgmMuted = false;
    std::vector<uint8_t> sfxBuffers[SFX_COUNT];
    std::vector<uint8_t> buildWav(const std::vector<int16_t>& samples, int sampleRate = 22050);
};

extern AudioManager audioMgr;
