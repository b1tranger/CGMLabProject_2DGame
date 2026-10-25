#include "SkyFallScene.h"
#include "Renderer.h"
#include "Audio.h"
#include "UIManager.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

SkyFallScene gameSkyFallScene;

SkyFallScene::SkyFallScene() {
    pos = glm::vec2(0.0f, 0.0f);
    velX = 0.0f;
    initStars();
}

void SkyFallScene::initStars() {
    stars.clear();
    for (int i = 0; i < 64; ++i) {
        CelestialStar star;
        star.x = -16.0f + static_cast<float>(std::rand() % 320) / 10.0f;
        star.y = 1.0f + static_cast<float>(std::rand() % 70) / 10.0f;
        star.size = 0.08f + static_cast<float>(std::rand() % 14) / 100.0f;
        star.twinkleFreq = 1.8f + static_cast<float>(std::rand() % 30) / 10.0f;
        star.twinklePhase = static_cast<float>(std::rand() % 628) / 100.0f;
        stars.push_back(star);
    }
}

void SkyFallScene::enter(const glm::vec2& pPos, RunDirection lastDir) {
    platformerSavedPos = pPos;
    platformerSavedDir = lastDir;
    pos = glm::vec2(0.0f, 0.0f);
    velX = 0.0f;
    isHit = false;
    hitTimer = 0.0f;
    darkAlpha = 0.0f;
    birds.clear();
    spikes.clear();
    items.clear();
    genBottomY = -8.0f;
    scenePlayTime = 0.0f;
    directionUnlockedNotified = false;

    // Generate initial birds, spike mines, and items with spacious pacing
    for (int i = 0; i < 4; ++i) {
        float y = -8.0f - i * 14.0f;

        if (i % 2 == 0) {
            bool fromLeft = (i % 4 == 0);
            float birdX = fromLeft ? -9.5f : 9.5f;
            float birdVx = fromLeft ? (3.0f + (std::rand() % 15) / 10.0f) : -(3.0f + (std::rand() % 15) / 10.0f);
            birds.push_back({ glm::vec2(birdX, y), birdVx, static_cast<float>(std::rand() % 10), true });
        }

        if (i % 3 == 1) {
            float trapX = ((std::rand() % 60) / 10.0f) - 3.0f;
            spikes.push_back({ glm::vec2(trapX, y - 3.0f), glm::vec2(0.85f, 0.85f), true });
        }

        float itemX = ((std::rand() % 80) / 10.0f) - 4.0f;
        if (i % 2 == 0) {
            items.push_back({ glm::vec2(itemX, y - 1.5f), 1, std::rand() % 4, false }); // Shield
        } else {
            items.push_back({ glm::vec2(itemX, y - 1.5f), 0, 0, false }); // Coin
        }
        genBottomY = y;
    }

    showCornerNotification("SCENE ENTER", "vvv SKY FALL DIVE vvv", "Survive 15s trial! Steer A/D, SHIFT to Dash!", glm::vec4(0.95f, 0.85f, 0.2f, 1.0f), 1, 0);
    spawnFloatingText("vvv SKY-FALL TRIAL (15s) vvv", glm::vec2(0.0f, 1.2f), glm::vec4(0.95f, 0.85f, 0.25f, 1.0f));
}

void SkyFallScene::update(float deltaTime, GLFWwindow* window,
                          float& shieldTimer, float& postHitInvulnTimer, bool isDashing,
                          int& coinsCollected, int& relicsTriggered, float& scoreDistance,
                          bool& requestExitToRunner, RunDirection& exitDir)
{
    requestExitToRunner = false;
    (void)postHitInvulnTimer;

    scenePlayTime += deltaTime;
    if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN && !directionUnlockedNotified) {
        directionUnlockedNotified = true;
        audioMgr.play(SFX_RELIC);
        spawnFloatingText("=== EXIT APPEARED! ===", pos + glm::vec2(0.0f, 1.2f),
                          glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), 0.32f, 0.46f, 3.2f, 1.3f);
        showCornerNotification("EXIT", "=== EXIT APPEARED! ===", "Direction Arrow Relics spawned! Steer into an arrow to return!", glm::vec4(0.35f, 1.0f, 0.5f, 1.0f), 1, 0);

        // Convert uncollected items ahead into Left/Right Direction Relics
        for (auto& itm : items) {
            if (!itm.collected && itm.pos.y < pos.y && itm.pos.y > pos.y - 35.0f) {
                itm.type = 2;
                itm.fruitType = (itm.pos.x <= 0.0f) ? DIR_LEFT : DIR_RIGHT;
            }
        }

        // Spawn staggered Left & Right Direction Arrow gates visibly ahead in the descent route
        items.push_back({ glm::vec2(-2.2f, pos.y - 6.5f), 2, DIR_LEFT, false });
        items.push_back({ glm::vec2( 2.2f, pos.y - 6.5f), 2, DIR_RIGHT, false });

        items.push_back({ glm::vec2(-3.0f, pos.y - 12.0f), 2, DIR_LEFT, false });
        items.push_back({ glm::vec2( 0.0f, pos.y - 12.0f), 2, (platformerSavedDir == DIR_RIGHT ? DIR_RIGHT : DIR_LEFT), false });
        items.push_back({ glm::vec2( 3.0f, pos.y - 12.0f), 2, DIR_RIGHT, false });

        items.push_back({ glm::vec2(-2.2f, pos.y - 17.5f), 2, DIR_LEFT, false });
        items.push_back({ glm::vec2( 2.2f, pos.y - 17.5f), 2, DIR_RIGHT, false });
    }

    if (!isHit) {
        pos.y -= fallSpeed * deltaTime;
        scoreDistance += fallSpeed * deltaTime;
        pos.x += velX * deltaTime;
        pos.x = std::clamp(pos.x, -7.0f, 7.0f);
        velX *= 0.86f;

        while (genBottomY > pos.y - 25.0f) {
            genBottomY -= 14.0f;
            if (std::rand() % 2 == 0) {
                bool fromLeft = (std::rand() % 2 == 0);
                float birdX = fromLeft ? -9.5f : 9.5f;
                float birdVx = fromLeft ? (3.2f + (std::rand() % 12) / 10.0f) : -(3.2f + (std::rand() % 12) / 10.0f);
                birds.push_back({ glm::vec2(birdX, genBottomY), birdVx, static_cast<float>(std::rand() % 10), true });
            }

            if (std::rand() % 3 == 0) {
                float trapX = ((std::rand() % 60) / 10.0f) - 3.0f;
                spikes.push_back({ glm::vec2(trapX, genBottomY - 2.5f), glm::vec2(0.85f, 0.85f), true });
            }

            if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN) {
                int roll = std::rand() % 10;
                if (roll < 4) {
                    RunDirection arrowDir = (std::rand() % 2 == 0) ? DIR_LEFT : DIR_RIGHT;
                    items.push_back({ glm::vec2(-2.2f, genBottomY - 5.0f), 2, arrowDir, false });
                    items.push_back({ glm::vec2( 2.2f, genBottomY - 5.0f), 2, (arrowDir == DIR_LEFT ? DIR_RIGHT : DIR_LEFT), false });
                } else if (roll < 7) {
                    items.push_back({ glm::vec2(-4.0f, genBottomY - 5.0f), 1, std::rand() % 4, false }); // Shield
                    items.push_back({ glm::vec2( 4.0f, genBottomY - 5.0f), 0, 0, false }); // Coin
                } else {
                    items.push_back({ glm::vec2(0.0f, genBottomY - 5.0f), 0, 0, false }); // Coin
                }
            } else {
                float itemX = ((std::rand() % 80) / 10.0f) - 4.0f;
                if (std::rand() % 2 == 0) {
                    items.push_back({ glm::vec2(itemX, genBottomY - 1.5f), 1, std::rand() % 4, false });
                } else {
                    items.push_back({ glm::vec2(itemX, genBottomY - 1.5f), 0, 0, false });
                }
            }
        }

        // Bird movements & collisions
        for (auto& bird : birds) {
            if (!bird.active) continue;
            bird.pos.x += bird.vx * deltaTime;
            bird.animTimer += deltaTime;
            if (bird.pos.x < -14.0f || bird.pos.x > 14.0f) bird.active = false;

            if (bird.active && std::abs(bird.pos.x - pos.x) < 1.15f && std::abs(bird.pos.y - pos.y) < 1.0f) {
                if (shieldTimer > 0.0f || isDashing) {
                    bird.active = false;
                    audioMgr.play(SFX_SLIDE);
                    if (shieldTimer > 0.0f && !isDashing) {
                        shieldTimer = 0.0f;
                        spawnFloatingText("SHIELD BROKE! [BIRD DEFLECTED]", pos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                        showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed bird strike!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                    } else {
                        spawnFloatingText("DASH DEFLECTED BIRD!", pos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                    }
                } else {
                    isHit = true;
                    hitTimer = 0.0f;
                    darkAlpha = 0.0f;
                    audioMgr.play(SFX_DEATH);
                    spawnFloatingText("STRUCK BY RAPTOR!", pos + glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 0.35f, 0.2f, 1.0f));
                    showCornerNotification("HIT", "INTERCEPTED BY RAPTOR!", "Returning to surface runner!", glm::vec4(1.0f, 0.4f, 0.2f, 1.0f), 0);
                    break;
                }
            }
        }

        // Spike collisions
        for (auto& trap : spikes) {
            if (!trap.active) continue;
            if (std::abs(trap.pos.x - pos.x) < 0.95f && std::abs(trap.pos.y - pos.y) < 0.95f) {
                if (shieldTimer > 0.0f || isDashing) {
                    trap.active = false;
                    audioMgr.play(SFX_SLIDE);
                    if (shieldTimer > 0.0f && !isDashing) {
                        shieldTimer = 0.0f;
                        spawnFloatingText("SHIELD BROKE! [MINE DEFLECTED]", pos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                        showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed aerial mine!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                    } else {
                        spawnFloatingText("DASH DEFLECTED MINE!", pos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                    }
                } else {
                    isHit = true;
                    hitTimer = 0.0f;
                    darkAlpha = 0.0f;
                    audioMgr.play(SFX_DEATH);
                    spawnFloatingText("MINE DETONATION!", pos + glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 0.35f, 0.2f, 1.0f));
                    showCornerNotification("HIT", "AERIAL MINE EXPLODED!", "Returning to surface runner!", glm::vec4(1.0f, 0.4f, 0.2f, 1.0f), 0);
                    break;
                }
            }
        }

        // Item collisions
        for (auto& itm : items) {
            float distThreshX = (itm.type == 2) ? 1.85f : 1.1f;
            float distThreshY = (itm.type == 2) ? 2.1f : 1.1f;
            if (!itm.collected &&
                std::abs(itm.pos.x - pos.x) < distThreshX &&
                std::abs(itm.pos.y - pos.y) < distThreshY)
            {
                itm.collected = true;
                if (itm.type == 0) {
                    coinsCollected++;
                    audioMgr.play(SFX_COIN);
                    spawnFloatingText("+50", pos + glm::vec2(0.0f, 0.5f), glm::vec4(1.0f, 0.88f, 0.2f, 1.0f));
                } else if (itm.type == 1) {
                    shieldTimer = 10.0f;
                    audioMgr.play(SFX_RELIC);
                    showCornerNotification("SHIELD", "10s ENERGY SHIELD!", "Absorbs 1 hazard hit!", glm::vec4(0.2f, 0.95f, 1.0f, 1.0f), 1, itm.fruitType);
                    spawnFloatingText("+10s ENERGY SHIELD!", pos + glm::vec2(0.0f, 0.8f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                } else if (itm.type == 2) {
                    RunDirection targetDir = static_cast<RunDirection>(itm.fruitType);
                    audioMgr.play(SFX_RELIC);
                    relicsTriggered++;
                    if (targetDir == DIR_LEFT || targetDir == DIR_RIGHT) {
                        exitDir = targetDir;
                        requestExitToRunner = true;
                        std::string dirStr = (targetDir == DIR_LEFT) ? "WARP <<< LEFT" : "WARP >>> RIGHT";
                        spawnFloatingText("DIRECTION TRIGGERED!\n" + dirStr, pos + glm::vec2(0.0f, 0.8f),
                                          glm::vec4(0.25f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.8f, 1.3f);
                        showCornerNotification("WARP", dirStr, "Resuming regular platform runner!", glm::vec4(0.25f, 0.95f, 1.0f, 1.0f), 1, 0);
                        break;
                    }
                }
            }
        }
    } else {
        hitTimer += deltaTime;
        darkAlpha = std::min(1.0f, darkAlpha + deltaTime * 1.8f);
        if (hitTimer >= 1.15f) {
            isHit = false;
            requestExitToRunner = true;
            exitDir = platformerSavedDir;
        }
    }

    // Steering input
    if (!isHit) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            velX = -7.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            velX = 7.5f;
        }
    }
}

void SkyFallScene::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec2& cameraPos,
                          float animTimer, float daylightFactor, float twilightFactor,
                          unsigned int birdTex, unsigned int tilesetTex, unsigned int coinTex,
                          unsigned int shieldTex, unsigned int arrowTex, unsigned int sceneryTex,
                          float shieldTimerVal, const glm::vec2& playerRenderSize)
{
    // Celestial Stars in night sky
    if (daylightFactor < 0.75f) {
        float starAlpha = (1.0f - daylightFactor) * 0.95f;
        for (const auto& star : stars) {
            float tw = 0.5f + 0.5f * std::sin(animTimer * star.twinkleFreq + star.twinklePhase);
            float a = starAlpha * tw;
            if (a > 0.04f) {
                glm::mat4 modelStar = glm::mat4(1.0f);
                modelStar = glm::translate(modelStar, glm::vec3(cameraPos.x * 0.05f + star.x, cameraPos.y * 0.05f + star.y, 0.0f));
                modelStar = glm::scale(modelStar, glm::vec3(star.size, star.size, 1.0f));
                gameRenderer.drawQuad(modelStar, view, projection, glm::vec4(1.0f, 1.0f, 0.95f, a), false, glm::vec4(0), 0);
            }
        }
    }

    // Distant Scenery Backdrop
    glm::vec4 nightSceneryTint(0.42f, 0.48f, 0.70f, 1.0f);
    glm::vec4 sunsetSceneryTint(1.08f, 0.72f, 0.58f, 1.0f);
    glm::vec4 daySceneryTint(1.02f, 1.0f, 0.98f, 1.0f);
    glm::vec4 curSceneryTint = glm::mix(nightSceneryTint, daySceneryTint, daylightFactor);
    curSceneryTint = glm::mix(curSceneryTint, sunsetSceneryTint, twilightFactor * 0.55f);

    glm::mat4 modelScenery = glm::mat4(1.0f);
    modelScenery = glm::translate(modelScenery, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
    modelScenery = glm::scale(modelScenery, glm::vec3(30.0f, 18.0f, 1.0f));
    gameRenderer.drawQuad(modelScenery, view, projection, curSceneryTint * 0.65f,
                          (sceneryTex != 0), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), sceneryTex);

    // 1. Endless Sky Clouds Backdrop
    for (int i = -4; i <= 4; ++i) {
        float bgX = i * 4.0f;
        glm::mat4 modelBg = glm::mat4(1.0f);
        modelBg = glm::translate(modelBg, glm::vec3(bgX, cameraPos.y, 0.0f));
        modelBg = glm::scale(modelBg, glm::vec3(2.5f, 30.0f, 1.0f));
        gameRenderer.drawQuad(modelBg, view, projection, glm::vec4(0.14f, 0.24f, 0.40f, 0.75f), false, glm::vec4(0), 0);
    }

    // 2. Render Floating Spike Traps
    for (const auto& trap : spikes) {
        if (!trap.active) continue;
        glm::mat4 modelTrap = glm::mat4(1.0f);
        modelTrap = glm::translate(modelTrap, glm::vec3(trap.pos.x, trap.pos.y, 0.0f));
        modelTrap = glm::scale(modelTrap, glm::vec3(1.3f, 1.3f, 1.0f));
        glm::vec4 trapUV = getTilesetUV(4, 8);
        gameRenderer.drawQuad(modelTrap, view, projection, glm::vec4(1.0f, 0.35f, 0.35f, 1.0f), (tilesetTex != 0), trapUV, tilesetTex);
    }

    // 3. Render Moving Birds (Flapping Raptor Asset)
    for (const auto& bird : birds) {
        if (!bird.active) continue;
        int bFrame = static_cast<int>(bird.animTimer * 7.0f) % 4;
        float u = bFrame * 0.25f;
        float du = 0.25f;
        glm::vec4 birdUV = (bird.vx < 0.0f) ? glm::vec4(u + du, 0.0f, -du, 1.0f) : glm::vec4(u, 0.0f, du, 1.0f);

        glm::mat4 modelBird = glm::mat4(1.0f);
        modelBird = glm::translate(modelBird, glm::vec3(bird.pos.x, bird.pos.y, 0.0f));
        modelBird = glm::scale(modelBird, glm::vec3(2.4f, 2.4f, 1.0f));
        gameRenderer.drawQuad(modelBird, view, projection, glm::vec4(1.0f), (birdTex != 0), birdUV, birdTex);
    }

    // 4. Render Sky Collectibles (Coins, Shield Pickups & Direction Arrow Relics)
    for (const auto& itm : items) {
        if (itm.collected) continue;
        if (itm.type == 0) {
            glm::mat4 modelCoin = glm::mat4(1.0f);
            modelCoin = glm::translate(modelCoin, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
            modelCoin = glm::scale(modelCoin, glm::vec3(0.9f, 0.9f, 1.0f));
            gameRenderer.drawQuad(modelCoin, view, projection, glm::vec4(1.0f), (coinTex != 0), getCoinUV(0), coinTex);
        } else if (itm.type == 1) {
            glm::mat4 modelShield = glm::mat4(1.0f);
            modelShield = glm::translate(modelShield, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
            modelShield = glm::scale(modelShield, glm::vec3(1.2f, 1.2f, 1.0f));
            gameRenderer.drawQuad(modelShield, view, projection, glm::vec4(1.0f), (shieldTex != 0), getShieldUV(itm.fruitType), shieldTex);
        } else if (itm.type == 2) {
            RunDirection relicDir = static_cast<RunDirection>(itm.fruitType);
            float pulse = 1.65f + 0.20f * std::sin(animTimer * 5.0f);
            glm::mat4 modelRelic = glm::mat4(1.0f);
            modelRelic = glm::translate(modelRelic, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
            modelRelic = glm::scale(modelRelic, glm::vec3(pulse, pulse, 1.0f));

            glm::vec4 arrowUV = getDirectionArrowUV(relicDir);
            glm::vec4 arrowTint(1.0f);
            if (relicDir == DIR_LEFT)       arrowTint = glm::vec4(0.2f, 0.95f, 1.0f, 1.0f);
            else if (relicDir == DIR_RIGHT) arrowTint = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
            else if (relicDir == DIR_UP)    arrowTint = glm::vec4(1.0f, 0.35f, 0.35f, 1.0f);

            gameRenderer.drawQuad(modelRelic, view, projection, arrowTint, (arrowTex != 0), arrowUV, arrowTex);
        }
    }

    // 5. Render Character in Sky-Fall (or Take Hit rotated 90 deg CCW!)
    CharacterAnimSet& activeSet = samuraiAnim;
    if (isHit) {
        unsigned int hitTex = (activeSet.texTakeHit != 0) ? activeSet.texTakeHit : activeSet.texDeath;
        int totalFrames = (activeSet.texTakeHit != 0) ? activeSet.takeHitFrames : activeSet.deathFrames;
        int fIdx = std::clamp(static_cast<int>(hitTimer * 6.0f), 0, totalFrames - 1);
        float u = static_cast<float>(fIdx) / static_cast<float>(totalFrames);
        float du = 1.0f / static_cast<float>(totalFrames);
        glm::vec4 uvRect(u, 0.0f, du, 1.0f);

        glm::mat4 modelHero = glm::mat4(1.0f);
        modelHero = glm::translate(modelHero, glm::vec3(pos.x, pos.y, 0.0f));
        modelHero = glm::rotate(modelHero, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
        gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f, 0.6f, 0.6f, 1.0f), (hitTex != 0), uvRect, hitTex);

        if (darkAlpha > 0.01f) {
            glm::mat4 modelDark = glm::mat4(1.0f);
            modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
            modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
            gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.0f, 0.0f, 0.0f, darkAlpha), false, glm::vec4(0), 0);
        }
    } else {
        unsigned int activeTex = activeSet.texFall;
        int totalFrames = activeSet.fallFrames;
        int fIdx = 0;
        float u = static_cast<float>(fIdx) / static_cast<float>(totalFrames);
        float du = 1.0f / static_cast<float>(totalFrames);
        bool faceLeft = (velX < -0.1f);
        glm::vec4 uvRect = faceLeft ? glm::vec4(u + du, 0.0f, -du, 1.0f) : glm::vec4(u, 0.0f, du, 1.0f);

        glm::mat4 modelHero = glm::mat4(1.0f);
        modelHero = glm::translate(modelHero, glm::vec3(pos.x, pos.y, 0.0f));
        modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
        gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeTex != 0), uvRect, activeTex);

        if (shieldTimerVal > 0.0f) {
            float shieldPulse = 1.0f + std::sin(animTimer * 8.0f) * 0.06f;
            glm::mat4 modelShield = glm::mat4(1.0f);
            modelShield = glm::translate(modelShield, glm::vec3(pos.x, pos.y, 0.0f));
            modelShield = glm::scale(modelShield, glm::vec3(2.35f * shieldPulse, 2.35f * shieldPulse, 1.0f));
            float sAlpha = std::min(0.70f, 0.35f + (shieldTimerVal / 10.0f) * 0.35f);
            gameRenderer.drawCircle(modelShield, view, projection, glm::vec4(0.2f, 0.85f, 1.0f, sAlpha), 2.2f);
        }
    }
}
