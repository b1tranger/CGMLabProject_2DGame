#pragma once

#include "Common.h"
#include <vector>

struct SpawnEventResult {
    int newPhaseStarted = 0; // 0 if no change, 2 or 3 on phase transition
    bool hordeTriggered = false;
};

class World {
public:
    World();
    ~World() = default;

    void reset(EnvironmentTheme initialTheme);
    void updateProcedural(float playerX, EnvironmentTheme activeTheme);
    void ensureGroundAround(float targetX, EnvironmentTheme theme);
    SpawnEventResult updateEnemySpawning(float deltaTime, float playerX,
                                         EnvironmentTheme activeTheme,
                                         bool allowSpawning);

    int getEnemyPhase() const { return currentEnemyPhase; }
    float getEnemyPhaseElapsed() const { return enemyPhaseTimer; }
    float getHordeCooldown() const { return hordeCooldownTimer; }
    int countActiveEnemies() const;

    bool checkCollision(const glm::vec2& posA, const glm::vec2& sizeA,
                        const glm::vec2& posB, const glm::vec2& sizeB) const;
    bool hasSolidGroundAt(float x, float y, float toleranceY = 0.5f) const;

    std::vector<PlatformTile>   tiles;
    std::vector<LadderTunnel>   tunnels;
    std::vector<SteepSlope>     slopes;
    std::vector<SpikeTrap>      spikes;
    std::vector<DirectionRelic> relics;
    std::vector<FruitShield>    fruits;
    std::vector<HeartPickup>    hearts;
    std::vector<AnimatedCoin>   coins;
    std::vector<EnemyEntity>    enemies;

    float rightGenEdge;
    float leftGenEdge;

private:
    int currentEnemyPhase;
    float enemyPhaseTimer;
    float periodicSpawnTimer;
    float hordeCooldownTimer;

    void generateChunk(float startX, float endX, bool isInitial, EnvironmentTheme theme);
    bool isNearHazardOrShaft(float x, bool checkSlopes) const;
    EnemyEntity makeEnemy(EnemyType type, const glm::vec2& pos) const;
    bool trySpawnEnemyInRange(EnemyType type, float minX, float maxX,
                              float avoidX, float avoidRadius,
                              bool relaxSeparation = false);
    void spawnHordeWave(float playerX, EnvironmentTheme activeTheme);
};

extern World gameWorld;
