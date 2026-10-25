#include "Audio.h"

#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#include <cstdlib>

AudioManager audioMgr;

AudioManager::AudioManager()
    : stepCooldown(0.0f), climbCooldown(0.0f), slideCooldown(0.0f) {}

std::vector<uint8_t> AudioManager::buildWav(const std::vector<int16_t>& samples, int sampleRate) {
    uint32_t dataSize = static_cast<uint32_t>(samples.size() * sizeof(int16_t));
    uint32_t totalSize = 36 + dataSize;
    std::vector<uint8_t> wav;
    wav.reserve(44 + dataSize);

    auto pushStr = [&](const char* s) { while (*s) wav.push_back((uint8_t)*s++); };
    auto push32 = [&](uint32_t val) {
        wav.push_back(val & 0xFF);
        wav.push_back((val >> 8) & 0xFF);
        wav.push_back((val >> 16) & 0xFF);
        wav.push_back((val >> 24) & 0xFF);
    };
    auto push16 = [&](uint16_t val) {
        wav.push_back(val & 0xFF);
        wav.push_back((val >> 8) & 0xFF);
    };

    pushStr("RIFF");
    push32(totalSize);
    pushStr("WAVE");
    pushStr("fmt ");
    push32(16);
    push16(1);              // PCM
    push16(1);              // Mono
    push32(sampleRate);
    push32(sampleRate * 2); // Byte rate
    push16(2);              // Block align
    push16(16);             // Bits per sample
    pushStr("data");
    push32(dataSize);

    for (int16_t s : samples) {
        push16(static_cast<uint16_t>(s));
    }
    return wav;
}

void AudioManager::init() {
    const int sr = 22050;
    const float PI = 3.1415926535f;

    // 1. SFX_STEP: Subtle crisp footstep thud
    {
        int count = static_cast<int>(sr * 0.045f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / sr;
            float env = 1.0f - ((float)i / count);
            float freq = 130.0f - (t * 900.0f);
            if (freq < 45.0f) freq = 45.0f;
            float s = std::sin(2.0f * PI * freq * t) * env;
            samples[i] = static_cast<int16_t>(s * 10500.0f);
        }
        sfxBuffers[SFX_STEP] = buildWav(samples, sr);
    }

    // 2. SFX_JUMP: Springy upward pitch sweep
    {
        int count = static_cast<int>(sr * 0.15f);
        std::vector<int16_t> samples(count);
        float phase = 0.0f;
        for (int i = 0; i < count; ++i) {
            float t = (float)i / count;
            float freq = 190.0f + (t * 430.0f);
            phase += (2.0f * PI * freq) / sr;
            float env = (1.0f - t);
            samples[i] = static_cast<int16_t>(std::sin(phase) * env * 14500.0f);
        }
        sfxBuffers[SFX_JUMP] = buildWav(samples, sr);
    }

    // 3. SFX_CLIMB: Ladder wooden rung clack
    {
        int count = static_cast<int>(sr * 0.055f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / count;
            float env = (1.0f - t) * (1.0f - t);
            float s = (std::sin(2.0f * PI * 460.0f * ((float)i / sr)) * 0.65f +
                       std::sin(2.0f * PI * 880.0f * ((float)i / sr)) * 0.35f) * env;
            samples[i] = static_cast<int16_t>(s * 13500.0f);
        }
        sfxBuffers[SFX_CLIMB] = buildWav(samples, sr);
    }

    // 4. SFX_SLIDE: Rushing slope friction whoosh
    {
        int count = static_cast<int>(sr * 0.22f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / count;
            float env = std::sin(t * PI);
            float noise = ((float)(std::rand() % 2000) - 1000.0f) / 1000.0f;
            float tone = std::sin(2.0f * PI * (280.0f + t * 240.0f) * ((float)i / sr));
            float s = (noise * 0.65f + tone * 0.35f) * env;
            samples[i] = static_cast<int16_t>(s * 14000.0f);
        }
        sfxBuffers[SFX_SLIDE] = buildWav(samples, sr);
    }

    // 5. SFX_COIN: Bright two-tone gold pickup chime
    {
        int count = static_cast<int>(sr * 0.17f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / sr;
            float freq = (t < 0.065f) ? 987.77f : 1318.51f;
            float env = (t < 0.065f) ? (1.0f - t / 0.065f * 0.3f) : (1.0f - (t - 0.065f) / 0.105f);
            float s = (std::sin(2.0f * PI * freq * t) + 0.25f * std::sin(4.0f * PI * freq * t)) * env;
            samples[i] = static_cast<int16_t>(s * 16000.0f);
        }
        sfxBuffers[SFX_COIN] = buildWav(samples, sr);
    }

    // 6. SFX_RELIC: Mystical 4-note ascending crystal arpeggio
    {
        int count = static_cast<int>(sr * 0.36f);
        std::vector<int16_t> samples(count);
        float notes[] = {523.25f, 659.25f, 783.99f, 1046.50f};
        for (int i = 0; i < count; ++i) {
            float t = (float)i / sr;
            int nIdx = std::min(3, static_cast<int>(t / 0.075f));
            float freq = notes[nIdx];
            float env = 1.0f - ((float)i / count);
            float s = (std::sin(2.0f * PI * freq * t) + 0.35f * std::sin(3.0f * PI * freq * t)) * env;
            samples[i] = static_cast<int16_t>(s * 16500.0f);
        }
        sfxBuffers[SFX_RELIC] = buildWav(samples, sr);
    }

    // 7. SFX_SHIFT: Dimensional portal warp & environment shift whoosh
    {
        int count = static_cast<int>(sr * 0.32f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / count;
            float freq = 580.0f - (std::sin(t * PI) * 360.0f);
            float env = std::sin(t * PI);
            float noise = ((float)(std::rand() % 1000) - 500.0f) / 500.0f;
            float s = (std::sin(2.0f * PI * freq * ((float)i / sr)) * 0.7f + noise * 0.3f) * env;
            samples[i] = static_cast<int16_t>(s * 17000.0f);
        }
        sfxBuffers[SFX_SHIFT] = buildWav(samples, sr);
    }

    // 8. SFX_DEATH: Impalement crunch / Game Over impact
    {
        int count = static_cast<int>(sr * 0.40f);
        std::vector<int16_t> samples(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / count;
            float env = (1.0f - t);
            float noise = ((float)(std::rand() % 2000) - 1000.0f) / 1000.0f;
            float tone = std::sin(2.0f * PI * (150.0f - t * 90.0f) * ((float)i / sr));
            float s = (noise * 0.75f + tone * 0.45f) * env;
            samples[i] = static_cast<int16_t>(s * 19000.0f);
        }
        sfxBuffers[SFX_DEATH] = buildWav(samples, sr);
    }
}

void AudioManager::play(SFXType type) {
    if (type >= 0 && type < SFX_COUNT && !sfxBuffers[type].empty()) {
        PlaySoundA(reinterpret_cast<LPCSTR>(sfxBuffers[type].data()), NULL, SND_MEMORY | SND_ASYNC);
    }
}

void AudioManager::updateCooldowns(float deltaTime) {
    stepCooldown  -= deltaTime;
    climbCooldown -= deltaTime;
    slideCooldown -= deltaTime;
}
