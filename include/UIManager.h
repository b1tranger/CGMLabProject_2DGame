#pragma once

#include "Common.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct FloatingText {
    std::string text;
    glm::vec2 pos;
    glm::vec4 color;
    float charW;
    float charH;
    float timer;
    float maxTime;
    float riseSpeed;
};

class UIManager {
public:
    static UIManager& instance();

    void spawnFloatingText(const std::string& text, const glm::vec2& pos, const glm::vec4& color,
                           float charW = 0.26f, float charH = 0.38f, float duration = 2.2f, float riseSpeed = 1.3f);
    void updateFloatingTexts(float rawDeltaTime);
    void renderFloatingTexts(const glm::mat4& view, const glm::mat4& projection);

    void showCornerNotification(const std::string& tag, const std::string& title, const std::string& detail,
                                const glm::vec4& accentColor, int iconType, int fruitType = 0);
    void updateCornerNotification(float dt);
    bool isNoticeActive(const std::string& tag = "") const {
        if (activeNotice.timer <= 0.0f) return false;
        if (!tag.empty()) return activeNotice.tag == tag;
        return true;
    }
    void renderCornerNotification(float halfW, float halfH, int frameCount,
                                 unsigned int coinTex, unsigned int shieldTex);

    void renderScoreHUD(float halfW, float halfH, int totalScore, int coins, float distance,
                        float shieldTimer, int frameCount, unsigned int coinTex);
    void renderTrialBadge(float halfW, float halfH, float timeLeft, float totalTime, float animTimer);
    void renderStartHintBanner(float halfW, float halfH, float animTimer);

    void renderPauseMenu(float halfW, float halfH, int totalScore, int coins, int relics, float distance,
                         bool freeExploration, int activeScenery, bool isNight, float timeLeftInPhase);
    void renderTutorialModal(float halfW, float halfH, bool openedFromPause);
    void renderGameOver(float halfW, float halfH, const std::string& reason,
                        int totalScore, int coins, float distance);

    void clear();

private:
    UIManager();
    ~UIManager() = default;

    std::vector<FloatingText> floatingTexts;
    CornerNotification activeNotice;
};

inline void spawnFloatingText(const std::string& text, const glm::vec2& pos, const glm::vec4& color,
                             float charW = 0.26f, float charH = 0.38f, float duration = 2.2f, float riseSpeed = 1.3f)
{
    UIManager::instance().spawnFloatingText(text, pos, color, charW, charH, duration, riseSpeed);
}

inline void showCornerNotification(const std::string& tag, const std::string& title, const std::string& detail,
                                  const glm::vec4& accentColor, int iconType, int fruitType = 0)
{
    UIManager::instance().showCornerNotification(tag, title, detail, accentColor, iconType, fruitType);
}
