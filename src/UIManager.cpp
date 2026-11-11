#include "UIManager.h"
#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

// Helper: Draws text wrapped at word boundaries within maxWidth (returns line
// count).
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
  int maxCharsFirst =
      std::max(1, static_cast<int>(std::floor(maxWidth / charW)));
  int maxCharsCont = std::max(
      1, static_cast<int>(std::floor((maxWidth - hangIndent) / charW)));

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
  centerNotice.timer = 0.0f;
  centerNotice.maxTime = 3.0f;
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
  centerNotice.timer = 0.0f;
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

void UIManager::showCenterAnnouncement(const std::string &title,
                                       const std::string &subtitle,
                                       const glm::vec4 &accentColor,
                                       float duration) {
  centerNotice.title = title;
  centerNotice.subtitle = subtitle;
  centerNotice.accentColor = accentColor;
  centerNotice.maxTime = (duration > 0.1f) ? duration : 3.0f;
  centerNotice.timer = centerNotice.maxTime;
}

void UIManager::updateCenterAnnouncement(float dt) {
  if (centerNotice.timer > 0.0f) {
    centerNotice.timer -= dt;
    if (centerNotice.timer < 0.0f) {
      centerNotice.timer = 0.0f;
    }
  }
}

void UIManager::renderCenterAnnouncement(float halfW, float halfH) {
  if (centerNotice.timer <= 0.0f) {
    return;
  }

  float elapsed = centerNotice.maxTime - centerNotice.timer;
  float fadeIn = std::min(1.0f, elapsed / 0.22f);
  float fadeOut = std::min(1.0f, centerNotice.timer / 0.45f);
  float alpha = std::min(fadeIn, fadeOut);
  if (alpha <= 0.01f) {
    return;
  }

  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  float cardW = std::min(halfW * 1.65f, 12.6f);
  float cardH = 1.75f;
  float cardX = 0.0f;
  float cardY = 0.0f;

  // Center backdrop panel
  glm::mat4 modelBg = glm::mat4(1.0f);
  modelBg = glm::translate(modelBg, glm::vec3(cardX, cardY, 0.0f));
  modelBg = glm::scale(modelBg, glm::vec3(cardW, cardH, 1.0f));
  gameRenderer.drawQuad(modelBg, identityView, orthoUI,
                        glm::vec4(0.03f, 0.05f, 0.09f, 0.88f * alpha), false,
                        glm::vec4(0), 0);

  // Top and bottom accent stripes
  glm::mat4 modelTop = glm::mat4(1.0f);
  modelTop = glm::translate(
      modelTop, glm::vec3(cardX, cardY + (cardH * 0.5f) - 0.05f, 0.0f));
  modelTop = glm::scale(modelTop, glm::vec3(cardW, 0.10f, 1.0f));
  gameRenderer.drawQuad(modelTop, identityView, orthoUI,
                        glm::vec4(centerNotice.accentColor.r,
                                  centerNotice.accentColor.g,
                                  centerNotice.accentColor.b, alpha),
                        false, glm::vec4(0), 0);

  glm::mat4 modelBot = glm::mat4(1.0f);
  modelBot = glm::translate(
      modelBot, glm::vec3(cardX, cardY - (cardH * 0.5f) + 0.04f, 0.0f));
  modelBot = glm::scale(modelBot, glm::vec3(cardW, 0.08f, 1.0f));
  gameRenderer.drawQuad(modelBot, identityView, orthoUI,
                        glm::vec4(centerNotice.accentColor.r,
                                  centerNotice.accentColor.g,
                                  centerNotice.accentColor.b, 0.80f * alpha),
                        false, glm::vec4(0), 0);

  float maxTextW = cardW - 0.9f;
  float titleCharW = 0.28f;
  float titleCharH = 0.42f;
  if (!centerNotice.title.empty()) {
    float naturalW =
        static_cast<float>(centerNotice.title.length()) * titleCharW;
    if (naturalW > maxTextW) {
      float scale = maxTextW / naturalW;
      titleCharW *= scale;
      titleCharH *= scale;
    }
    float titleW = static_cast<float>(centerNotice.title.length()) * titleCharW;
    gameRenderer.drawText(centerNotice.title, -titleW * 0.5f, cardY + 0.18f,
                          titleCharW, titleCharH,
                          glm::vec4(centerNotice.accentColor.r,
                                    centerNotice.accentColor.g,
                                    centerNotice.accentColor.b, alpha),
                          orthoUI);
  }

  if (!centerNotice.subtitle.empty()) {
    float subCharW = 0.17f;
    float subCharH = 0.25f;
    float naturalW =
        static_cast<float>(centerNotice.subtitle.length()) * subCharW;
    if (naturalW > maxTextW) {
      float scale = maxTextW / naturalW;
      subCharW *= scale;
      subCharH *= scale;
    }
    float subW = static_cast<float>(centerNotice.subtitle.length()) * subCharW;
    gameRenderer.drawText(
        centerNotice.subtitle, -subW * 0.5f, cardY - 0.40f, subCharW, subCharH,
        glm::vec4(0.92f, 0.95f, 1.0f, 0.95f * alpha), orthoUI);
  }
}

void UIManager::renderScoreHUD(float halfW, float halfH, int totalScore,
                               int coins, float distance, float shieldTimer,
                               int playerLives, int frameCount,
                               unsigned int coinTex, unsigned int heartTex,
                               bool isBlocking, float blockCooldown,
                               int previousScore) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  float scoreCardW = 7.8f;
  float scoreCardH = 1.88f;
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
    float hy = scoreCardY + 0.42f;
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

  float textLeft = scoreCardX - (scoreCardW * 0.5f) + 1.25f;
  std::string scoreStr = "SCORE " + std::to_string(totalScore);
  std::string prevStr = "PREV SCORE " + std::to_string(previousScore);
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
  gameRenderer.drawText(scoreStr, textLeft, scoreCardY + 0.36f, 0.25f, 0.36f,
                        glm::vec4(1.0f, 0.86f, 0.22f, 1.0f), orthoUI);
  gameRenderer.drawText(prevStr, textLeft, scoreCardY - 0.04f, 0.17f, 0.25f,
                        glm::vec4(0.72f, 0.86f, 1.0f, 0.95f), orthoUI);

  bool hasStatusBadges =
      (shieldTimer > 0.0f || isBlocking || blockCooldown > 0.0f);
  float statsCharW = hasStatusBadges ? 0.155f : 0.175f;
  float statsCharH = hasStatusBadges ? 0.24f : 0.26f;
  glm::vec4 statsColor =
      isBlocking               ? glm::vec4(0.35f, 1.0f, 0.95f, 1.0f)
      : (blockCooldown > 0.0f) ? glm::vec4(1.0f, 0.85f, 0.35f, 1.0f)
      : (shieldTimer > 0.0f)   ? glm::vec4(0.3f, 0.95f, 1.0f, 1.0f)
                               : glm::vec4(0.85f, 0.94f, 1.0f, 0.95f);
  gameRenderer.drawText(statsStr, textLeft, scoreCardY - 0.52f, statsCharW,
                        statsCharH, statsColor, orthoUI);
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

  int lines = drawWrappedText(
      "HERO: SAMURAI MACK [KATANA MASTER]", textX, curY, 0.22f, 0.32f, maxTextW,
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
  modelMinusBtn = glm::scale(modelMinusBtn,
                             glm::vec3(slider.btnSize, slider.btnSize, 1.0f));
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
  modelPlusBtn =
      glm::scale(modelPlusBtn, glm::vec3(slider.btnSize, slider.btnSize, 1.0f));
  gameRenderer.drawQuad(modelPlusBtn, identityView, orthoUI,
                        glm::vec4(0.14f, 0.20f, 0.30f, 0.95f), false,
                        glm::vec4(0), 0);
  gameRenderer.drawText("+", slider.plusBtnX - 0.09f, slider.trackY - 0.13f,
                        0.20f, 0.26f, glm::vec4(0.85f, 0.94f, 1.0f, 1.0f),
                        orthoUI);

  // Slider background track
  glm::mat4 modelTrackBg = glm::mat4(1.0f);
  modelTrackBg = glm::translate(modelTrackBg,
                                glm::vec3(trackCenterX, slider.trackY, 0.0f));
  modelTrackBg =
      glm::scale(modelTrackBg, glm::vec3(trackW, slider.trackHeight, 1.0f));
  gameRenderer.drawQuad(modelTrackBg, identityView, orthoUI,
                        glm::vec4(0.14f, 0.19f, 0.28f, 1.0f), false,
                        glm::vec4(0), 0);

  // Slider filled bar
  float fillW = trackW * frac;
  if (fillW > 0.01f) {
    glm::mat4 modelFill = glm::mat4(1.0f);
    modelFill =
        glm::translate(modelFill, glm::vec3(slider.trackLeft + fillW * 0.5f,
                                            slider.trackY, 0.0f));
    modelFill =
        glm::scale(modelFill, glm::vec3(fillW, slider.trackHeight, 1.0f));
    glm::vec4 fillCol = bgmMuted ? glm::vec4(0.85f, 0.35f, 0.35f, 1.0f)
                                 : glm::vec4(0.25f, 0.85f, 1.0f, 1.0f);
    gameRenderer.drawQuad(modelFill, identityView, orthoUI, fillCol, false,
                          glm::vec4(0), 0);
  }

  // 10% step tick notches (0%, 10%, ..., 100%)
  for (int step = 0; step <= 10; ++step) {
    float tickX =
        slider.trackLeft + (static_cast<float>(step) / 10.0f) * trackW;
    glm::mat4 modelTick = glm::mat4(1.0f);
    modelTick =
        glm::translate(modelTick, glm::vec3(tickX, slider.trackY, 0.0f));
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
  lines = drawWrappedText("[Y]             SHIFT MODE: " + shiftStr, textX,
                          curY, 0.20f, 0.28f, maxTextW, shiftCol, orthoUI,
                          0.34f, 16.0f * 0.20f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  std::string scenStr = (activeScenery == 0)   ? "SILVERGRASS FIELD"
                        : (activeScenery == 1) ? "AUTUMN FOLIAGE"
                        : (activeScenery == 2) ? "GREEN FOREST"
                                               : "URBAN AREA";
  lines =
      drawWrappedText("[B]             SCENERY: " + scenStr, textX, curY, 0.21f,
                      0.29f, maxTextW, glm::vec4(0.3f, 0.95f, 0.85f, 1.0f),
                      orthoUI, 0.34f, 16.0f * 0.21f);
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

  lines = drawWrappedText("[T]             TUTORIAL / HOW TO PLAY", textX, curY,
                          0.21f, 0.29f, maxTextW,
                          glm::vec4(0.35f, 1.0f, 0.65f, 1.0f), orthoUI, 0.34f,
                          16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  lines = drawWrappedText("[R]             RESTART RUN", textX, curY, 0.21f,
                          0.29f, maxTextW, glm::vec4(0.95f, 0.85f, 0.3f, 1.0f),
                          orthoUI, 0.34f, 16.0f * 0.21f);
  curY -= 0.40f + (lines - 1) * 0.34f;

  lines = drawWrappedText("[Q]             QUIT TO DESKTOP", textX, curY, 0.21f,
                          0.29f, maxTextW, glm::vec4(1.0f, 0.4f, 0.4f, 1.0f),
                          orthoUI, 0.34f, 16.0f * 0.21f);
  curY -= 0.44f + (lines - 1) * 0.34f;

  drawWrappedText(
      "CONTROLS: [A/D] MOVE  [W/SPACE] JUMP/CLIMB  [S] SLIDE  [SHIFT] DASH  "
      "[L-CLICK] ATK  [R-CLICK] BLOCK  [F] MODE  [M] BGM",
      textX, curY, 0.145f, 0.20f, maxTextW, glm::vec4(0.6f, 0.7f, 0.85f, 0.85f),
      orthoUI, 0.26f);
}

void UIManager::renderTutorialModal(float halfW, float halfH,
                                    bool openedFromPause, float animTimer,
                                    const TutorialPreviewTextures &enemyTex) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  glm::mat4 modelOverlay = glm::mat4(1.0f);
  modelOverlay =
      glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelOverlay, identityView, orthoUI,
                        glm::vec4(0.02f, 0.03f, 0.06f, 0.88f), false,
                        glm::vec4(0), 0);

  float tutW = 18.0f;
  float tutH = 12.6f;
  glm::mat4 modelTut = glm::mat4(1.0f);
  modelTut = glm::scale(modelTut, glm::vec3(tutW, tutH, 1.0f));
  gameRenderer.drawQuad(modelTut, identityView, orthoUI,
                        glm::vec4(0.04f, 0.07f, 0.12f, 0.98f), false,
                        glm::vec4(0), 0);

  glm::mat4 modelHeaderStripe = glm::mat4(1.0f);
  modelHeaderStripe = glm::translate(
      modelHeaderStripe, glm::vec3(0.0f, tutH * 0.5f - 0.09f, 0.0f));
  modelHeaderStripe =
      glm::scale(modelHeaderStripe, glm::vec3(tutW, 0.18f, 1.0f));
  gameRenderer.drawQuad(modelHeaderStripe, identityView, orthoUI,
                        glm::vec4(0.25f, 0.85f, 1.0f, 1.0f), false,
                        glm::vec4(0), 0);

  auto drawPanel = [&](float x1, float y1, float x2, float y2,
                       const glm::vec4 &bgCol, const glm::vec4 &borderCol) {
    float cx = (x1 + x2) * 0.5f;
    float cy = (y1 + y2) * 0.5f;
    float w = x2 - x1;
    float h = y2 - y1;
    glm::mat4 mBorder = glm::mat4(1.0f);
    mBorder = glm::translate(mBorder, glm::vec3(cx, cy, 0.0f));
    mBorder = glm::scale(mBorder, glm::vec3(w + 0.05f, h + 0.05f, 1.0f));
    gameRenderer.drawQuad(mBorder, identityView, orthoUI, borderCol, false,
                          glm::vec4(0), 0);

    glm::mat4 mBg = glm::mat4(1.0f);
    mBg = glm::translate(mBg, glm::vec3(cx, cy, 0.0f));
    mBg = glm::scale(mBg, glm::vec3(w, h, 1.0f));
    gameRenderer.drawQuad(mBg, identityView, orthoUI, bgCol, false,
                          glm::vec4(0), 0);
  };

  auto beginCellScissor = [&](float x1, float y1, float x2, float y2,
                              GLboolean &wasEnabled, GLint prevScissor[4]) {
    wasEnabled = glIsEnabled(GL_SCISSOR_TEST);
    glGetIntegerv(GL_SCISSOR_BOX, prevScissor);
    GLint curVp[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, curVp);

    auto toFbX = [&](float ux) -> int {
      float norm = (ux + halfW) / (2.0f * halfW);
      return curVp[0] + static_cast<int>(std::floor(norm * curVp[2]));
    };
    auto toFbY = [&](float uy) -> int {
      float norm = (uy + halfH) / (2.0f * halfH);
      return curVp[1] + static_cast<int>(std::floor(norm * curVp[3]));
    };
    int sx0 = toFbX(x1);
    int sx1 = toFbX(x2);
    int sy0 = toFbY(y1);
    int sy1 = toFbY(y2);
    if (wasEnabled) {
      sx0 = std::max(sx0, prevScissor[0]);
      sy0 = std::max(sy0, prevScissor[1]);
      sx1 = std::min(sx1, prevScissor[0] + prevScissor[2]);
      sy1 = std::min(sy1, prevScissor[1] + prevScissor[3]);
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(sx0, sy0, std::max(0, sx1 - sx0), std::max(0, sy1 - sy0));
  };

  auto endCellScissor = [&](GLboolean wasEnabled, const GLint prevScissor[4]) {
    if (wasEnabled) {
      glScissor(prevScissor[0], prevScissor[1], prevScissor[2], prevScissor[3]);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
  };

  const float innerLeft = -tutW * 0.5f + 0.65f;
  const float innerRight = tutW * 0.5f - 0.65f;
  const float innerW = innerRight - innerLeft;

  // Title & Intro
  std::string mainHeading = "=== WELCOME TO THE WENDERER ===";
  float mhW = static_cast<float>(mainHeading.length()) * 0.27f;
  gameRenderer.drawText(mainHeading, -mhW * 0.5f, tutH * 0.5f - 0.62f, 0.27f,
                        0.38f, glm::vec4(1.0f, 0.88f, 0.25f, 1.0f), orthoUI);

  std::string subHeading = "Endless runner with traps to avoid, shields to "
                           "pickup, and bosses to slay.";
  float shW = static_cast<float>(subHeading.length()) * 0.16f;
  gameRenderer.drawText(subHeading, -shW * 0.5f, tutH * 0.5f - 0.96f, 0.16f,
                        0.23f, glm::vec4(0.68f, 0.88f, 1.0f, 1.0f), orthoUI);

  // =========================================================================
  // 1. PLAYER CONTROLS (6-Cell Animation Grid with Controls Below)
  // =========================================================================
  float pcHeaderY = tutH * 0.5f - 1.32f;
  gameRenderer.drawText("PLAYER CONTROLS", innerLeft, pcHeaderY, 0.19f, 0.26f,
                        glm::vec4(0.35f, 1.0f, 0.65f, 1.0f), orthoUI);

  const int numPlayerCells = 6;
  const float cellGap = 0.22f;
  const float cellW =
      (innerW - cellGap * (numPlayerCells - 1)) / numPlayerCells;
  const float pcTop = pcHeaderY - 0.10f;
  const float pcBottom = pcTop - 1.98f;
  const float animBoxBottom = pcBottom + 0.58f;

  struct PlayerPreviewDef {
    const char *actionTitle;
    const char *controlLine1;
    const char *controlLine2;
    glm::vec4 accent;
  };
  const PlayerPreviewDef playerPreviews[numPlayerCells] = {
      {"RUN", "[A / D]", "MOVE", glm::vec4(0.35f, 1.0f, 0.65f, 1.0f)},
      {"JUMP/CLIMB", "[W / SPACE]", "ASCEND",
       glm::vec4(0.35f, 0.90f, 1.0f, 1.0f)},
      {"SLIDE/DROP", "[S / DOWN]", "DESCEND",
       glm::vec4(1.0f, 0.80f, 0.30f, 1.0f)},
      {"SHADOW DASH", "[SHIFT]", "INVULN BURST",
       glm::vec4(0.30f, 0.95f, 1.0f, 1.0f)},
      {"KATANA ATK", "[L-CLICK]", "OR [J] / [X]",
       glm::vec4(1.0f, 0.45f, 0.45f, 1.0f)},
      {"DEFEND/PARRY", "[R-CLICK]", "OR [K] (1s)",
       glm::vec4(0.45f, 0.92f, 1.0f, 1.0f)},
  };

  for (int i = 0; i < numPlayerCells; ++i) {
    float cx1 = innerLeft + i * (cellW + cellGap);
    float cx2 = cx1 + cellW;
    float cx = (cx1 + cx2) * 0.5f;
    const auto &def = playerPreviews[i];

    drawPanel(cx1, pcBottom, cx2, pcTop, glm::vec4(0.06f, 0.10f, 0.17f, 0.96f),
              glm::vec4(def.accent.r, def.accent.g, def.accent.b, 0.50f));

    // Subtle divider between animation box and control label footer
    glm::mat4 mSubDiv = glm::mat4(1.0f);
    mSubDiv = glm::translate(mSubDiv, glm::vec3(cx, animBoxBottom, 0.0f));
    mSubDiv = glm::scale(mSubDiv, glm::vec3(cellW - 0.10f, 0.025f, 1.0f));
    gameRenderer.drawQuad(mSubDiv, identityView, orthoUI,
                          glm::vec4(0.25f, 0.38f, 0.55f, 0.65f), false,
                          glm::vec4(0), 0);

    // Clip animated Samurai sprite strictly inside the upper animation area
    GLboolean wasScissor = GL_FALSE;
    GLint prevScissor[4] = {0, 0, 0, 0};
    beginCellScissor(cx1 + 0.04f, animBoxBottom + 0.03f, cx2 - 0.04f,
                     pcTop - 0.04f, wasScissor, prevScissor);

    unsigned int pTex = samuraiAnim.texIdle;
    int pFrames = samuraiAnim.idleFrames;
    int pFrame = 0;
    float pAngle = 0.0f;
    float pOffsetY = 0.0f;
    glm::vec4 pTint(1.0f);
    bool drawDashGhosts = false;

    if (i == 0) {
      pTex = samuraiAnim.texRun;
      pFrames = samuraiAnim.runFrames;
      pFrame = static_cast<int>(animTimer * 9.0f) % std::max(1, pFrames);
    } else if (i == 1) {
      float cycle = std::fmod(animTimer, 1.3f);
      pOffsetY = std::sin((cycle / 1.3f) * 3.14159f) * 0.24f - 0.06f;
      if (cycle < 0.65f) {
        pTex = samuraiAnim.texJump;
        pFrames = samuraiAnim.jumpFrames;
        pFrame = static_cast<int>(cycle * 3.0f) % std::max(1, pFrames);
      } else {
        pTex = samuraiAnim.texFall;
        pFrames = samuraiAnim.fallFrames;
        pFrame =
            static_cast<int>((cycle - 0.65f) * 3.0f) % std::max(1, pFrames);
      }
    } else if (i == 2) {
      pTex = samuraiAnim.texAttack;
      pFrames = samuraiAnim.attackFrames;
      pFrame = std::min(1, std::max(0, pFrames - 1));
      pAngle = -0.45f;
    } else if (i == 3) {
      pTex = samuraiAnim.texRun;
      pFrames = samuraiAnim.runFrames;
      pFrame = static_cast<int>(animTimer * 14.0f) % std::max(1, pFrames);
      pTint = glm::vec4(0.45f, 0.92f, 1.0f, 1.0f);
      drawDashGhosts = true;
    } else if (i == 4) {
      pTex = samuraiAnim.texAttack;
      pFrames = samuraiAnim.attackFrames;
      pFrame = static_cast<int>(animTimer * 10.0f) % std::max(1, pFrames);
    } else if (i == 5) {
      pTex = samuraiAnim.texDefend;
      pFrames = samuraiAnim.defendFrames;
      pFrame = static_cast<int>(animTimer * 8.0f) % std::max(1, pFrames);
    }

    float pu =
        static_cast<float>(pFrame) / static_cast<float>(std::max(1, pFrames));
    float pdu = 1.0f / static_cast<float>(std::max(1, pFrames));
    glm::vec4 pUV(pu, 0.0f, pdu, 1.0f);
    float animCenterY = (animBoxBottom + pcTop) * 0.5f + pOffsetY - 0.06f;
    float spriteQuadSize = 3.85f;

    if (drawDashGhosts && pTex != 0) {
      for (int g = 1; g <= 2; ++g) {
        glm::mat4 mGhost = glm::mat4(1.0f);
        mGhost = glm::translate(mGhost,
                                glm::vec3(cx - 0.24f * g, animCenterY, 0.0f));
        mGhost =
            glm::scale(mGhost, glm::vec3(spriteQuadSize, spriteQuadSize, 1.0f));
        gameRenderer.drawQuad(mGhost, identityView, orthoUI,
                              glm::vec4(0.2f, 0.85f, 1.0f, 0.55f - 0.20f * g),
                              true, pUV, pTex);
      }
    }

    if (pTex != 0) {
      glm::mat4 mSprite = glm::mat4(1.0f);
      mSprite = glm::translate(mSprite, glm::vec3(cx, animCenterY, 0.0f));
      if (std::abs(pAngle) > 0.01f) {
        mSprite = glm::rotate(mSprite, pAngle, glm::vec3(0.0f, 0.0f, 1.0f));
      }
      mSprite =
          glm::scale(mSprite, glm::vec3(spriteQuadSize, spriteQuadSize, 1.0f));
      gameRenderer.drawQuad(mSprite, identityView, orthoUI, pTint, true, pUV,
                            pTex);
    }
    endCellScissor(wasScissor, prevScissor);

    // Top tiny action badge inside cell
    float actW =
        static_cast<float>(std::string(def.actionTitle).length()) * 0.115f;
    gameRenderer.drawText(def.actionTitle, cx - actW * 0.5f, pcTop - 0.22f,
                          0.115f, 0.16f, def.accent, orthoUI);

    // Player controls shown directly below the corresponding animation
    float c1W =
        static_cast<float>(std::string(def.controlLine1).length()) * 0.135f;
    gameRenderer.drawText(def.controlLine1, cx - c1W * 0.5f,
                          animBoxBottom - 0.24f, 0.135f, 0.19f,
                          glm::vec4(1.0f, 0.92f, 0.45f, 1.0f), orthoUI);
    float c2W =
        static_cast<float>(std::string(def.controlLine2).length()) * 0.115f;
    gameRenderer.drawText(def.controlLine2, cx - c2W * 0.5f,
                          animBoxBottom - 0.47f, 0.115f, 0.16f,
                          glm::vec4(0.84f, 0.92f, 1.0f, 0.95f), orthoUI);
  }

  // =========================================================================
  // 2. ENEMIES (2 Grid Cells Looping ALL Animations of the 2 Enemy Types)
  // =========================================================================
  float enHeaderY = pcBottom - 0.40f;
  gameRenderer.drawText("ENEMIES", innerLeft, enHeaderY, 0.19f, 0.26f,
                        glm::vec4(1.0f, 0.72f, 0.30f, 1.0f), orthoUI);

  const float enGap = 0.30f;
  const float enCellW = (innerW - enGap) * 0.5f;
  const float enTop = enHeaderY - 0.10f;
  const float enBottom = enTop - 2.25f;

  // --- Enemy Grid Cell 1: Bringer of Death (Loops ALL 6 Animations + Thunder
  // Spell) ---
  {
    float ex1 = innerLeft;
    float ex2 = ex1 + enCellW;
    drawPanel(ex1, enBottom, ex2, enTop, glm::vec4(0.06f, 0.08f, 0.15f, 0.96f),
              glm::vec4(0.82f, 0.42f, 0.98f, 0.55f));

    float previewX1 = ex1 + 0.10f;
    float previewX2 = ex1 + 3.45f;
    float previewY1 = enBottom + 0.10f;
    float previewY2 = enTop - 0.10f;
    drawPanel(previewX1, previewY1, previewX2, previewY2,
              glm::vec4(0.03f, 0.04f, 0.09f, 0.95f),
              glm::vec4(0.55f, 0.35f, 0.80f, 0.45f));

    float bodT = std::fmod(animTimer, 7.2f);
    unsigned int bTex = enemyTex.bodIdle;
    int bFrames = 8;
    int bFrame = 0;
    const char *bPhase = "IDLE";
    bool showSpell = false;
    int spellFrame = 0;
    glm::vec4 bTint(1.0f);

    if (bodT < 1.2f) {
      bTex = enemyTex.bodIdle;
      bFrames = 8;
      bFrame = static_cast<int>(bodT * 6.0f) % bFrames;
      bPhase = "IDLE";
    } else if (bodT < 2.2f) {
      bTex = enemyTex.bodWalk;
      bFrames = 8;
      bFrame = static_cast<int>((bodT - 1.2f) * 8.0f) % bFrames;
      bPhase = "WALK";
    } else if (bodT < 3.45f) {
      bTex = enemyTex.bodAttack;
      bFrames = 10;
      bFrame = std::min(bFrames - 1, static_cast<int>((bodT - 2.2f) * 8.0f));
      bPhase = "SCYTHE ATTACK";
    } else if (bodT < 5.05f) {
      bTex = enemyTex.bodCast;
      bFrames = 9;
      float dt = bodT - 3.45f;
      bFrame = std::min(bFrames - 1, static_cast<int>(dt * 7.5f));
      bPhase = "CAST + THUNDER SPELL";
      showSpell = true;
      spellFrame = std::min(15, static_cast<int>(dt * 10.0f));
    } else if (bodT < 5.80f) {
      bTex = enemyTex.bodHurt;
      bFrames = 3;
      bFrame = static_cast<int>((bodT - 5.05f) * 6.0f) % bFrames;
      bPhase = "HURT";
      bTint = glm::vec4(1.0f, 0.45f, 0.45f, 1.0f);
    } else {
      bTex = enemyTex.bodDeath;
      bFrames = 10;
      bFrame = std::min(bFrames - 1, static_cast<int>((bodT - 5.80f) * 7.5f));
      bPhase = "DEATH";
    }

    GLboolean wasScissor = GL_FALSE;
    GLint prevScissor[4] = {0, 0, 0, 0};
    beginCellScissor(previewX1 + 0.03f, previewY1 + 0.03f, previewX2 - 0.03f,
                     previewY2 - 0.03f, wasScissor, prevScissor);
    if (bTex != 0) {
      float eu = static_cast<float>(bFrame) / static_cast<float>(bFrames);
      float edu = 1.0f / static_cast<float>(bFrames);
      // Flip horizontally so BOD faces right inside preview box
      glm::vec4 eUV(eu + edu, 0.0f, -edu, 1.0f);
      float bodCenterX = showSpell ? ((previewX1 + previewX2) * 0.5f - 0.25f)
                                   : ((previewX1 + previewX2) * 0.5f + 0.65f);
      float bodCenterY = (previewY1 + previewY2) * 0.5f - 0.12f;
      glm::mat4 mBod = glm::mat4(1.0f);
      mBod = glm::translate(mBod, glm::vec3(bodCenterX, bodCenterY, 0.0f));
      mBod = glm::scale(mBod, glm::vec3(2.85f, 1.90f, 1.0f));
      gameRenderer.drawQuad(mBod, identityView, orthoUI, bTint, true, eUV,
                            bTex);
    }
    if (showSpell && enemyTex.bodSpell != 0) {
      float su = static_cast<float>(spellFrame) / 16.0f;
      glm::vec4 sUV(su, 0.0f, 1.0f / 16.0f, 1.0f);
      glm::mat4 mSpell = glm::mat4(1.0f);
      mSpell = glm::translate(
          mSpell,
          glm::vec3(previewX2 - 0.72f, (previewY1 + previewY2) * 0.5f, 0.0f));
      mSpell = glm::scale(mSpell, glm::vec3(1.35f, 1.75f, 1.0f));
      gameRenderer.drawQuad(mSpell, identityView, orthoUI,
                            glm::vec4(1.0f, 1.0f, 1.0f, 0.95f), true, sUV,
                            enemyTex.bodSpell);
    }
    endCellScissor(wasScissor, prevScissor);

    float infoX = previewX2 + 0.25f;
    float infoW = ex2 - infoX - 0.20f;
    drawWrappedText("BRINGER OF DEATH  [1 HIT]", infoX, enTop - 0.34f, 0.155f,
                    0.22f, infoW, glm::vec4(0.88f, 0.55f, 1.0f, 1.0f), orthoUI,
                    0.25f);
    gameRenderer.drawText(std::string("LOOP: ") + bPhase, infoX, enTop - 0.66f,
                          0.135f, 0.19f, glm::vec4(1.0f, 0.88f, 0.35f, 1.0f),
                          orthoUI);
    drawWrappedText("Patrols platforms & caverns, telegraphs scythe cleaves, "
                    "and casts multi-layer Thunder Spells.",
                    infoX, enTop - 0.98f, 0.13f, 0.185f, infoW,
                    glm::vec4(0.86f, 0.92f, 0.98f, 0.95f), orthoUI, 0.24f);
    drawWrappedText("Anim: Idle > Walk > Attack > Cast+Spell > Hurt > Death",
                    infoX, enBottom + 0.38f, 0.105f, 0.15f, infoW,
                    glm::vec4(0.62f, 0.75f, 0.90f, 0.90f), orthoUI, 0.19f);
  }

  // --- Enemy Grid Cell 2: Slime -> Boss Demon Slime (Loops ALL 10 Animations)
  // ---
  {
    float ex1 = innerLeft + enCellW + enGap;
    float ex2 = ex1 + enCellW;
    drawPanel(ex1, enBottom, ex2, enTop, glm::vec4(0.06f, 0.08f, 0.15f, 0.96f),
              glm::vec4(0.95f, 0.38f, 0.32f, 0.55f));

    float previewX1 = ex1 + 0.10f;
    float previewX2 = ex1 + 3.45f;
    float previewY1 = enBottom + 0.10f;
    float previewY2 = enTop - 0.10f;
    drawPanel(previewX1, previewY1, previewX2, previewY2,
              glm::vec4(0.03f, 0.04f, 0.09f, 0.95f),
              glm::vec4(0.85f, 0.32f, 0.32f, 0.45f));

    float slT = std::fmod(animTimer, 14.5f);
    unsigned int sTex = enemyTex.slimeIdle;
    int sFrames = 6;
    int sFrame = 0;
    glm::vec2 sRender(1.55f, 1.55f);
    const char *sPhase = "SLIME IDLE";
    glm::vec4 sTint(1.0f);

    if (slT < 1.0f) {
      sTex = enemyTex.slimeIdle;
      sFrames = 6;
      sFrame = static_cast<int>(slT * 6.0f) % sFrames;
      sRender = glm::vec2(1.55f, 1.55f);
      sPhase = "SLIME IDLE";
    } else if (slT < 2.0f) {
      sTex = enemyTex.slimeMove;
      sFrames = 8;
      sFrame = static_cast<int>((slT - 1.0f) * 8.0f) % sFrames;
      sRender = glm::vec2(1.55f, 1.55f);
      sPhase = "SLIME MOVE";
    } else if (slT < 2.8f) {
      sTex = enemyTex.slimeTakeHit;
      sFrames = 6;
      sFrame = static_cast<int>((slT - 2.0f) * 8.0f) % sFrames;
      sRender = glm::vec2(1.55f, 1.55f);
      sPhase = "SLIME HIT";
      sTint = glm::vec4(1.0f, 0.45f, 0.45f, 1.0f);
    } else if (slT < 6.0f) {
      sTex = enemyTex.slimeTransform;
      sFrames = 33;
      sFrame = std::min(sFrames - 1, static_cast<int>((slT - 2.8f) * 10.3f));
      sRender = glm::vec2(2.30f, 2.30f);
      sPhase = "TRANSFORMING -> BOSS";
    } else if (slT < 7.0f) {
      sTex = enemyTex.demonIdle;
      sFrames = 6;
      sFrame = static_cast<int>((slT - 6.0f) * 6.0f) % sFrames;
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS IDLE";
    } else if (slT < 8.4f) {
      sTex = enemyTex.demonWalk;
      sFrames = 12;
      sFrame = static_cast<int>((slT - 7.0f) * 8.5f) % sFrames;
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS WALK";
    } else if (slT < 10.0f) {
      sTex = enemyTex.demonAttack;
      sFrames = 15;
      sFrame = std::min(sFrames - 1, static_cast<int>((slT - 8.4f) * 9.5f));
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS CLEAVE";
    } else if (slT < 11.7f) {
      sTex = enemyTex.demonSmash;
      sFrames = 18;
      sFrame = std::min(sFrames - 1, static_cast<int>((slT - 10.0f) * 10.5f));
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS JUMP SMASH";
    } else if (slT < 12.4f) {
      sTex = enemyTex.demonTakeHit;
      sFrames = 5;
      sFrame = static_cast<int>((slT - 11.7f) * 8.0f) % sFrames;
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS HIT";
      sTint = glm::vec4(1.0f, 0.45f, 0.45f, 1.0f);
    } else {
      sTex = enemyTex.demonDeath;
      sFrames = 22;
      sFrame = std::min(sFrames - 1, static_cast<int>((slT - 12.4f) * 10.5f));
      sRender = glm::vec2(3.15f, 1.75f);
      sPhase = "DEMON BOSS DEATH";
    }

    GLboolean wasScissor = GL_FALSE;
    GLint prevScissor[4] = {0, 0, 0, 0};
    beginCellScissor(previewX1 + 0.03f, previewY1 + 0.03f, previewX2 - 0.03f,
                     previewY2 - 0.03f, wasScissor, prevScissor);
    if (sTex != 0) {
      float su = static_cast<float>(sFrame) / static_cast<float>(sFrames);
      float sdu = 1.0f / static_cast<float>(sFrames);
      glm::vec4 sUV(su, 0.0f, sdu, 1.0f);
      glm::mat4 mSlime = glm::mat4(1.0f);
      mSlime = glm::translate(
          mSlime, glm::vec3((previewX1 + previewX2) * 0.5f,
                            (previewY1 + previewY2) * 0.5f - 0.05f, 0.0f));
      mSlime = glm::scale(mSlime, glm::vec3(sRender.x, sRender.y, 1.0f));
      gameRenderer.drawQuad(mSlime, identityView, orthoUI, sTint, true, sUV,
                            sTex);
    }
    endCellScissor(wasScissor, prevScissor);

    float infoX = previewX2 + 0.25f;
    float infoW = ex2 - infoX - 0.20f;
    drawWrappedText("SLIME [2 HP] -> DEMON BOSS [5 HP]", infoX, enTop - 0.34f,
                    0.125f, 0.19f, infoW, glm::vec4(0.35f, 0.95f, 0.55f, 1.0f),
                    orthoUI, 0.23f);
    gameRenderer.drawText(std::string("LOOP: ") + sPhase, infoX, enTop - 0.66f,
                          0.135f, 0.19f, glm::vec4(1.0f, 0.88f, 0.35f, 1.0f),
                          orthoUI);
    drawWrappedText("Defeating a Slime (2 hits) awakens the Demon Boss (5 "
                    "hits). Leap-smashes platforms; dash or flee to despawn.",
                    infoX, enTop - 0.98f, 0.13f, 0.185f, infoW,
                    glm::vec4(0.86f, 0.92f, 0.98f, 0.95f), orthoUI, 0.24f);
    drawWrappedText("Anim: Slime(Idle/Move/Hit) > Transform > "
                    "Boss(Idle/Walk/Atk/Smash/Hit/Death)",
                    infoX, enBottom + 0.38f, 0.105f, 0.15f, infoW,
                    glm::vec4(0.62f, 0.75f, 0.90f, 0.90f), orthoUI, 0.19f);
  }

  // =========================================================================
  // 3. GAME MECHANICS (Left Column) & 4. OTHERS (Right Column)
  // =========================================================================
  const float secTop = enBottom - 0.48f;
  const float secBottom = -tutH * 0.5f + 0.88f;
  const float colGap = 0.32f;
  const float gmLeft = innerLeft;
  const float gmRight = innerLeft + 9.65f;
  const float otLeft = gmRight + colGap;
  const float otRight = innerRight;

  // --- Section 3: GAME MECHANICS ---
  gameRenderer.drawText("GAME MECHANICS", gmLeft, secTop + 0.13f, 0.19f, 0.26f,
                        glm::vec4(0.35f, 0.95f, 1.0f, 1.0f), orthoUI);
  drawPanel(gmLeft, secBottom, gmRight, secTop,
            glm::vec4(0.05f, 0.09f, 0.15f, 0.94f),
            glm::vec4(0.25f, 0.75f, 0.95f, 0.45f));

  struct TutRow {
    std::string text;
    glm::vec4 color;
  };
  const TutRow gmRows[] = {
      {"MODES  : Free Exploration is DEFAULT ([A/D] move). Press [F] to toggle "
       "Auto-Run!",
       glm::vec4(0.35f, 1.0f, 0.65f, 1.0f)},
      {"LIVES  : 3 Hits absorbed before death. Pick up Hearts to restore "
       "lives.",
       glm::vec4(1.0f, 0.35f, 0.65f, 1.0f)},
      {"SHIELDS: Pick up glowing Shield icons to absorb 1 hazard/enemy hit.",
       glm::vec4(0.35f, 0.95f, 1.0f, 1.0f)},
      {"WARPS  : Touch Direction Relics to transition into new dimensions.",
       glm::vec4(0.45f, 1.0f, 0.55f, 1.0f)},
      {"SHIFTS : Press [Y] to toggle Auto-Shift Timer (or rely solely on "
       "relics).",
       glm::vec4(1.0f, 0.68f, 0.28f, 1.0f)},
  };

  float gmTextX = gmLeft + 0.28f;
  float gmMaxW = (gmRight - gmLeft) - 0.56f;
  float gmY = secTop - 0.46f;
  for (const auto &row : gmRows) {
    int lines = drawWrappedText(row.text, gmTextX, gmY, 0.16f, 0.23f, gmMaxW,
                                row.color, orthoUI, 0.29f, 9.0f * 0.16f);
    gmY -= 0.42f + (lines - 1) * 0.29f;
  }

  // --- Section 4: OTHERS ---
  gameRenderer.drawText("OTHERS", otLeft, secTop + 0.13f, 0.19f, 0.26f,
                        glm::vec4(0.80f, 0.72f, 1.0f, 1.0f), orthoUI);
  drawPanel(otLeft, secBottom, otRight, secTop,
            glm::vec4(0.05f, 0.09f, 0.15f, 0.94f),
            glm::vec4(0.65f, 0.55f, 0.95f, 0.45f));

  const TutRow otRows[] = {
      {"MUSIC   : BGM defaults to 30% volume. Slider in [ESC] Pause (10% "
       "steps), [M] Mute.",
       glm::vec4(0.40f, 0.85f, 1.0f, 1.0f)},
      {"TUTORIAL: Check the pause menu for controls, or press 'T' at any time "
       "to view this tutorial window again without losing your progress.",
       glm::vec4(0.35f, 1.0f, 0.70f, 1.0f)},
      {"EXTRAS  : [ESC] Pause  [B] Scenery  [N] Day/Night  [F11] Fullscreen  "
       "[R] Restart",
       glm::vec4(0.85f, 0.90f, 1.0f, 0.92f)},
  };

  float otTextX = otLeft + 0.28f;
  float otMaxW = (otRight - otLeft) - 0.56f;
  float otY = secTop - 0.46f;
  for (const auto &row : otRows) {
    int lines = drawWrappedText(row.text, otTextX, otY, 0.15f, 0.22f, otMaxW,
                                row.color, orthoUI, 0.28f, 10.0f * 0.15f);
    otY -= 0.42f + (lines - 1) * 0.28f;
  }

  // Footer Resume / Begin Prompt
  std::string promptStr = openedFromPause
                              ? "PRESS [SPACE / ENTER / ESC / T] TO RESUME"
                              : "PRESS [SPACE / ENTER / ESC] TO BEGIN VENTURE";
  float prW = static_cast<float>(promptStr.length()) * 0.22f;
  gameRenderer.drawText(promptStr, -prW * 0.5f, -tutH * 0.5f + 0.34f, 0.22f,
                        0.32f, glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), orthoUI);
}

void UIManager::renderGameOver(float halfW, float halfH,
                               const std::string &reason, int totalScore,
                               int coins, float distance, int highScore,
                               bool isNewRecord) {
  glm::mat4 identityView = glm::mat4(1.0f);
  glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

  glm::mat4 modelOverlay = glm::mat4(1.0f);
  modelOverlay =
      glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
  gameRenderer.drawQuad(modelOverlay, identityView, orthoUI,
                        glm::vec4(0.12f, 0.02f, 0.04f, 0.82f), false,
                        glm::vec4(0), 0);

  float overW = 11.5f;
  float overH = 7.7f;
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

  gameRenderer.drawText("FINAL SCORE  : " + std::to_string(totalScore) + " PTS",
                        textX, curY, 0.25f, 0.36f,
                        glm::vec4(1.0f, 0.88f, 0.22f, 1.0f), orthoUI);
  curY -= 0.55f;

  std::string bestLine =
      "HIGHEST SCORE: " + std::to_string(std::max(highScore, totalScore)) +
      " PTS";
  if (isNewRecord && totalScore > 0) {
    bestLine += "  [NEW RECORD!]";
  }
  gameRenderer.drawText(bestLine, textX, curY, 0.23f, 0.34f,
                        (isNewRecord && totalScore > 0)
                            ? glm::vec4(0.38f, 1.0f, 0.72f, 1.0f)
                            : glm::vec4(0.55f, 0.92f, 1.0f, 1.0f),
                        orthoUI);
  curY -= 0.60f;

  gameRenderer.drawText(
      "DISTANCE     : " + std::to_string(static_cast<int>(distance)) +
          " METERS",
      textX, curY, 0.20f, 0.30f, glm::vec4(0.9f, 0.95f, 1.0f, 1.0f), orthoUI);
  curY -= 0.50f;

  gameRenderer.drawText("GOLD COINS   : " + std::to_string(coins) + " (+ " +
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
