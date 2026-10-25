#pragma once

#include "Common.h"
#include <vector>

class World {
public:
    World();
    ~World() = default;

    void reset(EnvironmentTheme initialTheme);
    void updateProcedural(float playerX, EnvironmentTheme activeTheme);
    void ensureGroundAround(float targetX, EnvironmentTheme theme);

    bool checkCollision(const glm::vec2& posA, const glm::vec2& sizeA,
                        const glm::vec2& posB, const glm::vec2& sizeB) const;

    std::vector<PlatformTile>   tiles;
    std::vector<LadderTunnel>   tunnels;
    std::vector<SteepSlope>     slopes;
    std::vector<SpikeTrap>      spikes;
    std::vector<DirectionRelic> relics;
    std::vector<FruitShield>    fruits;
    std::vector<AnimatedCoin>   coins;

    float rightGenEdge;
    float leftGenEdge;

private:
    void generateChunk(float startX, float endX, bool isInitial, EnvironmentTheme theme);
};

extern World gameWorld;
