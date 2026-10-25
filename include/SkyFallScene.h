#pragma once

#include "Common.h"
#include <vector>
#include <glm/glm.hpp>

struct SkyBirdObstacle {
    glm::vec2 pos;
    float vx;
    float animTimer;
    bool active;
};

struct SkySpikeTrap {
    glm::vec2 pos;
    glm::vec2 size;
    bool active;
};

struct SkyItem {
    glm::vec2 pos;
    int type; // 0=coin, 1=shield, 2=direction relic
    int fruitType;
    bool collected;
};

struct CelestialStar {
    float x;
    float y;
    float size;
    float twinkleFreq;
    float twinklePhase;
};

class SkyFallScene {
public:
    SkyFallScene();

    void initStars();
    void enter(const glm::vec2& playerPos, RunDirection lastDir);
    void update(float deltaTime, GLFWwindow* window,
                float& shieldTimer, float& postHitInvulnTimer, bool isDashing,
                int& coinsCollected, int& relicsTriggered, float& scoreDistance,
                bool& requestExitToRunner, RunDirection& exitDir);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec2& cameraPos,
                float animTimer, float daylightFactor, float twilightFactor,
                unsigned int birdTex, unsigned int tilesetTex, unsigned int coinTex,
                unsigned int shieldTex, unsigned int arrowTex, unsigned int sceneryTex,
                float shieldTimerVal, const glm::vec2& playerRenderSize);

    const glm::vec2& getPlayerPos() const { return pos; }
    float getVelX() const { return velX; }
    void applyDash(float steer) { velX = steer; }
    float getTrialTime() const { return scenePlayTime; }

    const glm::vec2& getSavedPlatformerPos() const { return platformerSavedPos; }
    RunDirection getSavedPlatformerDir() const { return platformerSavedDir; }

private:
    glm::vec2 pos{ 0.0f, 0.0f };
    float velX               = 0.0f;
    float fallSpeed          = 9.2f;
    bool isHit               = false;
    float hitTimer           = 0.0f;
    float darkAlpha          = 0.0f;
    float genBottomY         = -8.0f;
    float scenePlayTime      = 0.0f;
    bool directionUnlockedNotified = false;

    glm::vec2 platformerSavedPos{ 0.0f, 0.0f };
    RunDirection platformerSavedDir = DIR_RIGHT;

    std::vector<SkyBirdObstacle> birds;
    std::vector<SkySpikeTrap> spikes;
    std::vector<SkyItem> items;
    std::vector<CelestialStar> stars;
};

extern SkyFallScene gameSkyFallScene;
