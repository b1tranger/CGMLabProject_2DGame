#include "../include/Common.h"
#include "../include/World.h"
#include "../include/Audio.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

int main() {
    std::srand(42);

    // 1. Test Audio Volume Control (Default 40%, 10% steps)
    assert(audioMgr.getBgmVolumePercent() == 40);
    assert(!audioMgr.isMuted());

    audioMgr.adjustBgmVolume(10);
    assert(audioMgr.getBgmVolumePercent() == 50);

    audioMgr.adjustBgmVolume(-20);
    assert(audioMgr.getBgmVolumePercent() == 30);

    audioMgr.setBgmVolumePercent(74); // snaps to 70%
    assert(audioMgr.getBgmVolumePercent() == 70);

    audioMgr.setBgmVolumePercent(76); // snaps to 80%
    assert(audioMgr.getBgmVolumePercent() == 80);

    audioMgr.toggleBgmMute();
    assert(audioMgr.isMuted());
    assert(audioMgr.getBgmVolumePercent() == 0);
    assert(audioMgr.getSavedBgmVolumePercent() == 80);

    audioMgr.toggleBgmMute();
    assert(!audioMgr.isMuted());
    assert(audioMgr.getBgmVolumePercent() == 80);

    audioMgr.setBgmVolumePercent(0);
    assert(audioMgr.isMuted());
    assert(audioMgr.getBgmVolumePercent() == 0);

    audioMgr.toggleBgmMute(); // unmute from 0 restores 40%
    assert(!audioMgr.isMuted());
    assert(audioMgr.getBgmVolumePercent() == 40);

    // 2. Test Regular Run Scene Relics: NO DIR_LEFT or DIR_RIGHT
    gameWorld.reset(THEME_EMERALD_FOREST);
    for (int step = 0; step < 25; ++step) {
        gameWorld.updateProcedural(static_cast<float>(step * 30), THEME_EMERALD_FOREST);
        for (const auto& r : gameWorld.relics) {
            assert(r.targetDir == DIR_UP || r.targetDir == DIR_DOWN);
        }
    }

    // 3. Test 3-Phase Enemy Spawning (1m Phase 1 & Phase 2) & Phase 3 Horde Spawn
    gameWorld.reset(THEME_EMERALD_FOREST);
    assert(gameWorld.getEnemyPhase() == 1);
    assert(gameWorld.countActiveEnemies() <= 2);

    // Simulate Phase 1 (0s to 55s): moving across chunks and clearing periodically
    for (int s = 0; s < 55; ++s) {
        float px = static_cast<float>(s * 2);
        gameWorld.updateProcedural(px, THEME_EMERALD_FOREST);
        SpawnEventResult evt = gameWorld.updateEnemySpawning(1.0f, px, THEME_EMERALD_FOREST, true);
        assert(evt.newPhaseStarted == 0);
        assert(!evt.hordeTriggered);
        assert(gameWorld.getEnemyPhase() == 1);
        assert(gameWorld.countActiveEnemies() <= 2);
    }

    // Clear remaining 5s to reach Phase 2 at 60s
    int phase2Transitions = 0;
    for (int s = 0; s < 6; ++s) {
        SpawnEventResult evt = gameWorld.updateEnemySpawning(1.0f, 110.0f, THEME_EMERALD_FOREST, true);
        if (evt.newPhaseStarted == 2) phase2Transitions++;
    }
    assert(phase2Transitions == 1);
    assert(gameWorld.getEnemyPhase() == 2);

    // Advance 59s through Phase 2 (61s -> 120s) -> Phase 3
    int phase3Transitions = 0;
    int hordeCount = 0;
    for (int s = 0; s < 65; ++s) {
        // Clear enemies right before Phase 3 so we can count exact Horde Spawn additions
        if (s == 58) {
            gameWorld.enemies.clear();
        }
        SpawnEventResult evt = gameWorld.updateEnemySpawning(1.0f, 110.0f, THEME_EMERALD_FOREST, true);
        if (evt.newPhaseStarted == 3) {
            phase3Transitions++;
        }
        if (evt.hordeTriggered) {
            hordeCount++;
            int slimes = 0, bods = 0;
            for (const auto& e : gameWorld.enemies) {
                if (e.type == ENEMY_SLIME_INITIAL) slimes++;
                else if (e.type == ENEMY_BRINGER_OF_DEATH) bods++;
            }
            std::cout << "Horde #1 spawned: slimes=" << slimes << ", bods=" << bods << "\n";
            assert(slimes >= 3);
            assert(bods >= 3);
        }
    }
    assert(phase3Transitions == 1);
    assert(gameWorld.getEnemyPhase() == 3);
    assert(hordeCount == 1);

    // Verify 3-minute (180s) cooldown before second Horde Spawn
    int horde2Count = 0;
    for (int s = 0; s < 170; ++s) {
        SpawnEventResult evt = gameWorld.updateEnemySpawning(1.0f, 110.0f, THEME_EMERALD_FOREST, true);
        if (evt.hordeTriggered) horde2Count++;
    }
    assert(horde2Count == 0); // Must NOT trigger before 180s cooldown elapses

    for (int s = 0; s < 20; ++s) {
        gameWorld.enemies.clear();
        SpawnEventResult evt = gameWorld.updateEnemySpawning(1.0f, 110.0f, THEME_EMERALD_FOREST, true);
        if (evt.hordeTriggered) {
            horde2Count++;
            int slimes = 0, bods = 0;
            for (const auto& e : gameWorld.enemies) {
                if (e.type == ENEMY_SLIME_INITIAL) slimes++;
                else if (e.type == ENEMY_BRINGER_OF_DEATH) bods++;
            }
            std::cout << "Horde #2 spawned: slimes=" << slimes << ", bods=" << bods << "\n";
            assert(slimes >= 3);
            assert(bods >= 3);
        }
    }
    assert(horde2Count == 1);

    // Verify Phase 3 chunk spawning can spawn up to 5 Slimes and 5 Bringers of Death
    int maxChunkSlimes = 0, maxChunkBods = 0;
    float walkX = 200.0f;
    for (int c = 0; c < 15; ++c) {
        gameWorld.enemies.clear();
        walkX += 40.0f;
        gameWorld.updateProcedural(walkX, THEME_EMERALD_FOREST);
        int cSlimes = 0, cBods = 0;
        for (const auto& e : gameWorld.enemies) {
            if (e.type == ENEMY_SLIME_INITIAL) cSlimes++;
            else if (e.type == ENEMY_BRINGER_OF_DEATH) cBods++;
        }
        maxChunkSlimes = std::max(maxChunkSlimes, cSlimes);
        maxChunkBods = std::max(maxChunkBods, cBods);
    }
    std::cout << "Phase 3 chunk max spawned: slimes=" << maxChunkSlimes << ", bods=" << maxChunkBods << "\n";
    assert(maxChunkSlimes >= 4 && maxChunkSlimes <= 5);
    assert(maxChunkBods >= 4 && maxChunkBods <= 5);

    std::cout << "ALL VERIFICATION ASSERTIONS PASSED!\n";
    return 0;
}

