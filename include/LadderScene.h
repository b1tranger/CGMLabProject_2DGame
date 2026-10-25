#pragma once

#include "Common.h"
#include <vector>
#include <glm/glm.hpp>

struct LadderTrap {
    int lane;
    float y;
    int type; // 0=spikes, 1=sawblade
    bool active;
};

struct LadderItem {
    int lane;
    float y;
    int type; // 0=coin, 1=shield, 2=direction relic
    int fruitType;
    bool collected;
};

class LadderScene {
public:
    static constexpr float TRACK_X[3] = { -3.2f, 0.0f, 3.2f };
    static constexpr float LEAP_TIME   = 0.20f;

    LadderScene();

    void enter(const glm::vec2& playerPos, RunDirection lastDir);
    void update(float deltaTime, GLFWwindow* window,
                float& shieldTimer, float& postHitInvulnTimer, bool isDashing,
                int& coinsCollected, int& relicsTriggered, float& scoreDistance,
                bool& requestExitToRunner, RunDirection& exitDir, bool& requestSkyFall);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec2& cameraPos,
                float animTimer, float daylightFactor, float twilightFactor,
                unsigned int tilesetTex, unsigned int coinTex, unsigned int shieldTex, unsigned int arrowTex,
                unsigned int sceneryTex, unsigned int spikesTex, float shieldTimer, const glm::vec2& playerRenderSize);

    float getPlayerX() const { return playerX; }
    float getPlayerY() const { return playerY; }
    float getTrialTime() const { return scenePlayTime; }
    bool isResurrecting() const { return isResurrectingState; }
    void applyDash(float surge) { playerY += surge; }

    const glm::vec2& getSavedPlatformerPos() const { return platformerSavedPos; }
    RunDirection getSavedPlatformerDir() const { return platformerSavedDir; }

private:
    int currentLane       = 1;
    int targetLane        = 1;
    float playerY         = 0.0f;
    float playerX         = 0.0f;
    float climbSpeed      = 5.4f;

    bool isLeaping        = false;
    float leapProgress    = 1.0f;

    bool isHit            = false;
    float hitVelY         = 0.0f;
    float darkAlpha       = 0.0f;
    bool isResurrectingState = false;
    float resurrectTimer  = 0.0f;

    float genTopY         = 8.0f;
    float scenePlayTime   = 0.0f;
    bool directionUnlockedNotified = false;

    glm::vec2 platformerSavedPos{ 0.0f, 0.0f };
    RunDirection platformerSavedDir = DIR_RIGHT;

    std::vector<LadderTrap> traps;
    std::vector<LadderItem> items;
};

extern LadderScene gameLadderScene;
