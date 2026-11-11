#include "UIManager.h"
#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// Helper: Draws text wrapped at word boundaries within maxWidth (returns line count).
static int drawWrappedText(const std::string &text, float startX, float startY,
                           float charW, float charH, float maxWidth,
                           const glm::vec4 &color, const glm::mat4 &orthoUI,
                           float lineStepY, float hangIndent = 0.0f) {
  std::vector<std::string> words;
  std::string currentWord;
  for (char c : text) {
    if (c == ' ') {
      if (!currentWord.empty()) {
        words.push_back(currentWord);
        currentWord.clear();
      }
    } else {
      currentWord += c;
    }
  }
  if (!currentWord.empty()) {
    words.push_back(currentWord);
  }
  if (words.empty()) {
    return 1;
  }

  std::vector<std::string> lines;
  std::string curLine;
  int maxCharsFirst = std::max(1, static_cast<int>(std::floor(maxWidth / charW)));
  int maxCharsCont =
      std::max(1, static_cast<int>(std::floor((maxWidth - hangIndent) / charW)));

  for (const std::string &w : words) {
    int limit = lines.empty() ? maxCharsFirst : maxCharsCont;
    if (curLine.empty()) {
      curLine = w;
    } else if (static_cast<int>(curLine.length() + 1 + w.length()) <= limit) {
      curLine += ' ';
      curLine += w;
    } else {
      lines.push_back(curLine);
      curLine = w;
    }
  }
  if (!curLine.empty()) {
    lines.push_back(curLine);
  }

  for (size_t i = 0; i < lines.size(); ++i) {
    float x = (i == 0) ? startX : (startX + hangIndent);
    float y = startY - static_cast<float>(i) * lineStepY;
    gameRenderer.drawText(lines[i], x, y, charW, charH, color, orthoUI);
  }
  return static_cast<int>(lines.size());
}

UIManager &UIManager::instance() {
  static UIManager inst;
  return inst;
}

UIManager::UIManager() {
  activeNotice.timer = 0.0f;
  activeNotice.maxTime = 2.0f;
  activeNotice.iconType = 0;
  activeNotice.fruitType = 0;
  pauseSliderLayout.trackLeft = -2.20f;
  pauseSliderLayout.trackRight = 4.60f;
  pauseSliderLayout.trackY = 1.00f;
  pauseSliderLayout.trackHeight = 0.16f;
  pauseSliderLayout.minusBtnX = -2.80f;
  pauseSliderLayout.plusBtnX = 5.20f;
  pauseSliderLayout.btnSize = 0.44f;
}

void UIManager::clear() {
  floatingTexts.clear();
  activeNotice.timer = 0.0f;
}

void UIManager::spawnFloatingText(const std::string &text, const glm::vec2 &pos,
                                  const glm::vec4 &color, float charW,
                                  float charH, float duration,
                                  float riseSpeed) {
  FloatingText ft;
  ft.text = text;
  ft.pos = pos;
  ft.color = color;
  ft.charW = charW;
  ft.charH = charH;
  ft.timer = duration;
  ft.maxTime = duration;
  ft.riseSpeed = riseSpeed;
  floatingTexts.push_back(ft);
}

void UIManager::updateFloatingTexts(float rawDeltaTime) {
  for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
    it->timer -= rawDeltaTime;
    it->pos.y += it->riseSpeed * rawDeltaTime;
    if (it->timer <= 0.0f) {
      it = floatingTexts.erase(it);
    } else {
      ++it;
    }
  }
}

void UIManager::renderFloatingTexts(const glm::mat4 &view,
                                    const glm::mat4 &projection) {
  for (const auto &ft : floatingTexts) {
    float lifeRatio = std::clamp(ft.timer / ft.maxTime, 0.0f, 1.0f);
    float alpha = (lifeRatio < 0.25f) ? (lifeRatio / 0.25f) : 1.0f;
    glm::vec4 textColor = ft.color;
    textColor.a *= alpha;

    // Multiline support for floating messages containing newlines
    size_t start = 0;
    size_t end = ft.text.find('\n');
    float lineY = ft.pos.y;
    while (end != std::string::npos) {
      std::string line = ft.text.substr(start, end - start);
      float lineWidth = static_cast<float>(line.length()) * ft.charW;
      gameRenderer.drawText(line, ft.pos.x - (lineWidth * 0.5f), lineY,
                            ft.charW, ft.charH, textColor, projection, view);
      lineY -= (ft.charH * 1.15f);
      start = end + 1;
      end = ft.text.find('\n', start);
    }
    std::string lastLine = ft.text.substr(start);
    float lineWidth = static_cast<float>(lastLine.length()) * ft.charW;
    gameRenderer.drawText(lastLine, ft.pos.x - (lineWidth * 0.5f), lineY,
                          ft.charW, ft.charH, textColor, projection, view);
  }
}

void UIManager::showCornerNotification(const std::string &tag,
                                       const std::string &title,
                                       const std::string &detail,
                                       const glm::vec4 &accentColor,
                                       int iconType, int fruitType) {
  activeNotice.tag = tag;
  activeNotice.title = title;
  activeNotice.detail = detail;
  activeNotice.accentColor = accentColor;
  activeNotice.timer = 2.4f;
  activeNotice.maxTime = 2.4f;
  activeNotice.iconType = iconType;
  activeNotice.fruitType = fruitType;
}

void UIManager::updateCornerNotification(float dt) {
  if (activeNotice.timer > 0.0f) {
    activeNotice.timer -= dt;
    if (activeNotice.timer < 0.0f)
      activeNotice.timer = 0.0f;
  }
}

void UIManager::renderCornerNotification(float halfW, float halfH,
                                         int frameCount, unsigned int coinTex,
                                         unsigned int shieldTex) {
  if (activeNotice.timer <= 0.0f)
    return;

  float alpha = std::min(activeNotice.timer * 2.0f, 1.0f);
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  float cardW = 6.2f;
  float cardH = 1.45f;
  float cardX = -halfW + (cardW * 0.5f) + 0.6f;
  float cardY = halfH - (cardH * 0.5f) - 0.6f;

  glm::mat4 modelCard = glm::mat4(1.0f);
  modelCard = glm::translate(modelCard, glm::vec3(cardX, cardY, 0.0f));
  modelCard = glm::scale(modelCard, glm::vec3(cardW, cardH, 1.0f));
  gameRenderer.drawQuad(modelCard, identityView, orthoUI,
                        glm::vec4(0.03f, 0.05f, 0.09f, 0.92f * alpha), false,
                        glm::vec4(0), 0);

  glm::mat4 modelStripe = glm::mat4(1.0f);
  modelStripe = glm::translate(
      modelStripe, glm::vec3(cardX - (cardW * 0.5f) + 0.08f, cardY, 0.0f));
  modelStripe = glm::scale(modelStripe, glm::vec3(0.16f, cardH, 1.0f));
  gameRenderer.drawQuad(modelStripe, identityView, orthoUI,
                        glm::vec4(activeNotice.accentColor.r,
                                  activeNotice.accentColor.g,
                                  activeNotice.accentColor.b, alpha),
                        false, glm::vec4(0), 0);

  glm::mat4 modelIcon = glm::mat4(1.0f);
  modelIcon = glm::translate(
      modelIcon, glm::vec3(cardX - (cardW * 0.5f) + 0.6f, cardY, 0.0f));
  modelIcon = glm::scale(modelIcon, glm::vec3(0.9f, 0.9f, 1.0f));

  if (activeNotice.iconType == 0) {
    glm::vec4 coinUV = getCoinUV(frameCount / 4);
    gameRenderer.drawQuad(modelIcon, identityView, orthoUI,
                          glm::vec4(1.0f, 1.0f, 1.0f, alpha), (coinTex != 0),
                          coinUV, coinTex);
  } else {
    glm::vec4 shieldUV = getShieldUV(activeNotice.fruitType);
    gameRenderer.drawQuad(modelIcon, identityView, orthoUI,
                          glm::vec4(1.0f, 1.0f, 1.0f, alpha), (shieldTex != 0),
                          shieldUV, shieldTex);
  }

  glm::mat4 modelPill = glm::mat4(1.0f);
  modelPill =
      glm::translate(modelPill, glm::vec3(cardX + 0.55f, cardY + 0.32f, 0.0f));
  modelPill = glm::scale(modelPill, glm::vec3(4.3f, 0.36f, 1.0f));
  gameRenderer.drawQuad(modelPill, identityView, orthoUI,
                        glm::vec4(activeNotice.accentColor.r,
                                  activeNotice.accentColor.g,
                                  activeNotice.accentColor.b, 0.35f * alpha),
                        false, glm::vec4(0), 0);

  float textLeft = cardX - (cardW * 0.5f) + 1.25f;
  gameRenderer.drawText(activeNotice.tag, textLeft, cardY + 0.22f, 0.16f, 0.24f,
                        glm::vec4(1.0f, 1.0f, 1.0f, 0.95f * alpha), orthoUI);
  gameRenderer.drawText(
      activeNotice.title, textLeft, cardY - 0.08f, 0.20f, 0.30f,
      glm::vec4(activeNotice.accentColor.r, activeNotice.accentColor.g,
                activeNotice.accentColor.b, alpha),
      orthoUI);
  gameRenderer.drawText(activeNotice.detail, textLeft, cardY - 0.42f, 0.14f,
                        0.22f, glm::vec4(0.85f, 0.90f, 0.98f, 0.85f * alpha),
                        orthoUI);
}

void UIManager::renderScoreHUD(float halfW, float halfH, int totalScore,
                               int coins, float distance, float shieldTimer,
                               int playerLives, int frameCount,
                               unsigned int coinTex, unsigned int heartTex,
                               bool isBlocking, float blockCooldown) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  float scoreCardW = 7.4f;
  float scoreCardH = 1.45f;
  float scoreCardX = halfW - (scoreCardW * 0.5f) - 0.5f;
  float scoreCardY = halfH - (scoreCardH * 0.5f) - 0.6f;

  glm::mat4 modelScoreCard = glm::mat4(1.0f);
  modelScoreCard =
      glm::translate(modelScoreCard, glm::vec3(scoreCardX, scoreCardY, 0.0f));
  modelScoreCard =
      glm::scale(modelScoreCard, glm::vec3(scoreCardW, scoreCardH, 1.0f));
  gameRenderer.drawQuad(modelScoreCard, identityView, orthoUI,
                        glm::vec4(0.03f, 0.05f, 0.09f, 0.92f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelScoreStripe = glm::mat4(1.0f);
  modelScoreStripe = glm::translate(
      modelScoreStripe,
      glm::vec3(scoreCardX + (scoreCardW * 0.5f) - 0.08f, scoreCardY, 0.0f));
  modelScoreStripe =
      glm::scale(modelScoreStripe, glm::vec3(0.16f, scoreCardH, 1.0f));
  gameRenderer.drawQuad(modelScoreStripe, identityView, orthoUI,
                        glm::vec4(1.0f, 0.82f, 0.20f, 1.0f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelCoinHUD = glm::mat4(1.0f);
  modelCoinHUD = glm::translate(
      modelCoinHUD,
      glm::vec3(scoreCardX - (scoreCardW * 0.5f) + 0.6f, scoreCardY, 0.0f));
  modelCoinHUD = glm::scale(modelCoinHUD, glm::vec3(0.9f, 0.9f, 1.0f));
  glm::vec4 coinHUDUV = getCoinUV(frameCount / 4);
  gameRenderer.drawQuad(modelCoinHUD, identityView, orthoUI, glm::vec4(1.0f),
                        (coinTex != 0), coinHUDUV, coinTex);

  // 3 Ruby Heart icons for Player Lives
  for (int i = 0; i < 3; ++i) {
    float hx = scoreCardX + (scoreCardW * 0.5f) - 1.9f + (i * 0.55f);
    float hy = scoreCardY + 0.22f;
    glm::mat4 modelHeart = glm::mat4(1.0f);
    modelHeart = glm::translate(modelHeart, glm::vec3(hx, hy, 0.0f));
    modelHeart = glm::scale(modelHeart, glm::vec3(0.52f, 0.52f, 1.0f));
    bool hasLife = (playerLives > i);
    glm::vec4 heartCol = hasLife ? glm::vec4(1.0f, 0.25f, 0.35f, 1.0f)
                                 : glm::vec4(0.35f, 0.38f, 0.45f, 0.45f);
    glm::vec4 heartUV = getHeartUV((frameCount / 6) % 4);
    gameRenderer.drawQuad(modelHeart, identityView, orthoUI, heartCol,
                          (heartTex != 0), heartUV, heartTex);
  }

  std::string scoreStr = "SCORE " + std::to_string(totalScore);
  std::string statsStr = "COIN " + std::to_string(coins) + "  DIST " +
                         std::to_string(static_cast<int>(distance)) + "m";
  if (shieldTimer > 0.0f) {
    statsStr += "  SHIELD " +
                std::to_string(static_cast<int>(shieldTimer + 0.9f)) + "s";
  }
  if (isBlocking) {
    statsStr += "  [GUARD: ACTIVE]";
  } else if (blockCooldown > 0.0f) {
    char cdBuf[24];
    snprintf(cdBuf, sizeof(cdBuf), "  [DEF: %.1fs]", blockCooldown);
    statsStr += cdBuf;
  }
  gameRenderer.drawText(scoreStr, scoreCardX - (scoreCardW * 0.5f) + 1.25f,
                        scoreCardY + 0.16f, 0.26f, 0.38f,
                        glm::vec4(1.0f, 0.86f, 0.22f, 1.0f), orthoUI);
  glm::vec4 statsColor =
      isBlocking               ? glm::vec4(0.35f, 1.0f, 0.95f, 1.0f)
      : (blockCooldown > 0.0f) ? glm::vec4(1.0f, 0.85f, 0.35f, 1.0f)
      : (shieldTimer > 0.0f)   ? glm::vec4(0.3f, 0.95f, 1.0f, 1.0f)
                               : glm::vec4(0.85f, 0.94f, 1.0f, 0.95f);
  gameRenderer.drawText(statsStr, scoreCardX - (scoreCardW * 0.5f) + 1.25f,
                        scoreCardY - 0.38f, 0.18f, 0.27f, statsColor, orthoUI);
}

void UIManager::renderEnemyHealthBars(const glm::mat4 &view,
                                      const glm::mat4 &projection,
                                      const std::vector<EnemyEntity> &enemies,
                                      float animTimer) {
  (void)animTimer;
  for (const auto &enemy : enemies) {
    if (!enemy.active || enemy.state == ENEMY_STATE_DEATH)
      continue;

    bool isBoss = (enemy.type == ENEMY_BOSS_DEMON_SLIME);
    float barW = isBoss ? 3.4f : 1.5f;
    float barH = isBoss ? 0.32f : 0.18f;
    float barY = enemy.pos.y + (enemy.size.y * 0.5f) + (isBoss ? 0.55f : 0.32f);
    float barX = enemy.pos.x;

    // Background shadow / border
    glm::mat4 modelBg = glm::mat4(1.0f);
    modelBg = glm::translate(modelBg, glm::vec3(barX, barY, 0.0f));
    modelBg = glm::scale(modelBg, glm::vec3(barW + 0.12f, barH + 0.08f, 1.0f));
    gameRenderer.drawQuad(modelBg, view, projection,
                          glm::vec4(0.04f, 0.04f, 0.08f, 0.92f), false,
                          glm::vec4(0), 0);

    // Health fill
    float hpRatio =
        (enemy.maxHp > 0)
            ? (static_cast<float>(enemy.hp) / static_cast<float>(enemy.maxHp))
            : 0.0f;
    hpRatio = std::max(0.0f, std::min(1.0f, hpRatio));
    float fillW = barW * hpRatio;
    float fillOffset = -(barW * 0.5f) + (fillW * 0.5f);

    glm::vec4 hpCol;
    if (isBoss) {
      hpCol = glm::vec4(0.95f, 0.15f, 0.15f, 1.0f); // Crimson demon boss
    } else if (enemy.type == ENEMY_SLIME_INITIAL) {
      hpCol = glm::vec4(0.20f, 0.88f, 0.35f, 1.0f); // Emerald slime
    } else {
      hpCol = glm::vec4(0.85f, 0.35f, 0.95f, 1.0f); // Violet bringer of death
    }

    if (fillW > 0.01f) {
      glm::mat4 modelFill = glm::mat4(1.0f);
      modelFill =
          glm::translate(modelFill, glm::vec3(barX + fillOffset, barY, 0.0f));
      modelFill = glm::scale(modelFill, glm::vec3(fillW, barH, 1.0f));
      gameRenderer.drawQuad(modelFill, view, projection, hpCol, false,
                            glm::vec4(0), 0);
    }

    // Draw segmented tick marks if maxHp > 1
    if (enemy.maxHp > 1) {
      for (int seg = 1; seg < enemy.maxHp; ++seg) {
        float segX = barX - (barW * 0.5f) + (barW / enemy.maxHp) * seg;
        glm::mat4 modelTick = glm::mat4(1.0f);
        modelTick = glm::translate(modelTick, glm::vec3(segX, barY, 0.0f));
        modelTick = glm::scale(modelTick, glm::vec3(0.04f, barH, 1.0f));
        gameRenderer.drawQuad(modelTick, view, projection,
                              glm::vec4(0.06f, 0.06f, 0.10f, 0.95f), false,
                              glm::vec4(0), 0);
      }
    }
  }
}

void UIManager::renderTrialBadge(float halfW, float halfH, float timeLeft,
                                 float totalTime, float animTimer) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  if (timeLeft > 0.0f) {
    float badgeW = 7.8f;
    float badgeH = 1.15f;
    float badgeY = halfH - 1.15f;

    glm::mat4 modelBadge = glm::mat4(1.0f);
    modelBadge = glm::translate(modelBadge, glm::vec3(0.0f, badgeY, 0.0f));
    modelBadge = glm::scale(modelBadge, glm::vec3(badgeW, badgeH, 1.0f));
    gameRenderer.drawQuad(modelBadge, identityView, orthoUI,
                          glm::vec4(0.03f, 0.06f, 0.12f, 0.88f), false,
                          glm::vec4(0), 0);

    float pFrac = 1.0f - (timeLeft / totalTime);
    float barW = badgeW * pFrac;
    if (barW > 0.05f) {
      glm::mat4 modelPBar = glm::mat4(1.0f);
      modelPBar = glm::translate(
          modelPBar, glm::vec3(-badgeW * 0.5f + barW * 0.5f,
                               badgeY - (badgeH * 0.5f) + 0.05f, 0.0f));
      modelPBar = glm::scale(modelPBar, glm::vec3(barW, 0.09f, 1.0f));
      gameRenderer.drawQuad(modelPBar, identityView, orthoUI,
                            glm::vec4(0.25f, 0.85f, 1.0f, 0.95f), false,
                            glm::vec4(0), 0);
    }

    char buf[64];
    std::snprintf(buf, sizeof(buf), "SURVIVE UNTIL EXIT: %.1fs", timeLeft);
    std::string timerStr = buf;
    float tW = static_cast<float>(timerStr.length()) * 0.22f;
    glm::vec4 textCol = (timeLeft <= 3.0f)
                            ? glm::vec4(1.0f, 0.82f, 0.25f, 1.0f)
                            : glm::vec4(0.35f, 0.95f, 1.0f, 1.0f);
    gameRenderer.drawText(timerStr, -tW * 0.5f, badgeY - 0.12f, 0.22f, 0.32f,
                          textCol, orthoUI);
  } else {
    float bannerW = 10.6f;
    float bannerH = 1.45f;
    float bannerY = halfH - 1.25f;
    float pulse = 0.80f + 0.20f * std::sin(animTimer * 6.0f);

    glm::mat4 modelBanner = glm::mat4(1.0f);
    modelBanner = glm::translate(modelBanner, glm::vec3(0.0f, bannerY, 0.0f));
    modelBanner = glm::scale(modelBanner, glm::vec3(bannerW, bannerH, 1.0f));
    gameRenderer.drawQuad(modelBanner, identityView, orthoUI,
                          glm::vec4(0.02f, 0.14f, 0.08f, 0.92f * pulse), false,
                          glm::vec4(0), 0);

    glm::mat4 modelStripe = glm::mat4(1.0f);
    modelStripe = glm::translate(
        modelStripe, glm::vec3(0.0f, bannerY + (bannerH * 0.5f) - 0.05f, 0.0f));
    modelStripe = glm::scale(modelStripe, glm::vec3(bannerW, 0.10f, 1.0f));
    gameRenderer.drawQuad(modelStripe, identityView, orthoUI,
                          glm::vec4(0.30f, 1.0f, 0.50f, 1.0f), false,
                          glm::vec4(0), 0);

    std::string exTitle = "=== EXIT APPEARED! ===";
    float exW = static_cast<float>(exTitle.length()) * 0.25f;
    gameRenderer.drawText(exTitle, -exW * 0.5f, bannerY + 0.14f, 0.25f, 0.38f,
                          glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), orthoUI);

    std::string exSub = "ENTER AN OPEN EXIT GATE TO RETURN";
    float subW = static_cast<float>(exSub.length()) * 0.18f;
    gameRenderer.drawText(exSub, -subW * 0.5f, bannerY - 0.38f, 0.18f, 0.26f,
                          glm::vec4(1.0f, 0.92f, 0.40f, 1.0f), orthoUI);
  }
}

void UIManager::renderStartHintBanner(float halfW, float halfH,
                                      float animTimer) {
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
  float bannerPulse = 0.82f + 0.18f * std::sin(animTimer * 5.0f);
  gameRenderer.drawText("PRESS [A] OR [D] TO RUN  |  [SPACE] TO JUMP", -6.4f,
                        -2.6f, 0.24f, 0.34f,
                        glm::vec4(1.0f, 0.9f, 0.25f, bannerPulse), orthoUI);
}

VolumeSliderLayout UIManager::getPauseVolumeSliderLayout() {
  return instance().pauseSliderLayout;
}

void UIManager::renderPauseMenu(float halfW, float halfH, int totalScore,
                                int coins, int relics, float distance,
                                bool freeExploration, int activeScenery,
                                bool isNight, float timeLeftInPhase,
                                bool autoShiftEnabled, int bgmVolumePercent,
                                bool bgmMuted) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  glm::mat4 modelOverlay = glm::mat4(1.0f);
  modelOverlay =
      glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelOverlay, identityView, orthoUI,
                        glm::vec4(0.02f, 0.03f, 0.06f, 0.78f), false,
                        glm::vec4(0), 0);

  float menuW = 13.2f;
  float menuH = 9.8f;
  glm::mat4 modelMenu = glm::mat4(1.0f);
  modelMenu = glm::scale(modelMenu, glm::vec3(menuW, menuH, 1.0f));
  gameRenderer.drawQuad(modelMenu, identityView, orthoUI,
                        glm::vec4(0.05f, 0.07f, 0.12f, 0.96f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelHeaderStripe = glm::mat4(1.0f);
  modelHeaderStripe = glm::translate(
      modelHeaderStripe, glm::vec3(0.0f, menuH * 0.5f - 0.1f, 0.0f));
  modelHeaderStripe =
      glm::scale(modelHeaderStripe, glm::vec3(menuW, 0.2f, 1.0f));
  gameRenderer.drawQuad(modelHeaderStripe, identityView, orthoUI,
                        glm::vec4(1.0f, 0.82f, 0.20f, 1.0f), false,
                        glm::vec4(0), 0);

  float textX = -menuW * 0.5f + 0.9f;
  float maxTextW = menuW - 1.8f;
  float curY = menuH * 0.5f - 0.82f;

  gameRenderer.drawText("=== GAME PAUSED ===", textX + 2.0f, curY, 0.34f, 0.48f,
                        glm::vec4(1.0f, 0.85f, 0.25f, 1.0f), orthoUI);
  curY -= 0.70f;

  int lines = drawWrappedText("HERO: SAMURAI MACK [KATANA MASTER]", textX, curY,
                              0.22f, 0.32f, maxTextW,
                              glm::vec4(0.35f, 0.95f, 1.0f, 1.0f), orthoUI, 0.36f);
  curY -= 0.42f + (lines - 1) * 0.36f;

  lines = drawWrappedText("TOTAL SCORE: " + std::to_string(totalScore) + " PTS",
                          textX, curY, 0.23f, 0.34f, maxTextW,
                          glm::vec4(1.0f, 0.9f, 0.35f, 1.0f), orthoUI, 0.36f);
  curY -= 0.40f + (lines - 1) * 0.36f;

  lines = drawWrappedText(
      "DISTANCE: " + std::to_string(static_cast<int>(distance)) +
          "m   COINS: " + std::to_string(coins) +
          "   RELICS: " + std::to_string(relics),
      textX, curY, 0.19f, 0.28f, maxTextW,
      glm::vec4(0.85f, 0.92f, 0.98f, 0.95f), orthoUI, 0.34f);
  curY -= 0.46f + (lines - 1) * 0.34f;

  glm::mat4 modelDiv = glm::mat4(1.0f);
  modelDiv = glm::translate(modelDiv, glm::vec3(0.0f, curY + 0.12f, 0.0f));
  modelDiv = glm::scale(modelDiv, glm::vec3(menuW - 1.8f, 0.05f, 1.0f));
  gameRenderer.drawQuad(modelDiv, identityView, orthoUI,
                        glm::vec4(0.3f, 0.45f, 0.6f, 0.6f), false, glm::vec4(0),
                        0);
  curY -= 0.36f;

  lines = drawWrappedText("[ESC / SPACE]   RESUME GAME", textX, curY, 0.22f,
                          0.30f, maxTextW, glm::vec4(0.35f, 1.0f, 0.55f, 1.0f),
                          orthoUI, 0.34f, 16.0f * 0.22f);
  curY -= 0.42f + (lines - 1) * 0.34f;

  std::string modeMenuStr = freeExploration
                                ? "FREE EXPLORATION (DEFAULT) [MANUAL A/D]"
                                : "AUTO-RUNNER [CONTINUOUS SPRINT]";
  glm::vec4 modeMenuCol = freeExploration ? glm::vec4(0.35f, 1.0f, 0.55f, 1.0f)
                                          : glm::vec4(0.25f, 0.85f, 1.0f, 1.0f);
  lines = drawWrappedText("[F]             MODE: " + modeMenuStr, textX, curY,
                          0.20f, 0.28f, maxTextW, modeMenuCol, orthoUI, 0.34f,
                          16.0f * 0.20f);
  curY -= 0.42f + (lines - 1) * 0.34f;

  // BGM Music Volume Slider (10% increments, default 30%)
  int displayVol = bgmMuted ? 0 : std::clamp(bgmVolumePercent, 0, 100);
  std::string musicStr =
      bgmMuted ? ("MUTED (0%)  [M: Unmute / Drag / <- ->]")
               : (std::to_string(displayVol) + "%  [M: Mute / Drag / <- ->]");
  glm::vec4 musicCol = bgmMuted ? glm::vec4(1.0f, 0.45f, 0.45f, 1.0f)
                                : glm::vec4(0.40f, 0.85f, 1.0f, 1.0f);
  lines = drawWrappedText("[M]             BGM VOL: " + musicStr, textX, curY,
                          0.20f, 0.28f, maxTextW, musicCol, orthoUI, 0.34f,
                          16.0f * 0.20f);

  // Place interactive volume slider cleanly below the last line of BGM VOL text
  pauseSliderLayout.trackLeft = -2.20f;
  pauseSliderLayout.trackRight = 4.60f;
  pauseSliderLayout.trackY = curY - (lines - 1) * 0.34f - 0.35f;
  pauseSliderLayout.trackHeight = 0.16f;
  pauseSliderLayout.minusBtnX = -2.80f;
  pauseSliderLayout.plusBtnX = 5.20f;
  pauseSliderLayout.btnSize = 0.44f;

  VolumeSliderLayout slider = pauseSliderLayout;
  float trackW = slider.trackRight - slider.trackLeft;
  float trackCenterX = (slider.trackLeft + slider.trackRight) * 0.5f;
  float frac = static_cast<float>(displayVol) / 100.0f;
  float knobX = slider.trackLeft + frac * trackW;

  // [-] step button
  glm::mat4 modelMinusBtn = glm::mat4(1.0f);
  modelMinusBtn = glm::translate(
      modelMinusBtn, glm::vec3(slider.minusBtnX, slider.trackY, 0.0f));
  modelMinusBtn = glm::scale(
      modelMinusBtn, glm::vec3(slider.btnSize, slider.btnSize, 1.0f));
  gameRenderer.drawQuad(modelMinusBtn, identityView, orthoUI,
                        glm::vec4(0.14f, 0.20f, 0.30f, 0.95f), false,
                        glm::vec4(0), 0);
  gameRenderer.drawText("-", slider.minusBtnX - 0.09f, slider.trackY - 0.13f,
                        0.20f, 0.26f, glm::vec4(0.85f, 0.94f, 1.0f, 1.0f),
                        orthoUI);

  // [+] step button
  glm::mat4 modelPlusBtn = glm::mat4(1.0f);
  modelPlusBtn = glm::translate(
      modelPlusBtn, glm::vec3(slider.plusBtnX, slider.trackY, 0.0f));
  modelPlusBtn = glm::scale(
      modelPlusBtn, glm::vec3(slider.btnSize, slider.btnSize, 1.0f));
  gameRenderer.drawQuad(modelPlusBtn, identityView, orthoUI,
                        glm::vec4(0.14f, 0.20f, 0.30f, 0.95f), false,
                        glm::vec4(0), 0);
  gameRenderer.drawText("+", slider.plusBtnX - 0.09f, slider.trackY - 0.13f,
                        0.20f, 0.26f, glm::vec4(0.85f, 0.94f, 1.0f, 1.0f),
                        orthoUI);

  // Slider background track
  glm::mat4 modelTrackBg = glm::mat4(1.0f);
  modelTrackBg = glm::translate(
      modelTrackBg, glm::vec3(trackCenterX, slider.trackY, 0.0f));
  modelTrackBg =
      glm::scale(modelTrackBg, glm::vec3(trackW, slider.trackHeight, 1.0f));
  gameRenderer.drawQuad(modelTrackBg, identityView, orthoUI,
                        glm::vec4(0.14f, 0.19f, 0.28f, 1.0f), false,
                        glm::vec4(0), 0);

  // Slider filled bar
  float fillW = trackW * frac;
  if (fillW > 0.01f) {
    glm::mat4 modelFill = glm::mat4(1.0f);
    modelFill = glm::translate(
        modelFill,
        glm::vec3(slider.trackLeft + fillW * 0.5f, slider.trackY, 0.0f));
    modelFill =
        glm::scale(modelFill, glm::vec3(fillW, slider.trackHeight, 1.0f));
    glm::vec4 fillCol = bgmMuted ? glm::vec4(0.85f, 0.35f, 0.35f, 1.0f)
                                 : glm::vec4(0.25f, 0.85f, 1.0f, 1.0f);
    gameRenderer.drawQuad(modelFill, identityView, orthoUI, fillCol, false,
                          glm::vec4(0), 0);
  }

  // 10% step tick notches (0%, 10%, ..., 100%)
  for (int step = 0; step <= 10; ++step) {
    float tickX = slider.trackLeft + (static_cast<float>(step) / 10.0f) * trackW;
    glm::mat4 modelTick = glm::mat4(1.0f);
    modelTick = glm::translate(modelTick, glm::vec3(tickX, slider.trackY, 0.0f));
    modelTick = glm::scale(modelTick, glm::vec3(0.035f, 0.28f, 1.0f));
    glm::vec4 tickCol = (step * 10 <= displayVol)
                            ? glm::vec4(0.75f, 0.96f, 1.0f, 0.9f)
                            : glm::vec4(0.40f, 0.50f, 0.65f, 0.7f);
    gameRenderer.drawQuad(modelTick, identityView, orthoUI, tickCol, false,
                          glm::vec4(0), 0);
  }

  // Draggable slider knob
  glm::mat4 modelKnob = glm::mat4(1.0f);
  modelKnob = glm::translate(modelKnob, glm::vec3(knobX, slider.trackY, 0.0f));
  modelKnob = glm::scale(modelKnob, glm::vec3(0.30f, 0.44f, 1.0f));
  glm::vec4 knobCol = bgmMuted ? glm::vec4(1.0f, 0.45f, 0.45f, 1.0f)
                               : glm::vec4(1.0f, 0.88f, 0.25f, 1.0f);
  gameRenderer.drawQuad(modelKnob, identityView, orthoUI, knobCol, false,
                        glm::vec4(0), 0);

  curY = slider.trackY - 0.62f;

  std::string shiftStr =
      autoShiftEnabled ? "AUTO TIMER (6.5s)" : "MANUAL (RELICS ONLY)";
  glm::vec4 shiftCol = autoShiftEnabled ? glm::vec4(1.0f, 0.65f, 0.25f, 1.0f)
                                        : glm::vec4(0.4f, 0.95f, 0.5f, 1.0f);
  lines = drawWrappedText("[Y]             SHIFT MODE: " + shiftStr, textX, curY,
                          0.20f, 0.28f, maxTextW, shiftCol, orthoUI, 0.34f,
                          16.0f * 0.20f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  std::string scenStr = (activeScenery == 0)   ? "SILVERGRASS FIELD"
                        : (activeScenery == 1) ? "AUTUMN FOLIAGE"
                        : (activeScenery == 2) ? "GREEN FOREST"
                                               : "URBAN AREA";
  lines = drawWrappedText("[B]             SCENERY: " + scenStr, textX, curY,
                          0.21f, 0.29f, maxTextW,
                          glm::vec4(0.3f, 0.95f, 0.85f, 1.0f), orthoUI, 0.34f,
                          16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  std::string cycleStr = (isNight ? "NIGHT TIME (" : "DAY TIME (") +
                         std::to_string(static_cast<int>(timeLeftInPhase)) +
                         "s left)";
  lines = drawWrappedText("[N]             CYCLE: " + cycleStr, textX, curY,
                          0.21f, 0.29f, maxTextW,
                          isNight ? glm::vec4(0.5f, 0.75f, 1.0f, 1.0f)
                                  : glm::vec4(1.0f, 0.85f, 0.35f, 1.0f),
                          orthoUI, 0.34f, 16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  lines = drawWrappedText("[T]             TUTORIAL / HOW TO PLAY", textX,
                          curY, 0.21f, 0.29f, maxTextW,
                          glm::vec4(0.35f, 1.0f, 0.65f, 1.0f), orthoUI, 0.34f,
                          16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  lines = drawWrappedText("[R]             RESTART RUN", textX, curY, 0.21f,
                          0.29f, maxTextW, glm::vec4(0.95f, 0.85f, 0.3f, 1.0f),
                          orthoUI, 0.34f, 16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  lines = drawWrappedText("[Q]             QUIT TO DESKTOP", textX, curY,
                          0.21f, 0.29f, maxTextW,
                          glm::vec4(1.0f, 0.4f, 0.4f, 1.0f), orthoUI, 0.34f,
                          16.0f * 0.21f);
  curY -= 0.44f + (lines - 1) * 0.34f;

  drawWrappedText(
      "CONTROLS: [A/D] MOVE  [W/SPACE] JUMP/CLIMB  [S] SLIDE  [SHIFT] DASH  "
      "[L-CLICK] ATK  [R-CLICK] BLOCK  [F] MODE  [M] BGM",
      textX, curY, 0.145f, 0.20f, maxTextW,
      glm::vec4(0.6f, 0.7f, 0.85f, 0.85f), orthoUI, 0.26f);
}

void UIManager::renderTutorialModal(float halfW, float halfH,
                                    bool openedFromPause) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  glm::mat4 modelOverlay = glm::mat4(1.0f);
  modelOverlay =
      glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelOverlay, identityView, orthoUI,
                        glm::vec4(0.02f, 0.03f, 0.06f, 0.88f), false,
                        glm::vec4(0), 0);

  float tutW = 15.6f;
  float tutH = 10.4f;
  glm::mat4 modelTut = glm::mat4(1.0f);
  modelTut = glm::scale(modelTut, glm::vec3(tutW, tutH, 1.0f));
  gameRenderer.drawQuad(modelTut, identityView, orthoUI,
                        glm::vec4(0.04f, 0.07f, 0.12f, 0.98f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelHeaderStripe = glm::mat4(1.0f);
  modelHeaderStripe = glm::translate(
      modelHeaderStripe, glm::vec3(0.0f, tutH * 0.5f - 0.10f, 0.0f));
  modelHeaderStripe =
      glm::scale(modelHeaderStripe, glm::vec3(tutW, 0.20f, 1.0f));
  gameRenderer.drawQuad(modelHeaderStripe, identityView, orthoUI,
                        glm::vec4(0.25f, 0.85f, 1.0f, 1.0f), false,
                        glm::vec4(0), 0);

  float textX = -tutW * 0.5f + 0.85f;
  float maxTextW = tutW - 1.70f;
  float curY = tutH * 0.5f - 0.78f;

  gameRenderer.drawText("=== WELCOME TO THE WENDERER ===", textX + 1.8f, curY,
                        0.32f, 0.46f, glm::vec4(1.0f, 0.88f, 0.25f, 1.0f),
                        orthoUI);
  curY -= 0.50f;

  gameRenderer.drawText("--- THE ESSENCE OF WANDERING ---", textX + 2.4f, curY,
                        0.22f, 0.32f, glm::vec4(0.40f, 0.85f, 1.0f, 1.0f),
                        orthoUI);
  curY -= 0.46f;

  int lines = drawWrappedText(
      "Endless runner with traps to avoid, shields to pickup, and bosses to slay.",
      textX, curY, 0.185f, 0.27f, maxTextW, glm::vec4(0.95f, 0.95f, 1.0f, 1.0f),
      orthoUI, 0.31f);
  curY -= 0.35f + (lines - 1) * 0.31f;

  struct TutRow {
    std::string text;
    glm::vec4 color;
  };
  const TutRow rows[] = {
      {"MODES  : Free Exploration is DEFAULT ([A/D] move). Press [F] to toggle Auto-Run!",
       glm::vec4(0.35f, 1.0f, 0.65f, 1.0f)},
      {"MUSIC  : BGM defaults to 30% volume. Slider in [ESC] Pause (10% steps), [M] Mute.",
       glm::vec4(0.40f, 0.85f, 1.0f, 1.0f)},
      {"COMBAT : Press [Left Click] / [J] / [X] to attack with katana.",
       glm::vec4(1.0f, 0.45f, 0.45f, 1.0f)},
      {"DEFEND : Press [Right Click] or [K] to block attacks for 1s (2s cooldown on hit).",
       glm::vec4(0.35f, 1.0f, 0.95f, 1.0f)},
      {"ENEMIES: 1-hit Roamers (Multi-layer Thunder Spells!), 2-hit Slimes, 5-hit Demon Boss!",
       glm::vec4(1.0f, 0.75f, 0.35f, 1.0f)},
      {"LIVES  : 3 Hits absorbed before death. Pick up Hearts to restore lives.",
       glm::vec4(1.0f, 0.35f, 0.65f, 1.0f)},
      {"SHIELDS: Pick up glowing Shield icons to absorb 1 hazard/enemy hit.",
       glm::vec4(0.35f, 0.95f, 1.0f, 1.0f)},
      {"WARPS  : Touch Direction Relics to transition into new dimensions.",
       glm::vec4(0.45f, 1.0f, 0.55f, 1.0f)},
      {"SHIFTS : Press [Y] to toggle Auto-Shift Timer (or rely solely on relics).",
       glm::vec4(1.0f, 0.68f, 0.28f, 1.0f)},
  };

  for (const auto &row : rows) {
    lines = drawWrappedText(row.text, textX, curY, 0.18f, 0.26f, maxTextW,
                            row.color, orthoUI, 0.30f, 9.0f * 0.18f);
    curY -= 0.34f + (lines - 1) * 0.30f;
  }
  curY -= 0.12f;

  lines = drawWrappedText(
      "Check the pause menu for controls, or press 'T' at any time to view this tutorial window again without losing your progress.",
      textX, curY, 0.185f, 0.27f, maxTextW, glm::vec4(0.35f, 1.0f, 0.70f, 1.0f),
      orthoUI, 0.31f);
  curY -= 0.40f + (lines - 1) * 0.31f;

  glm::mat4 modelDiv = glm::mat4(1.0f);
  modelDiv = glm::translate(modelDiv, glm::vec3(0.0f, curY + 0.08f, 0.0f));
  modelDiv = glm::scale(modelDiv, glm::vec3(tutW - 1.7f, 0.04f, 1.0f));
  gameRenderer.drawQuad(modelDiv, identityView, orthoUI,
                        glm::vec4(0.3f, 0.45f, 0.6f, 0.6f), false, glm::vec4(0),
                        0);
  curY -= 0.32f;

  lines = drawWrappedText(
      "CONTROLS: [A/D] MOVE  [W/SPACE] JUMP/CLIMB  [S] SLIDE  [SHIFT] DASH  "
      "[L-CLICK] ATK  [R-CLICK] BLOCK  [F] AUTO-RUN  [M] MUTE",
      textX, curY, 0.15f, 0.21f, maxTextW, glm::vec4(0.7f, 0.8f, 0.95f, 0.9f),
      orthoUI, 0.26f);
  curY -= 0.42f + (lines - 1) * 0.26f;

  std::string promptStr = openedFromPause
                              ? "PRESS [SPACE / ENTER / ESC / T] TO RESUME"
                              : "PRESS [SPACE / ENTER / ESC] TO BEGIN VENTURE";
  float prW = static_cast<float>(promptStr.length()) * 0.23f;
  gameRenderer.drawText(promptStr, -prW * 0.5f, curY, 0.23f, 0.34f,
                        glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), orthoUI);
}

void UIManager::renderGameOver(float halfW, float halfH,
                               const std::string &reason, int totalScore,
                               int coins, float distance) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  glm::mat4 modelOverlay = glm::mat4(1.0f);
  modelOverlay =
      glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelOverlay, identityView, orthoUI,
                        glm::vec4(0.12f, 0.02f, 0.04f, 0.82f), false,
                        glm::vec4(0), 0);

  float overW = 11.5f;
  float overH = 7.0f;
  glm::mat4 modelOver = glm::mat4(1.0f);
  modelOver = glm::scale(modelOver, glm::vec3(overW, overH, 1.0f));
  gameRenderer.drawQuad(modelOver, identityView, orthoUI,
                        glm::vec4(0.08f, 0.03f, 0.05f, 0.96f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelOverStripe = glm::mat4(1.0f);
  modelOverStripe = glm::translate(modelOverStripe,
                                   glm::vec3(0.0f, overH * 0.5f - 0.1f, 0.0f));
  modelOverStripe = glm::scale(modelOverStripe, glm::vec3(overW, 0.2f, 1.0f));
  gameRenderer.drawQuad(modelOverStripe, identityView, orthoUI,
                        glm::vec4(1.0f, 0.25f, 0.3f, 1.0f), false, glm::vec4(0),
                        0);

  float textX = -overW * 0.5f + 1.0f;
  float curY = overH * 0.5f - 0.9f;

  gameRenderer.drawText("=== GAME OVER ===", textX + 2.0f, curY, 0.34f, 0.48f,
                        glm::vec4(1.0f, 0.28f, 0.32f, 1.0f), orthoUI);
  curY -= 0.85f;

  gameRenderer.drawText("CAUSE: " + reason, textX, curY, 0.22f, 0.32f,
                        glm::vec4(1.0f, 0.7f, 0.7f, 1.0f), orthoUI);
  curY -= 0.65f;

  gameRenderer.drawText("FINAL SCORE : " + std::to_string(totalScore) + " PTS",
                        textX, curY, 0.26f, 0.38f,
                        glm::vec4(1.0f, 0.88f, 0.22f, 1.0f), orthoUI);
  curY -= 0.60f;

  gameRenderer.drawText(
      "DISTANCE    : " + std::to_string(static_cast<int>(distance)) + " METERS",
      textX, curY, 0.20f, 0.30f, glm::vec4(0.9f, 0.95f, 1.0f, 1.0f), orthoUI);
  curY -= 0.50f;

  gameRenderer.drawText("GOLD COINS  : " + std::to_string(coins) + " (+ " +
                            std::to_string(coins * 50) + " PTS)",
                        textX, curY, 0.20f, 0.30f,
                        glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), orthoUI);
  curY -= 0.85f;

  gameRenderer.drawText("PRESS [SPACE] OR [R] TO RETRY RUN", textX + 0.8f, curY,
                        0.24f, 0.35f, glm::vec4(0.35f, 1.0f, 0.6f, 1.0f),
                        orthoUI);
}

void UIManager::renderSplashScreen(float halfW, float halfH, float fadeAlpha,
                                   float progress) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  // Lambda helpers for vector lines and quads
  auto drawHLine = [&](float x1, float x2, float y, const glm::vec4 &col,
                       float thickness = 0.025f) {
    float w = x2 - x1;
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3((x1 + x2) * 0.5f, y, 0.0f));
    m = glm::scale(m, glm::vec3(w, thickness, 1.0f));
    gameRenderer.drawQuad(m, identityView, orthoUI, col, false, glm::vec4(0),
                          0);
  };

  auto drawVLine = [&](float x, float y1, float y2, const glm::vec4 &col,
                       float thickness = 0.025f) {
    float h = y2 - y1;
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3(x, (y1 + y2) * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(thickness, h, 1.0f));
    gameRenderer.drawQuad(m, identityView, orthoUI, col, false, glm::vec4(0),
                          0);
  };

  auto drawFilledRect = [&](float x1, float y1, float x2, float y2,
                            const glm::vec4 &col) {
    float w = x2 - x1;
    float h = y2 - y1;
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3((x1 + x2) * 0.5f, (y1 + y2) * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(w, h, 1.0f));
    gameRenderer.drawQuad(m, identityView, orthoUI, col, false, glm::vec4(0),
                          0);
  };

  // 1. Dark Cinematic Studio Backdrop
  glm::mat4 modelBackdrop = glm::mat4(1.0f);
  modelBackdrop =
      glm::scale(modelBackdrop, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelBackdrop, identityView, orthoUI,
                        glm::vec4(0.02f, 0.025f, 0.04f, 1.0f), false,
                        glm::vec4(0), 0);

  // 2. Main Academic Card Dimensions
  float cardW = 15.6f;
  float cardH = 8.60f;
  float cardCenterY = 0.35f;

  // Ambient drop shadow
  drawFilledRect(-cardW * 0.5f - 0.12f, cardCenterY - cardH * 0.5f - 0.12f,
                 cardW * 0.5f + 0.12f, cardCenterY + cardH * 0.5f + 0.12f,
                 glm::vec4(0.0f, 0.0f, 0.0f, 0.65f * fadeAlpha));

  // Outer cyan accent frame
  drawFilledRect(-cardW * 0.5f - 0.035f, cardCenterY - cardH * 0.5f - 0.035f,
                 cardW * 0.5f + 0.035f, cardCenterY + cardH * 0.5f + 0.035f,
                 glm::vec4(0.20f, 0.65f, 0.95f, 0.85f * fadeAlpha));

  // Card background body
  drawFilledRect(-cardW * 0.5f, cardCenterY - cardH * 0.5f, cardW * 0.5f,
                 cardCenterY + cardH * 0.5f,
                 glm::vec4(0.04f, 0.065f, 0.11f, 0.98f * fadeAlpha));

  // Top cyan header accent stripe
  drawFilledRect(-cardW * 0.5f, cardCenterY + cardH * 0.5f - 0.14f,
                 cardW * 0.5f, cardCenterY + cardH * 0.5f,
                 glm::vec4(0.25f, 0.80f, 1.0f, 0.95f * fadeAlpha));

  // 3. Main Headers
  std::string mainTitle = "Computer Graphics & Multimedia Lab";
  float titleCharW = 0.22f, titleCharH = 0.34f;
  float titleW = static_cast<float>(mainTitle.length()) * titleCharW;
  gameRenderer.drawText(mainTitle, -titleW * 0.5f, 4.05f, titleCharW,
                        titleCharH, glm::vec4(0.35f, 0.90f, 1.0f, fadeAlpha),
                        orthoUI);

  std::string subTitle = "Group Project";
  float subCharW = 0.26f, subCharH = 0.38f;
  float subW = static_cast<float>(subTitle.length()) * subCharW;
  gameRenderer.drawText(subTitle, -subW * 0.5f, 3.55f, subCharW, subCharH,
                        glm::vec4(1.0f, 0.88f, 0.25f, fadeAlpha), orthoUI);

  // Vector underline for "Project Proposal"
  drawHLine(-subW * 0.5f - 0.25f, subW * 0.5f + 0.25f, 3.47f,
            glm::vec4(1.0f, 0.85f, 0.20f, 0.90f * fadeAlpha), 0.035f);

  // 4. Course Metadata Section
  float contentLeftX = -cardW * 0.5f + 0.85f;
  gameRenderer.drawText("Course Title : ", contentLeftX, 2.95f, 0.17f, 0.25f,
                        glm::vec4(0.40f, 0.80f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("Computer Graphics & Multimedia Lab",
                        contentLeftX + 15.0f * 0.17f, 2.95f, 0.17f, 0.25f,
                        glm::vec4(0.95f, 0.96f, 1.0f, fadeAlpha), orthoUI);

  gameRenderer.drawText("Course Code  : ", contentLeftX, 2.62f, 0.17f, 0.25f,
                        glm::vec4(0.40f, 0.80f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("CSE0611328", contentLeftX + 15.0f * 0.17f, 2.62f,
                        0.17f, 0.25f, glm::vec4(1.0f, 0.88f, 0.35f, fadeAlpha),
                        orthoUI);

  // 5. Faculty Section ("Submitted to:")
  gameRenderer.drawText("Submitted to:", contentLeftX, 2.15f, 0.18f, 0.26f,
                        glm::vec4(0.35f, 0.90f, 1.0f, fadeAlpha), orthoUI);
  drawHLine(contentLeftX, contentLeftX + 13.0f * 0.18f, 2.10f,
            glm::vec4(0.35f, 0.90f, 1.0f, 0.85f * fadeAlpha), 0.025f);

  // Faculty Grid Table
  float tableX = contentLeftX;
  float tableW = cardW - 1.70f;
  float col1W = 7.40f;
  float splitX = tableX + col1W;
  float rowH = 0.34f;
  float t1TopY = 1.98f;
  float t1BottomY = t1TopY - rowH * 3.0f;
  glm::vec4 tableBorderCol(0.25f, 0.50f, 0.80f, 0.85f * fadeAlpha);

  // Table 1 Header Fill
  drawFilledRect(tableX, t1TopY - rowH, tableX + tableW, t1TopY,
                 glm::vec4(0.08f, 0.16f, 0.28f, 0.95f * fadeAlpha));

  // Table 1 Grid Borders
  drawHLine(tableX, tableX + tableW, t1TopY, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t1TopY - rowH, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t1TopY - rowH * 2.0f, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t1BottomY, tableBorderCol);
  drawVLine(tableX, t1BottomY, t1TopY, tableBorderCol);
  drawVLine(splitX, t1BottomY, t1TopY, tableBorderCol);
  drawVLine(tableX + tableW, t1BottomY, t1TopY, tableBorderCol);

  // Table 1 Text Contents
  gameRenderer.drawText("Teacher's Name", tableX + 0.25f,
                        t1TopY - rowH * 0.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.88f, 0.30f, fadeAlpha), orthoUI);
  gameRenderer.drawText("Designation", splitX + 0.25f,
                        t1TopY - rowH * 0.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.88f, 0.30f, fadeAlpha), orthoUI);

  gameRenderer.drawText("Engineer Robiul Islam", tableX + 0.25f,
                        t1TopY - rowH * 1.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.92f, 0.95f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("Assistant Professor", splitX + 0.25f,
                        t1TopY - rowH * 1.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.65f, 0.85f, 0.98f, fadeAlpha), orthoUI);

  gameRenderer.drawText("Tasneen Tanvir", tableX + 0.25f,
                        t1TopY - rowH * 2.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.92f, 0.95f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("Lecturer", splitX + 0.25f,
                        t1TopY - rowH * 2.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.65f, 0.85f, 0.98f, fadeAlpha), orthoUI);

  // 6. Student Group Section ("Submitted by:")
  gameRenderer.drawText("Submitted by:", contentLeftX, 0.55f, 0.18f, 0.26f,
                        glm::vec4(0.35f, 0.90f, 1.0f, fadeAlpha), orthoUI);
  drawHLine(contentLeftX, contentLeftX + 13.0f * 0.18f, 0.50f,
            glm::vec4(0.35f, 0.90f, 1.0f, 0.85f * fadeAlpha), 0.025f);

  // Student Group Grid Table
  float t2TopY = 0.38f;
  float t2BottomY = t2TopY - rowH * 4.0f;

  // Table 2 Header Fill
  drawFilledRect(tableX, t2TopY - rowH, tableX + tableW, t2TopY,
                 glm::vec4(0.08f, 0.16f, 0.28f, 0.95f * fadeAlpha));

  // Table 2 Grid Borders
  drawHLine(tableX, tableX + tableW, t2TopY, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t2TopY - rowH, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t2TopY - rowH * 2.0f, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t2TopY - rowH * 3.0f, tableBorderCol);
  drawHLine(tableX, tableX + tableW, t2BottomY, tableBorderCol);
  drawVLine(tableX, t2BottomY, t2TopY, tableBorderCol);
  drawVLine(splitX, t2BottomY, t2TopY, tableBorderCol);
  drawVLine(tableX + tableW, t2BottomY, t2TopY, tableBorderCol);

  // Table 2 Text Contents
  gameRenderer.drawText("Student's Name", tableX + 0.25f,
                        t2TopY - rowH * 0.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.88f, 0.30f, fadeAlpha), orthoUI);
  gameRenderer.drawText("ID", splitX + 0.25f, t2TopY - rowH * 0.5f - 0.10f,
                        0.15f, 0.22f, glm::vec4(1.0f, 0.88f, 0.30f, fadeAlpha),
                        orthoUI);

  gameRenderer.drawText("Gaus Saraf Murady", tableX + 0.25f,
                        t2TopY - rowH * 1.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.92f, 0.95f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("0432410005101088", splitX + 0.25f,
                        t2TopY - rowH * 1.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.85f, 0.35f, fadeAlpha), orthoUI);

  gameRenderer.drawText("Lima Akter", tableX + 0.25f,
                        t2TopY - rowH * 2.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.92f, 0.95f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("0432410005101063", splitX + 0.25f,
                        t2TopY - rowH * 2.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.85f, 0.35f, fadeAlpha), orthoUI);

  gameRenderer.drawText("Fahima Akter", tableX + 0.25f,
                        t2TopY - rowH * 3.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(0.92f, 0.95f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("0432410005101093", splitX + 0.25f,
                        t2TopY - rowH * 3.5f - 0.10f, 0.15f, 0.22f,
                        glm::vec4(1.0f, 0.85f, 0.35f, fadeAlpha), orthoUI);

  // 7. Academic Footer Metadata
  gameRenderer.drawText("Semester   : ", contentLeftX, -1.45f, 0.16f, 0.24f,
                        glm::vec4(0.40f, 0.80f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("Autumn 2026", contentLeftX + 13.0f * 0.16f, -1.45f,
                        0.16f, 0.24f, glm::vec4(0.35f, 0.95f, 0.65f, fadeAlpha),
                        orthoUI);

  gameRenderer.drawText("Batch      : ", contentLeftX, -1.78f, 0.16f, 0.24f,
                        glm::vec4(0.40f, 0.80f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("55", contentLeftX + 13.0f * 0.16f, -1.78f, 0.16f,
                        0.24f, glm::vec4(0.35f, 0.95f, 0.65f, fadeAlpha),
                        orthoUI);

  gameRenderer.drawText("Department : ", contentLeftX, -2.11f, 0.16f, 0.24f,
                        glm::vec4(0.40f, 0.80f, 1.0f, fadeAlpha), orthoUI);
  gameRenderer.drawText("CSE", contentLeftX + 13.0f * 0.16f, -2.11f, 0.16f,
                        0.24f, glm::vec4(0.35f, 0.95f, 0.65f, fadeAlpha),
                        orthoUI);

  // 8. Cinematic Splash Progress Bar at bottom
  float barW = halfW * 1.5f;
  float barH = 0.05f;
  float barY = -halfH + 0.55f;

  // Bar Background
  drawFilledRect(-barW * 0.5f, barY - barH * 0.5f, barW * 0.5f,
                 barY + barH * 0.5f,
                 glm::vec4(0.16f, 0.20f, 0.28f, 0.70f * fadeAlpha));

  // Bar Fill
  float fillW = barW * glm::clamp(progress, 0.0f, 1.0f);
  if (fillW > 0.01f) {
    drawFilledRect(-barW * 0.5f, barY - barH * 0.65f, -barW * 0.5f + fillW,
                   barY + barH * 0.65f,
                   glm::vec4(0.25f, 0.85f, 1.0f, 0.90f * fadeAlpha));
  }

  // 9. Skip Prompt with subtle pulsing animation
  float pulse =
      0.65f + 0.35f * std::sin(static_cast<float>(glfwGetTime()) * 4.0f);
  glm::vec4 promptColor(0.88f, 0.92f, 1.0f, pulse * fadeAlpha);
  std::string skipText = "[SPACE / ESC / ENTER] SKIP";
  float textW = static_cast<float>(skipText.length()) * 0.18f;
  gameRenderer.drawText(skipText, -textW * 0.5f, barY + 0.26f, 0.18f, 0.26f,
                        promptColor, orthoUI);
}
