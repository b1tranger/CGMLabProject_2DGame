#include "LadderScene.h"
#include "Renderer.h"
#include "Audio.h"
#include "UIManager.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

LadderScene gameLadderScene;

LadderScene::LadderScene() {
    currentLane = 1;
    targetLane = 1;
    playerX = TRACK_X[1];
    playerY = 0.0f;
}

void LadderScene::enter(const glm::vec2& pPos, RunDirection lastDir) {
    platformerSavedPos = pPos;
    platformerSavedDir = lastDir;
    currentLane = 1;
    targetLane = 1;
    playerY = 0.0f;
    playerX = TRACK_X[1];
    isLeaping = false;
    leapProgress = 1.0f;
    isHit = false;
    hitVelY = 0.0f;
    darkAlpha = 0.0f;
    isResurrectingState = false;
    resurrectTimer = 0.0f;
    traps.clear();
    items.clear();
    genTopY = 5.0f;
    scenePlayTime = 0.0f;
    directionUnlockedNotified = false;

    // Generate initial traps and collectibles across the 3 endless ladders (initial ~25m)
    for (int i = 0; i < 6; ++i) {
        float y = 5.0f + i * 4.2f;
        int trapLane = std::rand() % 3;
        traps.push_back({ trapLane, y, std::rand() % 2, true });

        int itemLane = (trapLane + 1 + (std::rand() % 2)) % 3;
        if (i % 2 == 0) {
            items.push_back({ itemLane, y + 1.2f, 1, std::rand() % 4, false }); // Shield pickup
        } else {
            items.push_back({ itemLane, y + 1.2f, 0, 0, false }); // Coin
        }
        genTopY = y;
    }

    showCornerNotification("SCENE ENTER", "^^^ 3-LADDER ASCENT ^^^", "Survive 15s trial before direction shifts!", glm::vec4(0.35f, 0.95f, 0.4f, 1.0f), 1, 2);
    spawnFloatingText("^^^ 3-LADDER TRIAL (15s) ^^^", glm::vec2(0.0f, 1.2f), glm::vec4(0.35f, 1.0f, 0.45f, 1.0f));
}

void LadderScene::update(float deltaTime, GLFWwindow* window,
                         float& shieldTimer, float& postHitInvulnTimer, bool isDashing,
                         int& coinsCollected, int& relicsTriggered, float& scoreDistance,
                         bool& requestExitToRunner, RunDirection& exitDir, bool& requestSkyFall)
{
    requestExitToRunner = false;
    requestSkyFall = false;
    (void)postHitInvulnTimer;

    scenePlayTime += deltaTime;
    if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN && !directionUnlockedNotified) {
        directionUnlockedNotified = true;
        audioMgr.play(SFX_RELIC);
        spawnFloatingText("=== EXIT APPEARED! ===", glm::vec2(playerX, playerY + 1.2f),
                          glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), 0.32f, 0.46f, 3.2f, 1.3f);
        showCornerNotification("EXIT", "=== EXIT APPEARED! ===", "Direction Arrow Relics spawned! Climb into an arrow to return!", glm::vec4(0.35f, 1.0f, 0.5f, 1.0f), 1, 0);

        // Spawn Left and Right Direction Arrow Relics visibly ahead on the tracks
        items.push_back({ 0, playerY + 5.2f, 2, DIR_LEFT, false });
        items.push_back({ 2, playerY + 8.2f, 2, DIR_RIGHT, false });
        items.push_back({ 1, playerY + 11.8f, 2, DIR_LEFT, false });
    }

    if (!isHit && !isResurrectingState) {
        playerY += climbSpeed * deltaTime;
        scoreDistance += climbSpeed * deltaTime;

        if (isLeaping) {
            leapProgress += deltaTime / LEAP_TIME;
            if (leapProgress >= 1.0f) {
                leapProgress = 1.0f;
                isLeaping = false;
                currentLane = targetLane;
                playerX = TRACK_X[currentLane];
            } else {
                float fromX = TRACK_X[currentLane];
                float toX   = TRACK_X[targetLane];
                playerX = glm::mix(fromX, toX, leapProgress);
            }
        } else {
            playerX = TRACK_X[currentLane];
        }

        // Procedural generation of traps and items on 3 ladders
        while (genTopY < playerY + 25.0f) {
            genTopY += 4.5f;
            int trapLane = std::rand() % 3;
            traps.push_back({ trapLane, genTopY, std::rand() % 2, true });

            int itemLane = (trapLane + 1 + (std::rand() % 2)) % 3;
            if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN) {
                int roll = std::rand() % 10;
                if (roll < 4) {
                    RunDirection arrowDir = (std::rand() % 2 == 0) ? DIR_LEFT : DIR_RIGHT;
                    items.push_back({ itemLane, genTopY + 1.2f, 2, arrowDir, false });
                } else if (roll < 7) {
                    items.push_back({ itemLane, genTopY + 1.2f, 1, std::rand() % 4, false }); // Shield pickup
                } else {
                    items.push_back({ itemLane, genTopY + 1.2f, 0, 0, false }); // Coin
                }
            } else {
                if (std::rand() % 2 == 0) {
                    items.push_back({ itemLane, genTopY + 1.2f, 1, std::rand() % 4, false });
                } else {
                    items.push_back({ itemLane, genTopY + 1.2f, 0, 0, false });
                }
            }
        }

        // Trap collisions
        for (auto& trap : traps) {
            if (trap.active && trap.lane == currentLane && std::abs(playerY - trap.y) < 0.9f) {
                if (shieldTimer > 0.0f || isDashing) {
                    trap.active = false;
                    audioMgr.play(SFX_SLIDE);
                    if (shieldTimer > 0.0f && !isDashing) {
                        shieldTimer = 0.0f;
                        spawnFloatingText("SHIELD BROKE! [TRAP DEFLECTED]", glm::vec2(playerX, playerY + 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                        showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed trap hit!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                    } else {
                        spawnFloatingText("DASH DEFLECTED TRAP!", glm::vec2(playerX, playerY + 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                    }
                } else {
                    isHit = true;
                    hitVelY = -6.0f;
                    darkAlpha = 0.0f;
                    audioMgr.play(SFX_DEATH);
                    spawnFloatingText("STRUCK BY TRAP!", glm::vec2(playerX, playerY + 1.0f), glm::vec4(1.0f, 0.35f, 0.25f, 1.0f));
                    showCornerNotification("FALL", "KNOCKED OFF LADDER!", "Falling through the dark...", glm::vec4(1.0f, 0.35f, 0.2f, 1.0f), 0);
                    break;
                }
            }
        }

        // Collectibles collisions (Coins, Shield Pickups & Direction Arrow Relics)
        for (auto& itm : items) {
            if (!itm.collected && itm.lane == currentLane && std::abs(playerY - itm.y) < 1.0f) {
                itm.collected = true;
                if (itm.type == 0) {
                    coinsCollected++;
                    audioMgr.play(SFX_COIN);
                    spawnFloatingText("+50", glm::vec2(playerX, itm.y + 0.5f), glm::vec4(1.0f, 0.88f, 0.2f, 1.0f));
                } else if (itm.type == 1) {
                    shieldTimer = 10.0f;
                    audioMgr.play(SFX_RELIC);
                    showCornerNotification("SHIELD", "10s ENERGY SHIELD!", "Absorbs 1 hazard hit!", glm::vec4(0.2f, 0.95f, 1.0f, 1.0f), 1, itm.fruitType);
                    spawnFloatingText("+10s ENERGY SHIELD!", glm::vec2(playerX, itm.y + 0.8f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                } else if (itm.type == 2) {
                    RunDirection targetDir = static_cast<RunDirection>(itm.fruitType);
                    audioMgr.play(SFX_RELIC);
                    relicsTriggered++;
                    if (targetDir == DIR_LEFT || targetDir == DIR_RIGHT) {
                        exitDir = targetDir;
                        requestExitToRunner = true;
                        std::string dirStr = (targetDir == DIR_LEFT) ? "WARP <<< LEFT" : "WARP >>> RIGHT";
                        spawnFloatingText("DIRECTION TRIGGERED!\n" + dirStr, glm::vec2(playerX, itm.y + 0.8f),
                                          glm::vec4(0.25f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.8f, 1.3f);
                        showCornerNotification("WARP", dirStr, "Exiting ladder climb to platform runner!", glm::vec4(0.25f, 0.95f, 1.0f, 1.0f), 1, 0);
                        break;
                    } else if (targetDir == DIR_DOWN) {
                        requestSkyFall = true;
                        spawnFloatingText("WARP vvv SKY-FALL", glm::vec2(playerX, itm.y + 0.8f), glm::vec4(0.3f, 1.0f, 0.5f, 1.0f));
                        break;
                    }
                }
            }
        }
    } else if (isHit) {
        playerY += hitVelY * deltaTime;
        hitVelY -= 22.0f * deltaTime;
        darkAlpha = std::min(1.0f, darkAlpha + deltaTime * 1.5f);
        if (darkAlpha >= 0.95f) {
            isHit = false;
            isResurrectingState = true;
            resurrectTimer = 0.0f;
        }
    } else if (isResurrectingState) {
        resurrectTimer += deltaTime;
        if (resurrectTimer >= 1.25f) {
            isResurrectingState = false;
            requestExitToRunner = true;
            exitDir = platformerSavedDir;
        }
    }

    // Lane switching input with A/D or Left/Right
    if (!isHit && !isResurrectingState && !isLeaping) {
        static bool aKeyLast = false;
        static bool dKeyLast = false;
        bool aPress = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
        bool dPress = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);

        if (aPress && !aKeyLast) {
            if (currentLane > 0) {
                targetLane = currentLane - 1;
                isLeaping = true;
                leapProgress = 0.0f;
                audioMgr.play(SFX_JUMP);
            }
        }
        if (dPress && !dKeyLast) {
            if (currentLane < 2) {
                targetLane = currentLane + 1;
                isLeaping = true;
                leapProgress = 0.0f;
                audioMgr.play(SFX_JUMP);
            }
        }
        aKeyLast = aPress;
        dKeyLast = dPress;
    }
}

void LadderScene::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec2& cameraPos,
                         float animTimer, float daylightFactor, float twilightFactor,
                         unsigned int tilesetTex, unsigned int coinTex, unsigned int shieldTex, unsigned int arrowTex,
                         unsigned int sceneryTex, float shieldTimerVal, const glm::vec2& playerRenderSize)
{
    // Thematic Scenery Backdrop behind ladders
    glm::vec4 nightSceneryTint(0.42f, 0.48f, 0.70f, 1.0f);
    glm::vec4 sunsetSceneryTint(1.08f, 0.72f, 0.58f, 1.0f);
    glm::vec4 daySceneryTint(1.02f, 1.0f, 0.98f, 1.0f);
    glm::vec4 curSceneryTint = glm::mix(nightSceneryTint, daySceneryTint, daylightFactor);
    curSceneryTint = glm::mix(curSceneryTint, sunsetSceneryTint, twilightFactor * 0.55f);

    glm::mat4 modelScenery = glm::mat4(1.0f);
    modelScenery = glm::translate(modelScenery, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
    modelScenery = glm::scale(modelScenery, glm::vec3(28.0f, 18.0f, 1.0f));
    gameRenderer.drawQuad(modelScenery, view, projection, curSceneryTint * 0.72f,
                          (sceneryTex != 0), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), sceneryTex);

    // 1. Dark Vertical Tower Backdrop
    for (int i = -4; i <= 4; ++i) {
        float bgX = i * 4.0f;
        glm::mat4 modelBg = glm::mat4(1.0f);
        modelBg = glm::translate(modelBg, glm::vec3(bgX, cameraPos.y, 0.0f));
        modelBg = glm::scale(modelBg, glm::vec3(2.6f, 32.0f, 1.0f));
        gameRenderer.drawQuad(modelBg, view, projection, glm::vec4(0.07f, 0.09f, 0.15f, 0.90f), false, glm::vec4(0), 0);
    }

    // 2. Render 3 Infinite Parallel Ladders
    for (int lane = 0; lane < 3; ++lane) {
        float lx = TRACK_X[lane];
        float minY = std::floor(cameraPos.y - 12.0f);
        float maxY = std::ceil(cameraPos.y + 12.0f);

        // Left & Right ladder rails
        glm::mat4 modelRailL = glm::mat4(1.0f);
        modelRailL = glm::translate(modelRailL, glm::vec3(lx - 0.72f, cameraPos.y, 0.0f));
        modelRailL = glm::scale(modelRailL, glm::vec3(0.18f, 24.0f, 1.0f));
        gameRenderer.drawQuad(modelRailL, view, projection, glm::vec4(0.48f, 0.34f, 0.20f, 1.0f), false, glm::vec4(0), 0);

        glm::mat4 modelRailR = glm::mat4(1.0f);
        modelRailR = glm::translate(modelRailR, glm::vec3(lx + 0.72f, cameraPos.y, 0.0f));
        modelRailR = glm::scale(modelRailR, glm::vec3(0.18f, 24.0f, 1.0f));
        gameRenderer.drawQuad(modelRailR, view, projection, glm::vec4(0.48f, 0.34f, 0.20f, 1.0f), false, glm::vec4(0), 0);

        // Ladder Rungs
        for (float ry = minY; ry <= maxY; ry += 0.85f) {
            glm::mat4 modelRung = glm::mat4(1.0f);
            modelRung = glm::translate(modelRung, glm::vec3(lx, ry, 0.0f));
            modelRung = glm::scale(modelRung, glm::vec3(1.30f, 0.24f, 1.0f));
            glm::vec4 rungUV = getTilesetUV(9, 4);
            gameRenderer.drawQuad(modelRung, view, projection, glm::vec4(1.0f), (tilesetTex != 0), rungUV, tilesetTex);
        }
    }

    // 3. Render Ladder Traps (Spikes / Sawblades)
    for (const auto& trap : traps) {
        if (!trap.active) continue;
        float tx = TRACK_X[trap.lane];
        glm::mat4 modelTrap = glm::mat4(1.0f);
        modelTrap = glm::translate(modelTrap, glm::vec3(tx, trap.y, 0.0f));
        modelTrap = glm::scale(modelTrap, glm::vec3(1.35f, 1.35f, 1.0f));
        glm::vec4 trapUV = getTilesetUV(4, 8);
        gameRenderer.drawQuad(modelTrap, view, projection, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), (tilesetTex != 0), trapUV, tilesetTex);
    }

    // 4. Render Ladder Collectibles (Coins, Shield Pickups & Direction Arrow Relics)
    for (const auto& itm : items) {
        if (itm.collected) continue;
        float ix = TRACK_X[itm.lane];
        if (itm.type == 0) {
            glm::mat4 modelCoin = glm::mat4(1.0f);
            modelCoin = glm::translate(modelCoin, glm::vec3(ix, itm.y, 0.0f));
            modelCoin = glm::scale(modelCoin, glm::vec3(0.9f, 0.9f, 1.0f));
            gameRenderer.drawQuad(modelCoin, view, projection, glm::vec4(1.0f), (coinTex != 0), getCoinUV(0), coinTex);
        } else if (itm.type == 1) {
            glm::mat4 modelShield = glm::mat4(1.0f);
            modelShield = glm::translate(modelShield, glm::vec3(ix, itm.y, 0.0f));
            modelShield = glm::scale(modelShield, glm::vec3(1.2f, 1.2f, 1.0f));
            gameRenderer.drawQuad(modelShield, view, projection, glm::vec4(1.0f), (shieldTex != 0), getShieldUV(itm.fruitType), shieldTex);
        } else if (itm.type == 2) {
            RunDirection relicDir = static_cast<RunDirection>(itm.fruitType);
            float pulse = 1.35f + 0.15f * std::sin(animTimer * 5.0f);
            glm::mat4 modelRelic = glm::mat4(1.0f);
            modelRelic = glm::translate(modelRelic, glm::vec3(ix, itm.y, 0.0f));
            modelRelic = glm::scale(modelRelic, glm::vec3(pulse, pulse, 1.0f));

            glm::vec4 arrowUV = getDirectionArrowUV(relicDir);
            glm::vec4 arrowTint(1.0f);
            if (relicDir == DIR_LEFT)       arrowTint = glm::vec4(0.2f, 0.95f, 1.0f, 1.0f);
            else if (relicDir == DIR_RIGHT) arrowTint = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
            else if (relicDir == DIR_DOWN)  arrowTint = glm::vec4(0.3f, 1.0f, 0.5f, 1.0f);

            gameRenderer.drawQuad(modelRelic, view, projection, arrowTint, (arrowTex != 0), arrowUV, arrowTex);
        }
    }

    // 5. Render Character on Ladder (Climber, Falling, or Reverse Death Resurrection!)
    CharacterAnimSet& activeSet = samuraiAnim;
    if (isResurrectingState) {
        glm::mat4 modelDark = glm::mat4(1.0f);
        modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
        modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
        gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.01f, 0.02f, 0.04f, 0.94f), false, glm::vec4(0), 0);

        glm::mat4 modelSpot = glm::mat4(1.0f);
        modelSpot = glm::translate(modelSpot, glm::vec3(cameraPos.x, cameraPos.y - 0.2f, 0.0f));
        modelSpot = glm::scale(modelSpot, glm::vec3(4.8f, 4.8f, 1.0f));
        gameRenderer.drawQuad(modelSpot, view, projection, glm::vec4(0.25f, 0.85f, 1.0f, 0.28f), false, glm::vec4(0), 0);

        int totalFrames = activeSet.deathFrames;
        int frameIdx = std::clamp(totalFrames - 1 - static_cast<int>(resurrectTimer * 5.0f), 0, totalFrames - 1);
        float u = static_cast<float>(frameIdx) / static_cast<float>(totalFrames);
        float du = 1.0f / static_cast<float>(totalFrames);
        glm::vec4 uvRect(u, 0.0f, du, 1.0f);

        glm::mat4 modelHero = glm::mat4(1.0f);
        modelHero = glm::translate(modelHero, glm::vec3(cameraPos.x, cameraPos.y - 0.10f, 0.0f));
        modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
        gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeSet.texDeath != 0), uvRect, activeSet.texDeath);

        std::string rezText = "=== RESURRECTION... RISE! ===";
        float rezW = rezText.length() * 0.30f;
        gameRenderer.drawText(rezText, cameraPos.x - rezW * 0.5f, cameraPos.y + 2.2f, 0.30f, 0.42f,
                              glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), projection, view);
    } else {
        unsigned int activeTex = isLeaping ? activeSet.texJump : activeSet.texJump;
        int totalFrames = activeSet.jumpFrames;
        int frameIdx = 0;

        if (isHit) {
            activeTex = activeSet.texFall;
            totalFrames = activeSet.fallFrames;
            frameIdx = 0;
        } else if (!isLeaping) {
            int climbStep = static_cast<int>(animTimer * 4.0f) % 2;
            activeTex = (climbStep == 0) ? activeSet.texJump : activeSet.texFall;
            totalFrames = (climbStep == 0) ? activeSet.jumpFrames : activeSet.fallFrames;
        }

        float u = static_cast<float>(frameIdx) / static_cast<float>(totalFrames);
        float du = 1.0f / static_cast<float>(totalFrames);
        glm::vec4 uvRect(u, 0.0f, du, 1.0f);

        glm::mat4 modelHero = glm::mat4(1.0f);
        modelHero = glm::translate(modelHero, glm::vec3(playerX, playerY - 0.10f, 0.0f));
        modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
        gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeTex != 0), uvRect, activeTex);

        // Circular shield bubble
        if (shieldTimerVal > 0.0f) {
            float shieldPulse = 1.0f + std::sin(animTimer * 8.0f) * 0.06f;
            glm::mat4 modelShield = glm::mat4(1.0f);
            modelShield = glm::translate(modelShield, glm::vec3(playerX, playerY, 0.0f));
            modelShield = glm::scale(modelShield, glm::vec3(2.35f * shieldPulse, 2.35f * shieldPulse, 1.0f));
            float sAlpha = std::min(0.70f, 0.35f + (shieldTimerVal / 10.0f) * 0.35f);
            gameRenderer.drawCircle(modelShield, view, projection, glm::vec4(0.2f, 0.85f, 1.0f, sAlpha), 2.2f);
        }

        if (darkAlpha > 0.01f) {
            glm::mat4 modelDark = glm::mat4(1.0f);
            modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
            modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
            gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.0f, 0.0f, 0.0f, darkAlpha), false, glm::vec4(0), 0);
        }
    }
}
