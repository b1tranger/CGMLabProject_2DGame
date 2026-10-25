#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

// -----------------------------------------------------------------------------
// Window Settings
// -----------------------------------------------------------------------------
constexpr unsigned int WINDOW_WIDTH  = 1024;
constexpr unsigned int WINDOW_HEIGHT = 720;
inline const char* WINDOW_TITLE = "The Wenderer: 2D Platformer [Multi-Elevation & Cavern Traversal]";

// -----------------------------------------------------------------------------
// Physics & Multi-Elevation World Tuning (Comfortable, Balanced Speed)
// -----------------------------------------------------------------------------
constexpr float GRAVITY          = -22.0f; // Forgiving, comfortable platformer gravity
constexpr float JUMP_BURST       = 11.2f; // Responsive, enjoyable jump height
constexpr float BASE_RUN_SPEED   = 4.8f;  // Balanced, readable platformer speed
constexpr float CLIMB_SPEED      = 3.8f;  // Smooth, readable ladder climbing speed
constexpr float SLIDE_SPEED      = 7.8f;  // Thrilling but controllable downhill slope sliding
constexpr float MIN_SWITCH_TIME  = 6.0f;  // Pacing: longer intervals to enjoy each phase
constexpr float MAX_SWITCH_TIME  = 10.0f;

// 4 Distinct Vertical Elevations:
// 1. Sky / Canopy Tier:      Y = +5.2 to +6.5
// 2. Surface Elevated Tier:  Y = +1.0 to +2.8
// 3. Surface Ground Level:   Y = -0.5
// 4. Underground Cavern:     Y = -4.5
// 5. Bedrock Foundation:     Y = -7.5 (Safety catch floor: zero out-of-bounds fall deaths!)
constexpr float BEDROCK_Y        = -7.5f;
constexpr float SURFACE_GROUND_Y = -0.5f;
constexpr float CAVERN_FLOOR_Y   = -4.5f;

// -----------------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------------
enum RunDirection {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP,
    DIR_DOWN
};

enum AnimationState {
    ANIM_RUN_RIGHT,
    ANIM_RUN_LEFT,
    ANIM_CLIMB_UP,
    ANIM_CLIMB_DOWN,
    ANIM_SLIDE,
    ANIM_FALLING,
    ANIM_JUMPING,
    ANIM_IDLE
};

enum GameState {
    STATE_INTRO_VIDEO = 0,
    STATE_RUNNING     = 1,
    STATE_PAUSED      = 2,
    STATE_GAME_OVER   = 3
};
constexpr GameState STATE_PLAYING = STATE_RUNNING;

enum GameScene {
    SCENE_PLATFORMER   = 0,
    SCENE_LADDER_CLIMB = 1,
    SCENE_SKY_FALL     = 2
};


enum EnvironmentTheme {
    THEME_EMERALD_FOREST = 0,
    THEME_FROZEN_GLACIER = 1,
    THEME_SUNSET_CANYON  = 2,
    THEME_MYSTIC_CAVERN  = 3,
    THEME_COUNT          = 4
};

enum SceneryTheme {
    SCENERY_SILVERGRASS = 0,
    SCENERY_AUTUMN      = 1,
    SCENERY_FOREST      = 2,
    SCENERY_URBAN       = 3,
    SCENERY_COUNT       = 4
};

constexpr float DAY_NIGHT_CYCLE_DURATION = 90.0f; // 1.5 minutes (90 seconds) per Day/Night half-cycle
constexpr float SCENE_DIRECTION_COOLDOWN = 15.0f; // 15 seconds trial before following direction changes

enum SFXType {
    SFX_STEP,
    SFX_JUMP,
    SFX_CLIMB,
    SFX_SLIDE,
    SFX_COIN,
    SFX_RELIC,
    SFX_SHIFT,
    SFX_DEATH,
    SFX_SLASH,
    SFX_ENEMY_HIT,
    SFX_ENEMY_ATTACK,
    SFX_ENEMY_DEATH,
    SFX_SLIME_BOUNCE,
    SFX_BOSS_ROAR,
    SFX_BOSS_ATTACK,
    SFX_HEART_PICKUP,
    SFX_BLOCK,
    SFX_COUNT
};

// -----------------------------------------------------------------------------
// World Data Structures
// -----------------------------------------------------------------------------
struct PlatformTile {
    glm::vec2 pos;
    glm::vec2 size;
    int tileX;
    int tileY;
    bool isLadder;
    bool isOneWay;
    int theme;
};

struct LadderTunnel {
    float x;         // Center X
    float minY;      // Lower floor elevation
    float maxY;      // Upper platform elevation
    float width;     // Full shaftway width
    bool playerInside;
};

struct SteepSlope {
    float startX;
    float endX;
    float startY;
    float endY;
    float angle;
    int theme;
};

struct SpikeTrap {
    glm::vec2 pos;
    glm::vec2 size;
};

struct DirectionRelic {
    glm::vec2 pos;
    float animPhase;
    RunDirection targetDir;
    bool collected;
};

struct FruitShield {
    glm::vec2 pos;
    float animPhase;
    int fruitType;
    bool collected;
};

struct HeartPickup {
    glm::vec2 pos;
    float animPhase;
    bool collected;
};

struct AnimatedCoin {
    glm::vec2 pos;
    float animTimer;
    bool collected;
};

// -----------------------------------------------------------------------------
// Enemy Entities & Boss Fight Structures
// -----------------------------------------------------------------------------
enum EnemyType {
    ENEMY_BRINGER_OF_DEATH = 0,
    ENEMY_SLIME_INITIAL    = 1,
    ENEMY_BOSS_DEMON_SLIME = 2
};

enum EnemyState {
    ENEMY_STATE_IDLE,
    ENEMY_STATE_PATROL,
    ENEMY_STATE_ALERT,
    ENEMY_STATE_ATTACK,
    ENEMY_STATE_CAST,
    ENEMY_STATE_HURT,
    ENEMY_STATE_TRANSFORM,
    ENEMY_STATE_DEATH,
    ENEMY_STATE_JUMP
};

struct SpellHazard {
    glm::vec2 pos;
    float animTimer;
    bool hasDealtDamage;
    bool audioPlayed;
    bool active;
};

struct EnemyEntity {
    EnemyType type;
    EnemyState state;
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec2 vel;
    int hp;
    int maxHp;
    bool facingLeft;
    float animTimer;
    float stateTimer;
    float attackCooldown;
    float jumpCooldown;
    float spellCooldown;
    float hurtTimer;
    bool active;
    bool hasHitPlayerThisAttack;
};

struct CornerNotification {
    std::string tag;
    std::string title;
    std::string detail;
    glm::vec4 accentColor;
    float timer;
    float maxTime;
    int iconType;
    int fruitType;
};

struct CharacterAnimSet {
    unsigned int texRun     = 0;
    unsigned int texIdle    = 0;
    unsigned int texJump    = 0;
    unsigned int texFall    = 0;
    unsigned int texAttack  = 0;
    unsigned int texDeath   = 0;
    unsigned int texTakeHit = 0;
    unsigned int texDefend  = 0;

    int runFrames     = 8;
    int idleFrames    = 8;
    int jumpFrames    = 2;
    int fallFrames    = 2;
    int attackFrames  = 6;
    int deathFrames   = 6;
    int takeHitFrames = 4;
    int defendFrames  = 8;
};

extern CharacterAnimSet samuraiAnim;

// -----------------------------------------------------------------------------
// UV Atlases Calculation Helpers
// -----------------------------------------------------------------------------
inline glm::vec4 getPlayerUV(int row, int col) {
    float u  = (col * 64.0f) / 832.0f;
    float v  = (row * 64.0f) / 2944.0f;
    float du = 64.0f / 832.0f;
    float dv = 64.0f / 2944.0f;
    return glm::vec4(u, v, du, dv);
}

inline glm::vec4 getTilesetUV(int tx, int ty) {
    float u  = static_cast<float>(tx) / 16.0f;
    float v  = static_cast<float>(ty) / 16.0f;
    float du = 1.0f / 16.0f;
    float dv = 1.0f / 16.0f;
    return glm::vec4(u, v, du, dv);
}

inline glm::vec4 getCoinUV(int frame) {
    int f = (frame % 12);
    float u  = static_cast<float>(f) * (16.0f / 192.0f);
    float v  = 0.0f;
    float du = 16.0f / 192.0f;
    float dv = 1.0f;
    return glm::vec4(u, v, du, dv);
}

inline glm::vec4 getShieldUV(int frame = 0) {
    int idx = (frame % 4);
    float u  = static_cast<float>(idx) * 0.25f;
    float v  = 0.0f;
    float du = 0.25f;
    float dv = 1.0f;
    return glm::vec4(u, v, du, dv);
}

inline glm::vec4 getHeartUV(int frame = 0) {
    int idx = (frame % 4);
    float u  = static_cast<float>(idx) * 0.25f;
    float v  = 0.0f;
    float du = 0.25f;
    float dv = 1.0f;
    return glm::vec4(u, v, du, dv);
}

inline glm::vec4 getFruitUV(int fruitIndex) {
    return getShieldUV(fruitIndex);
}

inline glm::vec4 getDirectionArrowUV(RunDirection dir) {
    int idx = 0;
    if (dir == DIR_LEFT) idx = 0;
    else if (dir == DIR_RIGHT) idx = 1;
    else if (dir == DIR_UP) idx = 2;
    else if (dir == DIR_DOWN) idx = 3;
    float u  = static_cast<float>(idx) * 0.25f;
    float v  = 0.0f;
    float du = 0.25f;
    float dv = 1.0f;
    return glm::vec4(u, v, du, dv);
}
