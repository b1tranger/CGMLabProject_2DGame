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
    void setBgmVolumePercent(int percent);
    void adjustBgmVolume(int deltaPercent);
    int getBgmVolumePercent() const { return isBgmMuted ? 0 : bgmVolumePercent; }
    int getSavedBgmVolumePercent() const { return bgmVolumePercent; }
    bool isMuted() const { return isBgmMuted; }

    float stepCooldown;
    float climbCooldown;
    float slideCooldown;

private:
    bool isBgmMuted = false;
    int bgmVolumePercent = 30;
    void applyBgmVolume();
    std::vector<uint8_t> sfxBuffers[SFX_COUNT];
    std::vector<uint8_t> buildWav(const std::vector<int16_t>& samples, int sampleRate = 22050);
};

extern AudioManager audioMgr;
