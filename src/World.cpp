#include "World.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

World gameWorld;

World::World()
    : rightGenEdge(35.0f), leftGenEdge(-25.0f) {}

bool World::checkCollision(const glm::vec2& posA, const glm::vec2& sizeA,
                           const glm::vec2& posB, const glm::vec2& sizeB) const {
    float halfAx = sizeA.x * 0.5f;
    float halfAy = sizeA.y * 0.5f;
    float halfBx = sizeB.x * 0.5f;
    float halfBy = sizeB.y * 0.5f;

    bool overlapX = (posA.x + halfAx >= posB.x - halfBx) && (posA.x - halfAx <= posB.x + halfBx);
    bool overlapY = (posA.y + halfAy >= posB.y - halfBy) && (posA.y - halfAy <= posB.y + halfBy);

    return overlapX && overlapY;
}

void World::generateChunk(float startX, float endX, bool isInitial, EnvironmentTheme theme) {
    const float TILE_SIZE = 1.0f;

    // List of planned open chasms (e.g. for downward slopes to underground) so platforms never block them!
    struct ChasmSpan {
        float x1, x2;
    };
    std::vector<ChasmSpan> chasmSpans;

    // Plan steep slopes leading underground in advance so we carve out the surface ground!
    if (!isInitial) {
        float testX = startX + 4.0f;
        while (testX < endX - 6.0f) {
            if (std::rand() % 3 == 0) {
                float sX = testX;
                float eX = sX + 4.8f;
                chasmSpans.push_back({sX - 0.6f, eX + 0.6f});

                SteepSlope caveSlope;
                caveSlope.startX = sX;
                caveSlope.endX   = eX;
                caveSlope.startY = SURFACE_GROUND_Y + 0.5f; // Start flush with surface ground
                caveSlope.endY   = CAVERN_FLOOR_Y + 0.5f;   // End flush with cavern floor
                caveSlope.angle  = std::atan2(caveSlope.endY - caveSlope.startY, caveSlope.endX - caveSlope.startX);
                caveSlope.theme  = theme;
                slopes.push_back(caveSlope);

                testX += 12.0f;
            } else {
                testX += 8.0f;
            }
        }
    }

    auto isInsideChasm = [&](float x) {
        for (const auto& span : chasmSpans) {
            if (x >= span.x1 && x <= span.x2) return true;
        }
        return false;
    };

    // 1. Bedrock Safety Foundation (Zero out-of-bounds fall deaths!)
    for (float x = startX; x < endX; x += TILE_SIZE) {
        bool hasSpike = (!isInitial && (std::rand() % 16 == 0));
        if (hasSpike) {
            SpikeTrap spike;
            spike.pos = glm::vec2(x + TILE_SIZE * 0.5f, BEDROCK_Y + 0.62f);
            spike.size = glm::vec2(0.85f, 0.7f);
            spikes.push_back(spike);
        }

        PlatformTile bedrockTop;
        bedrockTop.pos = glm::vec2(x + TILE_SIZE * 0.5f, BEDROCK_Y);
        bedrockTop.size = glm::vec2(TILE_SIZE, TILE_SIZE);
        bedrockTop.tileX = 1; bedrockTop.tileY = 2; // Deep rock
        bedrockTop.isLadder = false; bedrockTop.isOneWay = false; bedrockTop.theme = theme;
        tiles.push_back(bedrockTop);
    }

    // 2. Surface Ground Level (Y = SURFACE_GROUND_Y = -0.5f)
    // NOTE: Surface ground tiles are explicitly OMITTED over any chasm/slope span so the underground path is NEVER blocked!
    for (float x = startX; x < endX; x += TILE_SIZE) {
        if (isInsideChasm(x + TILE_SIZE * 0.5f)) {
            continue; // Completely open chasm for downward slope / cavern entry!
        }

        PlatformTile sTop;
        sTop.pos = glm::vec2(x + TILE_SIZE * 0.5f, SURFACE_GROUND_Y);
        sTop.size = glm::vec2(TILE_SIZE, TILE_SIZE);
        sTop.tileX = 1; sTop.tileY = 0; // Surface grass
        sTop.isLadder = false; sTop.isOneWay = false; sTop.theme = theme;
        tiles.push_back(sTop);

        PlatformTile sDirt;
        sDirt.pos = glm::vec2(x + TILE_SIZE * 0.5f, SURFACE_GROUND_Y - TILE_SIZE);
        sDirt.size = glm::vec2(TILE_SIZE, TILE_SIZE);
        sDirt.tileX = 1; sDirt.tileY = 1;
        sDirt.isLadder = false; sDirt.isOneWay = false; sDirt.theme = theme;
        tiles.push_back(sDirt);
    }

    // 3. Underground Cavern Floor (Y = CAVERN_FLOOR_Y = -4.5f)
    for (float x = startX; x < endX; x += TILE_SIZE) {
        PlatformTile cTop;
        cTop.pos = glm::vec2(x + TILE_SIZE * 0.5f, CAVERN_FLOOR_Y);
        cTop.size = glm::vec2(TILE_SIZE, TILE_SIZE);
        cTop.tileX = 1; cTop.tileY = 1; // Subterranean stone/dirt floor
        cTop.isLadder = false; cTop.isOneWay = false; cTop.theme = theme;
        tiles.push_back(cTop);

        if (!isInitial && (std::rand() % 16 == 0)) {
            SpikeTrap spike;
            spike.pos = glm::vec2(x + TILE_SIZE * 0.5f, CAVERN_FLOOR_Y + 0.62f);
            spike.size = glm::vec2(0.85f, 0.7f);
            spikes.push_back(spike);
        }
    }

    if (isInitial) {
        // Generate structured starting platforms on both the left side (-24 to -2) and right side (4 to 30)
        auto spawnPlatform = [&](float px, float py, int length, bool withLadder, bool withRelic) {
            for (int i = 0; i < length; ++i) {
                PlatformTile plat;
                plat.pos = glm::vec2(px + (i + 0.5f) * TILE_SIZE, py);
                plat.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                plat.tileX = (i == 0) ? 0 : (i == length - 1) ? 2 : 1;
                plat.tileY = 0;
                plat.isLadder = false;
                plat.isOneWay = true;
                plat.theme = theme;
                tiles.push_back(plat);

                if (i % 2 == 1) {
                    AnimatedCoin coin;
                    coin.pos = glm::vec2(plat.pos.x, py + 0.9f);
                    coin.animTimer = static_cast<float>(i * 2);
                    coin.collected = false;
                    coins.push_back(coin);
                }
            }

            if (withLadder) {
                float shaftX = px + 1.5f;
                float shaftBottom = SURFACE_GROUND_Y;
                float shaftTop = py;

                LadderTunnel tunnel;
                tunnel.x = shaftX;
                tunnel.minY = shaftBottom;
                tunnel.maxY = shaftTop;
                tunnel.width = 2.4f;
                tunnel.playerInside = false;
                tunnels.push_back(tunnel);

                for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                    PlatformTile lad;
                    lad.pos = glm::vec2(shaftX, ly);
                    lad.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                    lad.tileX = 9; lad.tileY = (ly >= shaftTop - 0.1f) ? 3 : 4;
                    lad.isLadder = true; lad.isOneWay = false; lad.theme = theme;
                    tiles.push_back(lad);
                }
            }

            if (withRelic) {
                DirectionRelic relic;
                relic.pos = glm::vec2(px + (length * 0.4f) * TILE_SIZE, py + 1.25f);
                relic.animPhase = 0.0f;
                // Give distinct directional arrows
                RunDirection dirs[] = { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };
                relic.targetDir = dirs[std::rand() % 4];
                relic.collected = false;
                relics.push_back(relic);

                // Spawn a Fruit Shield pickup providing 10s temporary invulnerability
                FruitShield fruit;
                fruit.pos = glm::vec2(px + (length * 0.75f) * TILE_SIZE, py + 1.25f);
                fruit.animPhase = 0.5f;
                fruit.fruitType = std::rand() % 4;
                fruit.collected = false;
                fruits.push_back(fruit);
            }
        };

        // 1. LEFT SIDE STARTING PLATFORMS (Immediate exploration running left from spawn x=0):
        spawnPlatform(-8.0f, 1.8f, 5, false, false);               // Low tier jump
        spawnPlatform(-16.0f, 3.0f, 6, true, true);                // Mid tier with ladder & relic
        spawnPlatform(-24.0f, 5.2f, 6, true, true);                // Sky canopy tier with ladder & relic

        // Underground Cavern Shaft on the Left:
        {
            float shaftX = -12.0f;
            float shaftBottom = CAVERN_FLOOR_Y + 0.5f;
            float shaftTop = SURFACE_GROUND_Y;
            LadderTunnel tunnel;
            tunnel.x = shaftX;
            tunnel.minY = shaftBottom;
            tunnel.maxY = shaftTop;
            tunnel.width = 2.4f;
            tunnel.playerInside = false;
            tunnels.push_back(tunnel);

            for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                PlatformTile lad;
                lad.pos = glm::vec2(shaftX, ly);
                lad.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                lad.tileX = 9; lad.tileY = (ly >= shaftTop - 0.1f) ? 3 : 4;
                lad.isLadder = true; lad.isOneWay = false; lad.theme = theme;
                tiles.push_back(lad);
            }
        }

        // 2. RIGHT SIDE STARTING PLATFORMS:
        spawnPlatform(4.0f, 1.8f, 5, false, false);
        spawnPlatform(12.0f, 3.0f, 6, true, true);
        spawnPlatform(21.0f, 5.2f, 7, true, true);

        // Underground Cavern Shaft on the Right:
        {
            float shaftX = 11.0f;
            float shaftBottom = CAVERN_FLOOR_Y + 0.5f;
            float shaftTop = SURFACE_GROUND_Y;
            LadderTunnel tunnel;
            tunnel.x = shaftX;
            tunnel.minY = shaftBottom;
            tunnel.maxY = shaftTop;
            tunnel.width = 2.4f;
            tunnel.playerInside = false;
            tunnels.push_back(tunnel);

            for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                PlatformTile lad;
                lad.pos = glm::vec2(shaftX, ly);
                lad.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                lad.tileX = 9; lad.tileY = (ly >= shaftTop - 0.1f) ? 3 : 4;
                lad.isLadder = true; lad.isOneWay = false; lad.theme = theme;
                tiles.push_back(lad);
            }
        }

        return;
    }

    // 4. Multiple Upward Ladders inside Underground Cavern leading to surface / ledges
    float curX = startX + 2.0f;
    while (curX < endX - 4.0f) {
        if (!isInsideChasm(curX) && std::rand() % 2 == 0) {
            float shaftX = curX + 1.5f;
            float shaftBottom = CAVERN_FLOOR_Y + 0.5f;
            float shaftTop = SURFACE_GROUND_Y;

            LadderTunnel tunnel;
            tunnel.x = shaftX;
            tunnel.minY = shaftBottom;
            tunnel.maxY = shaftTop;
            tunnel.width = 2.4f;
            tunnel.playerInside = false;
            tunnels.push_back(tunnel);

            // Left Wall Column
            for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                PlatformTile wallL;
                wallL.pos = glm::vec2(shaftX - 0.85f, ly);
                wallL.size = glm::vec2(0.6f, TILE_SIZE);
                wallL.tileX = 0; wallL.tileY = 1;
                wallL.isLadder = false; wallL.isOneWay = false; wallL.theme = theme;
                tiles.push_back(wallL);
            }
            // Right Wall Column
            for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                PlatformTile wallR;
                wallR.pos = glm::vec2(shaftX + 0.85f, ly);
                wallR.size = glm::vec2(0.6f, TILE_SIZE);
                wallR.tileX = 2; wallR.tileY = 1;
                wallR.isLadder = false; wallR.isOneWay = false; wallR.theme = theme;
                tiles.push_back(wallR);
            }
            // Center Ladder Rungs
            for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                PlatformTile lad;
                lad.pos = glm::vec2(shaftX, ly);
                lad.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                lad.tileX = 9; lad.tileY = (ly >= shaftTop - 0.1f) ? 3 : 4;
                lad.isLadder = true; lad.isOneWay = false; lad.theme = theme;
                tiles.push_back(lad);
            }
        }

        // 5. Elevated Platforms & Sky Canopy Tiers (Y = 1.5, 2.6, Sky Canopy 5.5)
        float platY = (std::rand() % 3 == 0) ? 5.5f : ((std::rand() % 2 == 0) ? 2.6f : 1.5f);
        int platLen = 4 + (std::rand() % 5);

        // Make sure elevated platform doesn't block downward slope chasm
        bool platformBlockedByChasm = false;
        for (int i = 0; i < platLen; ++i) {
            if (isInsideChasm(curX + (i + 0.5f) * TILE_SIZE)) {
                platformBlockedByChasm = true;
                break;
            }
        }

        if (!platformBlockedByChasm) {
            for (int i = 0; i < platLen && (curX + i * TILE_SIZE < endX); ++i) {
                PlatformTile plat;
                plat.pos = glm::vec2(curX + (i + 0.5f) * TILE_SIZE, platY);
                plat.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                plat.tileX = (i == 0) ? 0 : (i == platLen - 1) ? 2 : 1;
                plat.tileY = 0;
                plat.isLadder = false;
                plat.isOneWay = true; // Drop-down throughable
                plat.theme = theme;
                tiles.push_back(plat);

                if (i % 2 == 1 && (std::rand() % 2 == 0)) {
                    AnimatedCoin coin;
                    coin.pos = glm::vec2(plat.pos.x, platY + 1.0f);
                    coin.animTimer = static_cast<float>(std::rand() % 12);
                    coin.collected = false;
                    coins.push_back(coin);
                }
            }

            // Direction Arrow Relic on elevated perch
            if (platY > 4.5f || (std::rand() % 3 == 0)) {
                DirectionRelic relic;
                relic.pos = glm::vec2(curX + (platLen * 0.5f) * TILE_SIZE, platY + 1.35f);
                relic.animPhase = static_cast<float>(std::rand() % 100) / 10.0f;
                RunDirection dirs[] = { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };
                relic.targetDir = dirs[std::rand() % 4];
                relic.collected = false;
                relics.push_back(relic);
            }

            // Fruit Shield pickup (grants 10s temporary shield)
            if (std::rand() % 2 == 0) {
                FruitShield fruit;
                fruit.pos = glm::vec2(curX + (platLen * 0.25f) * TILE_SIZE, platY + 1.35f);
                fruit.animPhase = static_cast<float>(std::rand() % 100) / 10.0f;
                fruit.fruitType = std::rand() % 4;
                fruit.collected = false;
                fruits.push_back(fruit);
            }

            // Upward Ladder Shaft connecting Surface up to Sky Canopy Tier!
            if (platY > 4.5f) {
                float shaftX = curX + 1.5f;
                float shaftBottom = SURFACE_GROUND_Y;
                float shaftTop = platY;

                LadderTunnel skyTunnel;
                skyTunnel.x = shaftX;
                skyTunnel.minY = shaftBottom;
                skyTunnel.maxY = shaftTop;
                skyTunnel.width = 2.4f;
                skyTunnel.playerInside = false;
                tunnels.push_back(skyTunnel);

                for (float ly = shaftBottom; ly <= shaftTop; ly += TILE_SIZE) {
                    PlatformTile wallL;
                    wallL.pos = glm::vec2(shaftX - 0.85f, ly);
                    wallL.size = glm::vec2(0.6f, TILE_SIZE);
                    wallL.tileX = 0; wallL.tileY = 1;
                    wallL.isLadder = false; wallL.isOneWay = false; wallL.theme = theme;
                    tiles.push_back(wallL);

                    PlatformTile wallR;
                    wallR.pos = glm::vec2(shaftX + 0.85f, ly);
                    wallR.size = glm::vec2(0.6f, TILE_SIZE);
                    wallR.tileX = 2; wallR.tileY = 1;
                    wallR.isLadder = false; wallR.isOneWay = false; wallR.theme = theme;
                    tiles.push_back(wallR);

                    PlatformTile lad;
                    lad.pos = glm::vec2(shaftX, ly);
                    lad.size = glm::vec2(TILE_SIZE, TILE_SIZE);
                    lad.tileX = 9; lad.tileY = (ly >= shaftTop - 0.1f) ? 3 : 4;
                    lad.isLadder = true; lad.isOneWay = false; lad.theme = theme;
                    tiles.push_back(lad);
                }

                // Steep downward return slope to get back from Sky Canopy down to Surface!
                SteepSlope returnSlope;
                returnSlope.startX = curX + (platLen * TILE_SIZE);
                returnSlope.endX   = returnSlope.startX + 5.2f;
                returnSlope.startY = platY;
                returnSlope.endY   = SURFACE_GROUND_Y + 0.5f;
                returnSlope.angle  = std::atan2(returnSlope.endY - returnSlope.startY, returnSlope.endX - returnSlope.startX);
                returnSlope.theme  = theme;
                slopes.push_back(returnSlope);

                curX = returnSlope.endX + 3.0f;
                continue;
            }
        }

        curX += (platLen * TILE_SIZE) + 4.0f;
    }
}

void World::updateProcedural(float playerX, EnvironmentTheme activeTheme) {
    if (playerX + 35.0f > rightGenEdge) {
        generateChunk(rightGenEdge, rightGenEdge + 35.0f, false, activeTheme);
        rightGenEdge += 35.0f;
    }
    if (playerX - 35.0f < leftGenEdge) {
        generateChunk(leftGenEdge - 35.0f, leftGenEdge, false, activeTheme);
        leftGenEdge -= 35.0f;
    }

    tiles.erase(std::remove_if(tiles.begin(), tiles.end(),
        [playerX](const PlatformTile& t) { return std::abs(t.pos.x - playerX) > 85.0f; }), tiles.end());

    tunnels.erase(std::remove_if(tunnels.begin(), tunnels.end(),
        [playerX](const LadderTunnel& t) { return std::abs(t.x - playerX) > 85.0f; }), tunnels.end());

    slopes.erase(std::remove_if(slopes.begin(), slopes.end(),
        [playerX](const SteepSlope& s) { return std::abs(s.startX - playerX) > 85.0f; }), slopes.end());

    spikes.erase(std::remove_if(spikes.begin(), spikes.end(),
        [playerX](const SpikeTrap& s) { return std::abs(s.pos.x - playerX) > 85.0f; }), spikes.end());

    relics.erase(std::remove_if(relics.begin(), relics.end(),
        [playerX](const DirectionRelic& r) { return std::abs(r.pos.x - playerX) > 85.0f; }), relics.end());

    fruits.erase(std::remove_if(fruits.begin(), fruits.end(),
        [playerX](const FruitShield& f) { return std::abs(f.pos.x - playerX) > 85.0f; }), fruits.end());

    coins.erase(std::remove_if(coins.begin(), coins.end(),
        [playerX](const AnimatedCoin& c) { return std::abs(c.pos.x - playerX) > 85.0f; }), coins.end());

    // Keep leftGenEdge and rightGenEdge tightly bounded to active tile presence so reversing never encounters voids!
    if (leftGenEdge < playerX - 80.0f) {
        leftGenEdge = playerX - 80.0f;
    }
    if (rightGenEdge > playerX + 80.0f) {
        rightGenEdge = playerX + 80.0f;
    }
}

void World::ensureGroundAround(float targetX, EnvironmentTheme theme) {
    // Generate chunks to cover [targetX - 45, targetX + 45]
    while (targetX - 45.0f < leftGenEdge) {
        generateChunk(leftGenEdge - 35.0f, leftGenEdge, false, theme);
        leftGenEdge -= 35.0f;
    }
    while (targetX + 45.0f > rightGenEdge) {
        generateChunk(rightGenEdge, rightGenEdge + 35.0f, false, theme);
        rightGenEdge += 35.0f;
    }

    // Verify solid platforms exist within immediate vicinity of targetX
    bool hasLocalPlatform = false;
    for (const auto& t : tiles) {
        if (!t.isLadder && std::abs(t.pos.x - targetX) < 8.0f) {
            hasLocalPlatform = true;
            break;
        }
    }
    if (!hasLocalPlatform) {
        generateChunk(targetX - 45.0f, targetX + 45.0f, false, theme);
        leftGenEdge = std::min(leftGenEdge, targetX - 45.0f);
        rightGenEdge = std::max(rightGenEdge, targetX + 45.0f);
    }
}

void World::reset(EnvironmentTheme initialTheme) {
    tiles.clear();
    tunnels.clear();
    slopes.clear();
    spikes.clear();
    relics.clear();
    fruits.clear();
    coins.clear();

    rightGenEdge = 35.0f;
    leftGenEdge  = -25.0f;

    // Safe starter area
    generateChunk(-25.0f, 35.0f, true, initialTheme);

    // Starter Ladder Tunnel Shaft
    LadderTunnel startTunnel;
    startTunnel.x = 6.0f;
    startTunnel.minY = SURFACE_GROUND_Y;
    startTunnel.maxY = 2.5f;
    startTunnel.width = 2.4f;
    startTunnel.playerInside = false;
    tunnels.push_back(startTunnel);

    for (float ly = startTunnel.minY; ly <= startTunnel.maxY; ly += 1.0f) {
        PlatformTile wL;
        wL.pos = glm::vec2(startTunnel.x - 0.85f, ly);
        wL.size = glm::vec2(0.6f, 1.0f);
        wL.tileX = 0; wL.tileY = 1;
        wL.isLadder = false; wL.isOneWay = false; wL.theme = 0;
        tiles.push_back(wL);

        PlatformTile wR;
        wR.pos = glm::vec2(startTunnel.x + 0.85f, ly);
        wR.size = glm::vec2(0.6f, 1.0f);
        wR.tileX = 2; wR.tileY = 1;
        wR.isLadder = false; wR.isOneWay = false; wR.theme = 0;
        tiles.push_back(wR);

        PlatformTile lad;
        lad.pos = glm::vec2(startTunnel.x, ly);
        lad.size = glm::vec2(1.0f, 1.0f);
        lad.tileX = 9; lad.tileY = (ly >= startTunnel.maxY - 0.1f) ? 3 : 4;
        lad.isLadder = true; lad.isOneWay = false; lad.theme = 0;
        tiles.push_back(lad);
    }

    // Starter Elevated Platform
    for (int i = 0; i < 5; ++i) {
        PlatformTile plat;
        plat.pos = glm::vec2(startTunnel.x - 1.0f + (i * 1.0f), startTunnel.maxY);
        plat.size = glm::vec2(1.0f, 1.0f);
        plat.tileX = (i == 0) ? 0 : (i == 4) ? 2 : 1;
        plat.tileY = 0;
        plat.isLadder = false; plat.isOneWay = true; plat.theme = 0;
        tiles.push_back(plat);
    }

    // Starter Slope
    SteepSlope startSlope;
    startSlope.startX = startTunnel.x + 4.0f;
    startSlope.endX   = startSlope.startX + 4.0f;
    startSlope.startY = startTunnel.maxY;
    startSlope.endY   = SURFACE_GROUND_Y + 0.5f;
    startSlope.angle  = std::atan2(startSlope.endY - startSlope.startY, startSlope.endX - startSlope.startX);
    startSlope.theme  = 0;
    slopes.push_back(startSlope);
}
