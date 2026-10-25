/* =========================================================================
 * The Wenderer: 2D Side-View Endless Runner & Platformer
 * Developed for Computer Graphics & Multimedia Lab (CSE0611328)
 *
 * PHASE 3.2: Modular Architecture, Ladder Shaft Auto-Latching & Stopping,
 *            Unblocked Cavern Chutes, Midpoint Interrupts & Balanced Pacing
 * ========================================================================= */

#include "Common.h"
#include "Audio.h"
#include "Renderer.h"
#include "World.h"
#include "VideoPlayer.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

// -----------------------------------------------------------------------------
// Global Game State
// -----------------------------------------------------------------------------
GameState currentGameState = STATE_INTRO_VIDEO;
std::string gameOverReason = "";

// Character Animation Set Definition
struct CharacterAnimSet {
    unsigned int texRun     = 0;
    unsigned int texIdle    = 0;
    unsigned int texJump    = 0;
    unsigned int texFall    = 0;
    unsigned int texAttack  = 0;
    unsigned int texDeath   = 0;
    unsigned int texTakeHit = 0;

    int runFrames     = 8;
    int idleFrames    = 8;
    int jumpFrames    = 2;
    int fallFrames    = 2;
    int attackFrames  = 6;
    int deathFrames   = 6;
    int takeHitFrames = 4;
};

// Global Character Set (Samurai Mack)
CharacterAnimSet samuraiAnim;
float deathAnimTimer = 0.0f;

// Temporary Shield & Idle-Start State
float shieldTimer        = 0.0f; // Temporary shield duration gained from fruit pickups
float postHitInvulnTimer = 0.0f; // Brief post-shield break invulnerability to safely clear the hazard
bool isGameIdleAtStart   = true; // Character stands still at start until player moves
bool freeExplorationMode = false; // Toggle with 'F' key: allows full manual exploration without auto-run

// World & Object Textures
unsigned int worldTilesetTexture     = 0;
unsigned int platformsTexture        = 0;
unsigned int coinTexture             = 0;
unsigned int fruitTexture            = 0;
unsigned int birdTexture             = 0;
unsigned int directionArrowsTexture = 0;

// Player State
glm::vec2 playerPos(0.0f, SURFACE_GROUND_Y + 1.2f);
glm::vec2 playerVel(0.0f, 0.0f);
glm::vec2 playerHitbox(0.85f, 1.45f);
glm::vec2 playerRenderSize(5.5f, 5.5f);
float playerSpriteAngle = 0.0f;

bool isGrounded  = false;
bool isOnLadder  = false;
bool isSliding   = false;
float slideInterruptCooldown = 0.0f;

// Dash Movement System (Activated via SHIFT)
bool isDashing              = false;
float dashTimer             = 0.0f;
float dashCooldown          = 0.0f;
constexpr float DASH_BURST_TIME  = 0.22f;
constexpr float DASH_COOLDOWN_TIME = 0.65f;
constexpr float DASH_SPEED_MAG    = 15.0f;
struct DashGhost {
    glm::vec2 pos;
    float alpha;
    bool faceLeft;
    int frameIndex;
};
std::vector<DashGhost> dashGhosts;

// -----------------------------------------------------------------------------
// Dedicated Up/Down Mini-Game Scenes System
// -----------------------------------------------------------------------------
GameScene currentScene = SCENE_PLATFORMER;
glm::vec2 platformerSavedPos(0.0f, SURFACE_GROUND_Y + 1.2f);
RunDirection platformerSavedDir = DIR_RIGHT;

// Up Scene: 3 Parallel Endless Ladders
constexpr float LADDER_TRACK_X[3] = { -3.2f, 0.0f, 3.2f };
int ladderCurrentLane     = 1; // 0=Left, 1=Center, 2=Right
int ladderTargetLane      = 1;
float ladderPlayerY       = 0.0f;
float ladderPlayerX       = 0.0f;
float ladderClimbSpeed    = 5.4f;
bool isLadderLeaping      = false;
float ladderLeapProgress  = 1.0f;
constexpr float LADDER_LEAP_TIME = 0.20f;
bool isLadderHit          = false;
float ladderHitVelY       = 0.0f;
float ladderDarkAlpha     = 0.0f;
bool isLadderResurrecting = false;
float ladderResurrectTimer = 0.0f;

struct LadderTrap {
    int lane;
    float y;
    int type; // 0=spikes, 1=sawblade
    bool active;
};
std::vector<LadderTrap> ladderTraps;

struct LadderItem {
    int lane;
    float y;
    int type; // 0=coin, 1=relic
    int fruitType;
    bool collected;
};
std::vector<LadderItem> ladderItems;
float ladderGenTopY = 8.0f;

// Down Scene: Sky-Fall with Birds & Traps
glm::vec2 skyFallPos(0.0f, 0.0f);
float skyFallVelX       = 0.0f;
float skyFallSpeed      = 9.2f;
bool isSkyFallHit       = false;
float skyFallHitTimer   = 0.0f;
float skyFallDarkAlpha  = 0.0f;

struct SkyBirdObstacle {
    glm::vec2 pos;
    float vx;
    float animTimer;
    bool active;
};
std::vector<SkyBirdObstacle> skyBirdsList;

struct SkySpikeTrap {
    glm::vec2 pos;
    glm::vec2 size;
    bool active;
};
std::vector<SkySpikeTrap> skySpikesList;

struct SkyItem {
    glm::vec2 pos;
    int type; // 0=coin, 1=relic
    int fruitType;
    bool collected;
};
std::vector<SkyItem> skyItemsList;
float skyFallGenBottomY = -8.0f;

float playerSpeed = BASE_RUN_SPEED;
float scoreDistance = 0.0f;
int coinsCollected  = 0;
int relicsTriggered = 0;
int directionShiftsCount = 0;

// Traversal Direction Management with Horizontal Memory
RunDirection currentDirection  = DIR_RIGHT;
RunDirection lastHorizontalDir = DIR_RIGHT;
float directionSwitchTimer     = 6.5f;
// By default, random timer shifts are disabled so direction shifts ONLY happen when collecting Mystery Fruit Relics!
bool autoShiftEnabled          = false;

// World-Space Floating Text System
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
std::vector<FloatingText> floatingTexts;

void spawnFloatingText(const std::string& text, const glm::vec2& pos, const glm::vec4& color,
                       float charW = 0.26f, float charH = 0.38f, float duration = 2.2f, float riseSpeed = 1.3f)
{
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

// Dynamic Environment & Biome State
EnvironmentTheme activeTheme = THEME_EMERALD_FOREST;
glm::vec4 curSkyColor(0.08f, 0.10f, 0.16f, 1.0f);
glm::vec4 targetSkyColor(0.08f, 0.10f, 0.16f, 1.0f);
glm::vec4 curPillarColor(0.12f, 0.16f, 0.22f, 0.5f);
glm::vec4 targetPillarColor(0.12f, 0.16f, 0.22f, 0.5f);
glm::vec4 curTileTint(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 targetTileTint(1.0f, 1.0f, 1.0f, 1.0f);

float transitionPhaseTimer = 0.0f;
float transitionPhaseMax   = 0.75f;

// -----------------------------------------------------------------------------
// Background Scenery Assets & Dynamic Day/Night Cycle State
// -----------------------------------------------------------------------------
SceneryTheme activeScenery = SCENERY_SILVERGRASS;
unsigned int texScenery[SCENERY_COUNT] = { 0, 0, 0, 0 };

// Day/Night Cycle: changes every 1.5 min (90s) of playtime, initial scene is Night!
float dayNightTimer = 0.0f;
int lastCycleIndex = 0;
float daylightFactor = 0.0f; // 0.0 = deep night, 1.0 = bright daylight

// Scene Cooldown: Following direction changes occur only after 15s of playing that scene
float scenePlayTime = 0.0f;
bool sceneDirectionUnlockedNotified = false;

// Celestial Stars for Night Sky
struct CelestialStar {
    float x;
    float y;
    float size;
    float twinkleFreq;
    float twinklePhase;
};
std::vector<CelestialStar> skyStars;

// Sprite Animation Management
AnimationState currentAnim = ANIM_RUN_RIGHT;
float animTimer = 0.0f;
int currentAnimFrame = 0;

// Side-View Camera
glm::vec2 cameraPos(0.0f, 0.0f);

// Corner Notification System
CornerNotification activeNotice = {"", "", "", glm::vec4(0), 0.0f, 0.0f, 0, 0};

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);
void resetGame();
void triggerRandomDirectionShift(const std::string& sourceName, RunDirection forcedDir = (RunDirection)-1);
void setEnvironmentTheme(EnvironmentTheme theme);
void setSceneryTheme(SceneryTheme theme);
std::string getSceneryName(SceneryTheme s);
void showCornerNotification(const std::string& tag, const std::string& title,
                            const std::string& detail, const glm::vec4& color,
                            int iconType, int fruitType = 0);

// -----------------------------------------------------------------------------
// Corner HUD Notification Trigger
// -----------------------------------------------------------------------------
void showCornerNotification(const std::string& tag, const std::string& title,
                            const std::string& detail, const glm::vec4& color,
                            int iconType, int fruitType)
{
    activeNotice.tag = tag;
    activeNotice.title = title;
    activeNotice.detail = detail;
    activeNotice.accentColor = color;
    activeNotice.timer = 2.8f;
    activeNotice.maxTime = 2.8f;
    activeNotice.iconType = iconType;
    activeNotice.fruitType = fruitType;
}

// -----------------------------------------------------------------------------
// Environment Theme & Biome Transitions
// -----------------------------------------------------------------------------
void setEnvironmentTheme(EnvironmentTheme theme)
{
    activeTheme = theme;
    switch (theme) {
        case THEME_EMERALD_FOREST:
            activeScenery     = SCENERY_FOREST;
            targetSkyColor    = glm::vec4(0.08f, 0.11f, 0.17f, 1.0f);
            targetPillarColor = glm::vec4(0.12f, 0.17f, 0.23f, 0.55f);
            targetTileTint    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        case THEME_FROZEN_GLACIER:
            activeScenery     = SCENERY_SILVERGRASS;
            targetSkyColor    = glm::vec4(0.06f, 0.15f, 0.25f, 1.0f);
            targetPillarColor = glm::vec4(0.18f, 0.28f, 0.38f, 0.55f);
            targetTileTint    = glm::vec4(0.85f, 0.95f, 1.08f, 1.0f);
            break;
        case THEME_SUNSET_CANYON:
            activeScenery     = SCENERY_AUTUMN;
            targetSkyColor    = glm::vec4(0.22f, 0.08f, 0.12f, 1.0f);
            targetPillarColor = glm::vec4(0.32f, 0.14f, 0.18f, 0.55f);
            targetTileTint    = glm::vec4(1.08f, 0.92f, 0.85f, 1.0f);
            break;
        case THEME_MYSTIC_CAVERN:
            activeScenery     = SCENERY_URBAN;
            targetSkyColor    = glm::vec4(0.09f, 0.05f, 0.15f, 1.0f);
            targetPillarColor = glm::vec4(0.20f, 0.12f, 0.28f, 0.55f);
            targetTileTint    = glm::vec4(0.92f, 0.84f, 1.0f, 1.0f);
            break;
        default:
            break;
    }
}

std::string getSceneryName(SceneryTheme s) {
    switch (s) {
        case SCENERY_SILVERGRASS: return "SILVERGRASS FIELD";
        case SCENERY_AUTUMN:      return "AUTUMN FOLIAGE";
        case SCENERY_FOREST:      return "GREEN FOREST FOLIAGE";
        case SCENERY_URBAN:       return "URBAN AREA";
        default:                  return "UNKNOWN";
    }
}

void setSceneryTheme(SceneryTheme s) {
    activeScenery = s;
    std::string name = getSceneryName(s);
    glm::vec4 accent(1.0f);
    if (s == SCENERY_SILVERGRASS) accent = glm::vec4(0.85f, 0.9f, 0.75f, 1.0f);
    else if (s == SCENERY_AUTUMN) accent = glm::vec4(1.0f, 0.55f, 0.2f, 1.0f);
    else if (s == SCENERY_FOREST) accent = glm::vec4(0.3f, 0.95f, 0.45f, 1.0f);
    else if (s == SCENERY_URBAN)  accent = glm::vec4(0.4f, 0.8f, 1.0f, 1.0f);
    showCornerNotification("SCENERY", name, "Backdrop scenery shifted!", accent, 1, 0);
    spawnFloatingText("SCENERY: " + name, (currentScene == SCENE_PLATFORMER ? playerPos + glm::vec2(0.0f, 1.2f) : glm::vec2(0.0f, 1.5f)), accent);
}

// -----------------------------------------------------------------------------
// Scene Transitions: Up Ladder Climb, Down Sky-Fall, and Platformer Return
// -----------------------------------------------------------------------------
void enterLadderScene() {
    currentScene = SCENE_LADDER_CLIMB;
    platformerSavedPos = playerPos;
    platformerSavedDir = lastHorizontalDir;
    ladderCurrentLane = 1;
    ladderTargetLane = 1;
    ladderPlayerY = 0.0f;
    ladderPlayerX = LADDER_TRACK_X[1];
    isLadderLeaping = false;
    ladderLeapProgress = 1.0f;
    isLadderHit = false;
    ladderHitVelY = 0.0f;
    ladderDarkAlpha = 0.0f;
    isLadderResurrecting = false;
    ladderResurrectTimer = 0.0f;
    ladderTraps.clear();
    ladderItems.clear();
    ladderGenTopY = 5.0f;
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;

    // Generate initial traps and collectibles across the 3 endless ladders (initial ~25m)
    for (int i = 0; i < 6; ++i) {
        float y = 5.0f + i * 4.2f;
        int trapLane = std::rand() % 3;
        ladderTraps.push_back({ trapLane, y, std::rand() % 2, true });

        int itemLane = (trapLane + 1 + (std::rand() % 2)) % 3;
        if (i % 2 == 0) {
            ladderItems.push_back({ itemLane, y + 1.2f, 1, std::rand() % 4, false }); // Fruit shield
        } else {
            ladderItems.push_back({ itemLane, y + 1.2f, 0, 0, false }); // Coin
        }
        ladderGenTopY = y;
    }

    showCornerNotification("SCENE ENTER", "^^^ 3-LADDER ASCENT ^^^", "Survive 15s trial before direction shifts!", glm::vec4(0.35f, 0.95f, 0.4f, 1.0f), 1, 2);
    spawnFloatingText("^^^ 3-LADDER TRIAL (15s) ^^^", glm::vec2(0.0f, 1.2f), glm::vec4(0.35f, 1.0f, 0.45f, 1.0f));
}

void enterSkyFallScene() {
    currentScene = SCENE_SKY_FALL;
    platformerSavedPos = playerPos;
    platformerSavedDir = lastHorizontalDir;
    skyFallPos = glm::vec2(0.0f, 0.0f);
    skyFallVelX = 0.0f;
    isSkyFallHit = false;
    skyFallHitTimer = 0.0f;
    skyFallDarkAlpha = 0.0f;
    skyBirdsList.clear();
    skySpikesList.clear();
    skyItemsList.clear();
    skyFallGenBottomY = -8.0f;
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;

    // Generate initial birds, spike mines, and items with spacious, fair pacing
    for (int i = 0; i < 4; ++i) {
        float y = -8.0f - i * 14.0f;

        // Birds spawn on alternate tiers with moderate, readable speed
        if (i % 2 == 0) {
            bool fromLeft = (i % 4 == 0);
            float birdX = fromLeft ? -9.5f : 9.5f;
            float birdVx = fromLeft ? (3.0f + (std::rand() % 15) / 10.0f) : -(3.0f + (std::rand() % 15) / 10.0f);
            skyBirdsList.push_back({ glm::vec2(birdX, y), birdVx, static_cast<float>(std::rand() % 10), true });
        }

        // Spike traps spawn rarely (every 3 tiers) leaving wide open escape lanes
        if (i % 3 == 1) {
            float trapX = ((std::rand() % 60) / 10.0f) - 3.0f;
            skySpikesList.push_back({ glm::vec2(trapX, y - 3.0f), glm::vec2(0.85f, 0.85f), true });
        }

        // Spawn Gold Coins & Fruit Shield pickups
        float itemX = ((std::rand() % 80) / 10.0f) - 4.0f;
        if (i % 2 == 0) {
            skyItemsList.push_back({ glm::vec2(itemX, y - 1.5f), 1, std::rand() % 4, false }); // Fruit pickup -> grants shield!
        } else {
            skyItemsList.push_back({ glm::vec2(itemX, y - 1.5f), 0, 0, false }); // Coin
        }
        skyFallGenBottomY = y;
    }

    showCornerNotification("SCENE ENTER", "vvv SKY FALL DIVE vvv", "Survive 15s trial! Steer A/D, SHIFT to Dash!", glm::vec4(0.95f, 0.85f, 0.2f, 1.0f), 1, 0);
    spawnFloatingText("vvv SKY-FALL TRIAL (15s) vvv", glm::vec2(0.0f, 1.2f), glm::vec4(0.95f, 0.85f, 0.25f, 1.0f));
}

void returnToPlatformerScene(bool fromJump) {
    currentScene = SCENE_PLATFORMER;
    // Guarantee that left and right chunks around saved return point are generated
    gameWorld.ensureGroundAround(platformerSavedPos.x, activeTheme);

    float dirSign = (platformerSavedDir == DIR_LEFT) ? -1.0f : 1.0f;
    currentDirection = platformerSavedDir;
    lastHorizontalDir = platformerSavedDir;

    // Search for the highest solid ground/platform tile directly beneath platformerSavedPos.x
    float bestTopY = -999.0f;
    bool foundSolid = false;
    for (const auto& t : gameWorld.tiles) {
        if (!t.isLadder && std::abs(t.pos.x - platformerSavedPos.x) <= (t.size.x * 0.5f + 0.35f)) {
            float topY = t.pos.y + (t.size.y * 0.5f);
            if (topY <= platformerSavedPos.y + 0.8f && topY > bestTopY) {
                bestTopY = topY;
                foundSolid = true;
            }
        }
    }

    if (foundSolid) {
        playerPos = glm::vec2(platformerSavedPos.x, bestTopY + (playerHitbox.y * 0.5f));
        isGrounded = !fromJump;
        playerVel = glm::vec2(dirSign * BASE_RUN_SPEED, fromJump ? JUMP_BURST * 0.75f : 0.0f);
    } else {
        // Over a chasm or slope: keep in air with gravity active so the player falls safely, NEVER floating on air!
        playerPos = glm::vec2(platformerSavedPos.x, platformerSavedPos.y);
        isGrounded = false;
        playerVel = glm::vec2(dirSign * BASE_RUN_SPEED, fromJump ? JUMP_BURST * 0.75f : -1.5f);
    }

    isGameIdleAtStart = false;
    isOnLadder = false;
    isSliding = false;
    playerSpriteAngle = 0.0f;
    audioMgr.play(fromJump ? SFX_JUMP : SFX_STEP);
    spawnFloatingText("SURVIVED! BACK TO RUNNER!", playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(0.3f, 1.0f, 0.85f, 1.0f));
    showCornerNotification("RETURN", "BACK ON PLATFORM", "Resuming infinite sprint!", glm::vec4(0.2f, 0.9f, 1.0f, 1.0f), 0);
    gameWorld.updateProcedural(playerPos.x, activeTheme);
}

// -----------------------------------------------------------------------------
// Dynamic Direction Shift
// -----------------------------------------------------------------------------
void triggerRandomDirectionShift(const std::string& sourceName, RunDirection forcedDir)
{
    directionShiftsCount++;
    RunDirection newDir = forcedDir;

    if ((int)forcedDir < 0) {
        int roll = std::rand() % 4;
        if (roll == 0) newDir = DIR_RIGHT;
        else if (roll == 1) newDir = DIR_LEFT;
        else if (roll == 2) newDir = DIR_UP;
        else newDir = DIR_DOWN;

        if (newDir == currentDirection) {
            newDir = (currentDirection == DIR_RIGHT) ? DIR_LEFT : DIR_RIGHT;
        }
    }

    currentDirection = newDir;
    if (newDir == DIR_RIGHT || newDir == DIR_LEFT) {
        lastHorizontalDir = newDir;
    }

    float rNorm = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    directionSwitchTimer = MIN_SWITCH_TIME + rNorm * (MAX_SWITCH_TIME - MIN_SWITCH_TIME);

    // Switch Biome
    EnvironmentTheme nextTheme = static_cast<EnvironmentTheme>((activeTheme + 1) % THEME_COUNT);
    setEnvironmentTheme(nextTheme);

    audioMgr.play(SFX_SHIFT);
    transitionPhaseTimer = transitionPhaseMax;

    // Check Scene Triggering for Up and Down Directions!
    if (currentDirection == DIR_UP) {
        enterLadderScene();
        return;
    } else if (currentDirection == DIR_DOWN) {
        enterSkyFallScene();
        return;
    } else {
        if (currentScene != SCENE_PLATFORMER) {
            returnToPlatformerScene(true);
            return;
        }
    }

    std::string dirTitle = "";
    std::string dirDesc  = "";
    glm::vec4 color(0.2f, 0.85f, 1.0f, 1.0f);

    if (currentDirection == DIR_RIGHT) {
        dirTitle = "FORWARD RUSH >>>";
        dirDesc  = "Sprinting Rightward through new territory!";
        color    = glm::vec4(0.1f, 0.85f, 1.0f, 1.0f);
    } else if (currentDirection == DIR_LEFT) {
        dirTitle = "<<< REVERSE DASH";
        dirDesc  = "Direction Inverted! Backtracking Leftward!";
        color    = glm::vec4(1.0f, 0.55f, 0.15f, 1.0f);
    }

    showCornerNotification(sourceName, dirTitle, dirDesc, color, 1, std::rand() % 4);
}

// -----------------------------------------------------------------------------
// Reset Game
// -----------------------------------------------------------------------------
void resetGame()
{
    isGameIdleAtStart = true;
    playerPos = glm::vec2(0.0f, SURFACE_GROUND_Y + 1.2f);
    playerVel = glm::vec2(0.0f, 0.0f);
    isGrounded = true;
    isOnLadder = false;
    isSliding  = false;
    slideInterruptCooldown = 0.0f;
    playerSpriteAngle = 0.0f;
    playerSpeed = BASE_RUN_SPEED;
    scoreDistance = 0.0f;
    coinsCollected = 0;
    relicsTriggered = 0;
    directionShiftsCount = 0;
    gameOverReason = "";
    shieldTimer = 0.0f;
    postHitInvulnTimer = 0.0f;
    freeExplorationMode = false;

    currentDirection  = DIR_RIGHT;
    lastHorizontalDir = DIR_RIGHT;
    directionSwitchTimer = 6.5f;

    currentAnim = ANIM_IDLE;
    animTimer = 0.0f;
    currentAnimFrame = 0;
    deathAnimTimer = 0.0f;

    cameraPos = glm::vec2(0.0f, 0.0f);
    floatingTexts.clear();

    dayNightTimer = 0.0f; // Initial scene will be night time
    lastCycleIndex = 0;
    daylightFactor = 0.0f;
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;

    setEnvironmentTheme(THEME_EMERALD_FOREST);
    gameWorld.reset(THEME_EMERALD_FOREST);

    showCornerNotification("START", "THE WENDERER", "Traverse Sky Canopy, Surface & Caverns!",
                           glm::vec4(0.2f, 0.85f, 1.0f, 1.0f), 0);

    currentGameState = STATE_RUNNING;
}

// -----------------------------------------------------------------------------
// Main Entry Point
// -----------------------------------------------------------------------------
int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int initFbW = 0, initFbH = 0;
    glfwGetFramebufferSize(window, &initFbW, &initFbH);
    glViewport(0, 0, initFbW, initFbH);

    audioMgr.init();
    if (!gameRenderer.init()) {
        std::cerr << "Failed to initialize Game Renderer" << std::endl;
        return -1;
    }

    // Load Character Textures (Samurai Mack)
    samuraiAnim.texRun    = gameRenderer.loadTexture("assets/samurai/Run.png");
    samuraiAnim.texIdle   = gameRenderer.loadTexture("assets/samurai/Idle.png");
    samuraiAnim.texJump   = gameRenderer.loadTexture("assets/samurai/Jump.png");
    samuraiAnim.texFall   = gameRenderer.loadTexture("assets/samurai/Fall.png");
    samuraiAnim.texAttack = gameRenderer.loadTexture("assets/samurai/Attack1.png");
    samuraiAnim.texDeath   = gameRenderer.loadTexture("assets/samurai/Death.png");
    samuraiAnim.texTakeHit = gameRenderer.loadTexture("assets/samurai/Take Hit.png");
    samuraiAnim.runFrames     = 8;
    samuraiAnim.idleFrames    = 8;
    samuraiAnim.jumpFrames    = 2;
    samuraiAnim.fallFrames    = 2;
    samuraiAnim.attackFrames  = 6;
    samuraiAnim.deathFrames   = 6;
    samuraiAnim.takeHitFrames = 4;

    worldTilesetTexture    = gameRenderer.loadTexture("assets/world_tileset.png");
    platformsTexture       = gameRenderer.loadTexture("assets/platforms.png");
    coinTexture            = gameRenderer.loadTexture("assets/coin.png");
    fruitTexture           = gameRenderer.loadTexture("assets/fruit.png");
    birdTexture            = gameRenderer.loadTexture("assets/bird.png");
    directionArrowsTexture = gameRenderer.loadTexture("assets/direction_arrows.png");

    // Load 4 Thematic Background Scenery Textures
    texScenery[SCENERY_SILVERGRASS] = gameRenderer.loadTexture("assets/scenery_silvergrass.png");
    texScenery[SCENERY_AUTUMN]      = gameRenderer.loadTexture("assets/scenery_autumn.png");
    texScenery[SCENERY_FOREST]      = gameRenderer.loadTexture("assets/scenery_forest.png");
    texScenery[SCENERY_URBAN]       = gameRenderer.loadTexture("assets/scenery_urban.png");

    // Initialize Celestial Stars for Night Sky
    skyStars.clear();
    for (int i = 0; i < 64; ++i) {
        CelestialStar star;
        star.x = -16.0f + static_cast<float>(std::rand() % 320) / 10.0f;
        star.y = 1.0f + static_cast<float>(std::rand() % 70) / 10.0f;
        star.size = 0.08f + static_cast<float>(std::rand() % 14) / 100.0f;
        star.twinkleFreq = 1.8f + static_cast<float>(std::rand() % 30) / 10.0f;
        star.twinklePhase = static_cast<float>(std::rand() % 628) / 100.0f;
        skyStars.push_back(star);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    resetGame();

    // Initialize & Start Intro Video Player
    currentGameState = STATE_INTRO_VIDEO;
    if (!introVideoPlayer.open(L"assets/introVid.mp4")) {
        std::cerr << "Notice: Intro video not found or failed to load. Skipping intro." << std::endl;
        currentGameState = STATE_RUNNING;
        isGameIdleAtStart = true;
    }

    float lastFrameTime = static_cast<float>(glfwGetTime());
    float titleUpdateTimer = 0.0f;
    int frameCount = 0;

    // -------------------------------------------------------------------------
    // Main Game Loop
    // -------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float rawDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (rawDeltaTime > 0.06f) rawDeltaTime = 0.06f;

        float deltaTime = (currentGameState == STATE_PAUSED) ? 0.0f : rawDeltaTime;
        if (transitionPhaseTimer > 0.0f && currentGameState == STATE_RUNNING) {
            transitionPhaseTimer -= rawDeltaTime;
            if (transitionPhaseTimer > transitionPhaseMax * 0.5f) {
                deltaTime *= 0.5f;
            }
        }
        if (currentGameState == STATE_GAME_OVER) {
            deathAnimTimer += rawDeltaTime * 7.0f;
        }

        // ---------------------------------------------------------------------
        // Day / Night Cycle Simulation (1.5 min = 90.0s per phase, starts at Night)
        // ---------------------------------------------------------------------
        if (currentGameState == STATE_RUNNING) {
            dayNightTimer += deltaTime;
            if (shieldTimer > 0.0f) {
                shieldTimer -= deltaTime;
                if (shieldTimer < 0.0f) shieldTimer = 0.0f;
            }
            if (postHitInvulnTimer > 0.0f) {
                postHitInvulnTimer -= deltaTime;
                if (postHitInvulnTimer < 0.0f) postHitInvulnTimer = 0.0f;
            }
        }

        int cycleIndex = static_cast<int>(dayNightTimer / DAY_NIGHT_CYCLE_DURATION);
        bool isNight = (cycleIndex % 2 == 0); // 0 = Night (initial), 1 = Day, 2 = Night, ...
        float phaseTime = std::fmod(dayNightTimer, DAY_NIGHT_CYCLE_DURATION);

        // 15-second smooth twilight blend between phases
        if (isNight) {
            if (phaseTime < 75.0f) {
                daylightFactor = 0.0f; // Pure Night
            } else {
                float t = (phaseTime - 75.0f) / 15.0f;
                daylightFactor = t; // Dawn transition: 0.0 -> 1.0
            }
        } else {
            if (phaseTime < 75.0f) {
                daylightFactor = 1.0f; // Pure Day
            } else {
                float t = (phaseTime - 75.0f) / 15.0f;
                daylightFactor = 1.0f - t; // Dusk transition: 1.0 -> 0.0
            }
        }

        // Notification on phase switch
        if (cycleIndex != lastCycleIndex) {
            lastCycleIndex = cycleIndex;
            if (isNight) {
                showCornerNotification("CYCLE", "=== NIGHT HAS FALLEN ===", "Moonlight cloaks the landscape! (1.5m cycle)", glm::vec4(0.45f, 0.65f, 1.0f, 1.0f), 0);
                spawnFloatingText("=== NIGHT HAS FALLEN (1.5m) ===", (currentScene == SCENE_PLATFORMER ? playerPos + glm::vec2(0.0f, 1.5f) : glm::vec2(0.0f, 2.0f)), glm::vec4(0.5f, 0.75f, 1.0f, 1.0f));
            } else {
                showCornerNotification("CYCLE", "=== DAYBREAK HAS DAWNED ===", "Warm sunlight illuminates the horizon! (1.5m cycle)", glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), 0);
                spawnFloatingText("=== DAYBREAK HAS DAWNED (1.5m) ===", (currentScene == SCENE_PLATFORMER ? playerPos + glm::vec2(0.0f, 1.5f) : glm::vec2(0.0f, 2.0f)), glm::vec4(1.0f, 0.9f, 0.35f, 1.0f));
            }
        }

        // Environment Atmospheric Interpolation
        curSkyColor    = glm::mix(curSkyColor, targetSkyColor, rawDeltaTime * 2.5f);
        curPillarColor = glm::mix(curPillarColor, targetPillarColor, rawDeltaTime * 2.5f);
        curTileTint    = glm::mix(curTileTint, targetTileTint, rawDeltaTime * 2.5f);

        // Window Title Stats
        frameCount++;
        titleUpdateTimer += rawDeltaTime;
        if (titleUpdateTimer >= 0.25f) {
            float fps = frameCount / titleUpdateTimer;
            std::string dirStr = (currentScene == SCENE_LADDER_CLIMB) ? "^^^ 3-LADDER CLIMB" :
                                 (currentScene == SCENE_SKY_FALL)     ? "vvv SKY-FALL DIVE" :
                                 (currentDirection == DIR_RIGHT)      ? "RIGHT >>>" :
                                 (currentDirection == DIR_LEFT)       ? "<<< LEFT"  :
                                 (currentDirection == DIR_UP)         ? "^^^ UP" : "vvv DOWN";
            std::string zoneStr = (currentScene == SCENE_LADDER_CLIMB) ? "3-LADDER ASCENT" :
                                  (currentScene == SCENE_SKY_FALL)     ? "SKY-FALL DIVE" :
                                  (playerPos.y > 4.5f)  ? "SKY CANOPY" :
                                  (playerPos.y > 0.5f)  ? "SURFACE PLATFORMS" :
                                  (playerPos.y > -2.0f) ? "SURFACE GROUND" : "UNDERGROUND CAVERN";
            std::string dashStatus = (dashCooldown <= 0.0f) ? "DASH:READY [SHIFT]" : "DASH:CD";
            std::string modeStr = freeExplorationMode ? "MODE:FREE EXPLORE [F]" : "MODE:AUTO-RUN [F]";
            std::string stateStr = (currentGameState == STATE_GAME_OVER) ? "GAME OVER: " + gameOverReason + " (SPACE)" :
                                   (currentGameState == STATE_PAUSED)    ? "PAUSED (ESC / SPACE to Resume)" : "RUNNING";
            std::string timePhaseStr = (isNight ? "NIGHT (" : "DAY (") + std::to_string(static_cast<int>(DAY_NIGHT_CYCLE_DURATION - phaseTime)) + "s)";
            std::string sceneryStr = getSceneryName(activeScenery);
            std::string title = "The Wenderer | " + sceneryStr + " | " + timePhaseStr +
                                " | Zone: " + zoneStr +
                                " | " + modeStr +
                                " | Dist: " + std::to_string(static_cast<int>(scoreDistance)) +
                                "m | Coins: " + std::to_string(coinsCollected) +
                                " | Relics: " + std::to_string(relicsTriggered) +
                                " | Dir: " + dirStr + " | " + dashStatus +
                                " | FPS: " + std::to_string(static_cast<int>(fps)) +
                                " | " + stateStr;
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            titleUpdateTimer = 0.0f;
        }

        processInput(window, deltaTime);

        // ---------------------------------------------------------------------
        // Intro Video Playback & Cinematic Rendering
        // ---------------------------------------------------------------------
        if (currentGameState == STATE_INTRO_VIDEO) {
            introVideoPlayer.update(rawDeltaTime);
            if (introVideoPlayer.hasFinished()) {
                introVideoPlayer.close();
                currentGameState = STATE_RUNNING;
                isGameIdleAtStart = true;
            } else {
                glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(gameRenderer.getProgram());

                int curFbW = 0, curFbH = 0;
                glfwGetFramebufferSize(window, &curFbW, &curFbH);
                float aspect = (curFbH > 0) ? (static_cast<float>(curFbW) / static_cast<float>(curFbH)) :
                               (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
                float halfH  = 6.8f;
                float halfW  = halfH * aspect;
                glm::mat4 identityView = glm::mat4(1.0f);
                glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

                // 1. Render Video Frame (Fit aspect ratio 16:9 within viewport)
                unsigned int videoTex = introVideoPlayer.getTextureID();
                if (videoTex != 0) {
                    float vW = static_cast<float>(introVideoPlayer.getWidth());
                    float vH = static_cast<float>(introVideoPlayer.getHeight());
                    float videoAspect = (vH > 0.0f) ? (vW / vH) : (16.0f / 9.0f);

                    float quadW = halfW * 2.0f;
                    float quadH = quadW / videoAspect;
                    if (quadH > halfH * 2.0f) {
                        quadH = halfH * 2.0f;
                        quadW = quadH * videoAspect;
                    }

                    glm::mat4 modelVideo = glm::mat4(1.0f);
                    modelVideo = glm::scale(modelVideo, glm::vec3(quadW, quadH, 1.0f));
                    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
                    // Pass discardAlpha = false so opaque video frames are never clipped
                    gameRenderer.drawQuad(modelVideo, identityView, orthoUI, glm::vec4(1.0f), true, uvRect, videoTex, false);
                }

                // 2. Cinematic Progress Bar at bottom
                float barW = halfW * 1.7f;
                float barH = 0.07f;
                float barY = -halfH + 0.65f;
                // Bar Background
                glm::mat4 modelBarBg = glm::mat4(1.0f);
                modelBarBg = glm::translate(modelBarBg, glm::vec3(0.0f, barY, 0.0f));
                modelBarBg = glm::scale(modelBarBg, glm::vec3(barW, barH, 1.0f));
                gameRenderer.drawQuad(modelBarBg, identityView, orthoUI, glm::vec4(0.18f, 0.22f, 0.30f, 0.75f), false, glm::vec4(0), 0);

                // Bar Fill
                float fillFraction = introVideoPlayer.getProgress();
                float fillW = barW * fillFraction;
                if (fillW > 0.01f) {
                    glm::mat4 modelBarFill = glm::mat4(1.0f);
                    modelBarFill = glm::translate(modelBarFill, glm::vec3(-barW * 0.5f + fillW * 0.5f, barY, 0.0f));
                    modelBarFill = glm::scale(modelBarFill, glm::vec3(fillW, barH * 1.3f, 1.0f));
                    gameRenderer.drawQuad(modelBarFill, identityView, orthoUI, glm::vec4(0.2f, 0.85f, 1.0f, 0.95f), false, glm::vec4(0), 0);
                }

                // 3. Skip Prompt with subtle pulsing animation
                float pulse = 0.65f + 0.35f * std::sin(static_cast<float>(glfwGetTime()) * 4.0f);
                glm::vec4 promptColor(1.0f, 1.0f, 1.0f, pulse);
                std::string skipText = "[SPACE / ESC / ENTER] SKIP INTRO";
                float textW = static_cast<float>(skipText.length()) * 0.18f;
                gameRenderer.drawText(skipText, -textW * 0.5f, barY + 0.32f, 0.18f, 0.26f, promptColor, orthoUI);

                glfwSwapBuffers(window);
                glfwPollEvents();
                continue;
            }
        }

        // ---------------------------------------------------------------------
        // Gameplay Physics & Movement Update
        // ---------------------------------------------------------------------
        if (currentGameState == STATE_RUNNING)
        {
            playerSpeed = BASE_RUN_SPEED + (scoreDistance * 0.003f); // Balanced gradual speed scaling

            audioMgr.updateCooldowns(deltaTime);
            if (slideInterruptCooldown > 0.0f) slideInterruptCooldown -= deltaTime;

            if (activeNotice.timer > 0.0f) {
                activeNotice.timer -= rawDeltaTime;
            }

            if (autoShiftEnabled) {
                directionSwitchTimer -= deltaTime;
                if (directionSwitchTimer <= 0.0f) {
                    if (currentScene == SCENE_PLATFORMER || scenePlayTime >= SCENE_DIRECTION_COOLDOWN) {
                        triggerRandomDirectionShift("AUTONOMOUS SHIFT");
                    }
                }
            }

            // Update Dash Cooldown and Ghosts
            if (dashCooldown > 0.0f) dashCooldown -= deltaTime;
            if (isDashing) {
                dashTimer -= deltaTime;
                if (dashTimer <= 0.0f) {
                    isDashing = false;
                }
            }
            for (auto it = dashGhosts.begin(); it != dashGhosts.end(); ) {
                it->alpha -= deltaTime * 3.5f;
                if (it->alpha <= 0.0f) it = dashGhosts.erase(it);
                else ++it;
            }

            // Update World-Space Floating Texts
            for (auto it = floatingTexts.begin(); it != floatingTexts.end(); ) {
                it->timer -= rawDeltaTime;
                it->pos.y += it->riseSpeed * rawDeltaTime;
                if (it->timer <= 0.0f) {
                    it = floatingTexts.erase(it);
                } else {
                    ++it;
                }
            }

            // -----------------------------------------------------------------
            // Branch By Current Game Scene
            // -----------------------------------------------------------------
            if (currentScene == SCENE_LADDER_CLIMB)
            {
                scenePlayTime += deltaTime;
                if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN && !sceneDirectionUnlockedNotified) {
                    sceneDirectionUnlockedNotified = true;
                    audioMgr.play(SFX_RELIC);
                    spawnFloatingText(">>> DIRECTION SHIFTS UNLOCKED! <<<", glm::vec2(ladderPlayerX, ladderPlayerY + 1.2f), glm::vec4(0.35f, 1.0f, 0.5f, 1.0f));
                    showCornerNotification("TRIAL COMPLETE", "DIRECTION SHIFTS UNLOCKED!", "Left/Right arrow relics spawned! Touch to exit ladder!", glm::vec4(0.35f, 1.0f, 0.45f, 1.0f), 1, 0);

                    // Immediately spawn Left and Right Direction Arrow Relics just ahead of the player!
                    ladderItems.push_back({ 0, ladderPlayerY + 3.8f, 2, DIR_LEFT, false });
                    ladderItems.push_back({ 2, ladderPlayerY + 6.8f, 2, DIR_RIGHT, false });
                    ladderItems.push_back({ 1, ladderPlayerY + 10.2f, 2, DIR_LEFT, false });
                }

                if (!isLadderHit && !isLadderResurrecting) {
                    ladderPlayerY += ladderClimbSpeed * deltaTime;
                    scoreDistance += ladderClimbSpeed * deltaTime;

                    if (isLadderLeaping) {
                        ladderLeapProgress += deltaTime / LADDER_LEAP_TIME;
                        if (ladderLeapProgress >= 1.0f) {
                            ladderLeapProgress = 1.0f;
                            isLadderLeaping = false;
                            ladderCurrentLane = ladderTargetLane;
                            ladderPlayerX = LADDER_TRACK_X[ladderCurrentLane];
                        } else {
                            float fromX = LADDER_TRACK_X[ladderCurrentLane];
                            float toX   = LADDER_TRACK_X[ladderTargetLane];
                            ladderPlayerX = glm::mix(fromX, toX, ladderLeapProgress);
                        }
                    } else {
                        ladderPlayerX = LADDER_TRACK_X[ladderCurrentLane];
                    }

                    // Procedural generation of traps and items on 3 ladders
                    while (ladderGenTopY < ladderPlayerY + 25.0f) {
                        ladderGenTopY += 4.5f;
                        int trapLane = std::rand() % 3;
                        ladderTraps.push_back({ trapLane, ladderGenTopY, std::rand() % 2, true });

                        int itemLane = (trapLane + 1 + (std::rand() % 2)) % 3;
                        if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN) {
                            // After 15s trial: Regularly spawn Left and Right Direction Arrow Relics!
                            int roll = std::rand() % 10;
                            if (roll < 4) {
                                RunDirection arrowDir = (std::rand() % 2 == 0) ? DIR_LEFT : DIR_RIGHT;
                                ladderItems.push_back({ itemLane, ladderGenTopY + 1.2f, 2, arrowDir, false });
                            } else if (roll < 7) {
                                ladderItems.push_back({ itemLane, ladderGenTopY + 1.2f, 1, std::rand() % 4, false }); // Fruit shield
                            } else {
                                ladderItems.push_back({ itemLane, ladderGenTopY + 1.2f, 0, 0, false }); // Coin
                            }
                        } else {
                            if (std::rand() % 2 == 0) {
                                ladderItems.push_back({ itemLane, ladderGenTopY + 1.2f, 1, std::rand() % 4, false });
                            } else {
                                ladderItems.push_back({ itemLane, ladderGenTopY + 1.2f, 0, 0, false });
                            }
                        }
                    }

                    // Trap collisions
                    for (auto& trap : ladderTraps) {
                        if (trap.active && trap.lane == ladderCurrentLane && std::abs(ladderPlayerY - trap.y) < 0.9f) {
                            if (shieldTimer > 0.0f || isDashing) {
                                trap.active = false;
                                audioMgr.play(SFX_SLIDE);
                                if (shieldTimer > 0.0f && !isDashing) {
                                    shieldTimer = 0.0f; // Removed on first hit!
                                    spawnFloatingText("SHIELD BROKE! [TRAP DEFLECTED]", glm::vec2(ladderPlayerX, ladderPlayerY + 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                    showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed trap hit!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                                } else {
                                    spawnFloatingText("DASH DEFLECTED TRAP!", glm::vec2(ladderPlayerX, ladderPlayerY + 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                }
                            } else {
                                isLadderHit = true;
                                ladderHitVelY = -6.0f;
                                ladderDarkAlpha = 0.0f;
                                audioMgr.play(SFX_DEATH);
                                spawnFloatingText("STRUCK BY TRAP!", glm::vec2(ladderPlayerX, ladderPlayerY + 1.0f), glm::vec4(1.0f, 0.35f, 0.25f, 1.0f));
                                showCornerNotification("FALL", "KNOCKED OFF LADDER!", "Falling through the dark...", glm::vec4(1.0f, 0.35f, 0.2f, 1.0f), 0);
                                break;
                            }
                        }
                    }

                    // Collectibles collisions (Coins, Fruit Shields & Direction Arrow Relics)
                    for (auto& itm : ladderItems) {
                        if (!itm.collected && itm.lane == ladderCurrentLane && std::abs(ladderPlayerY - itm.y) < 1.0f) {
                            itm.collected = true;
                            if (itm.type == 0) {
                                coinsCollected++;
                                audioMgr.play(SFX_COIN);
                                spawnFloatingText("+50", glm::vec2(ladderPlayerX, itm.y + 0.5f), glm::vec4(1.0f, 0.88f, 0.2f, 1.0f));
                            } else if (itm.type == 1) {
                                shieldTimer = 10.0f;
                                audioMgr.play(SFX_RELIC);
                                showCornerNotification("SHIELD", "10s TEMPORARY SHIELD!", "Hazard invulnerability active!", glm::vec4(0.2f, 0.95f, 1.0f, 1.0f), 0);
                                spawnFloatingText("+10s SHIELD ACTIVE!", glm::vec2(ladderPlayerX, itm.y + 0.8f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                            } else if (itm.type == 2) {
                                RunDirection targetDir = static_cast<RunDirection>(itm.fruitType);
                                audioMgr.play(SFX_RELIC);
                                relicsTriggered++;
                                if (targetDir == DIR_LEFT || targetDir == DIR_RIGHT) {
                                    platformerSavedDir = targetDir;
                                    lastHorizontalDir = targetDir;
                                    currentDirection = targetDir;
                                    std::string dirStr = (targetDir == DIR_LEFT) ? "WARP <<< LEFT" : "WARP >>> RIGHT";
                                    spawnFloatingText("DIRECTION TRIGGERED!\n" + dirStr, glm::vec2(ladderPlayerX, itm.y + 0.8f),
                                                      glm::vec4(0.25f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.8f, 1.3f);
                                    showCornerNotification("WARP", dirStr, "Exiting ladder climb to platform runner!", glm::vec4(0.25f, 0.95f, 1.0f, 1.0f), 1, 0);
                                    returnToPlatformerScene(true);
                                    break;
                                } else if (targetDir == DIR_DOWN) {
                                    spawnFloatingText("WARP vvv SKY-FALL", glm::vec2(ladderPlayerX, itm.y + 0.8f), glm::vec4(0.3f, 1.0f, 0.5f, 1.0f));
                                    enterSkyFallScene();
                                    break;
                                }
                            }
                        }
                    }
                } else if (isLadderHit) {
                    ladderPlayerY += ladderHitVelY * deltaTime;
                    ladderHitVelY -= 22.0f * deltaTime;
                    ladderDarkAlpha = std::min(1.0f, ladderDarkAlpha + deltaTime * 1.5f);
                    if (ladderDarkAlpha >= 0.95f) {
                        isLadderHit = false;
                        isLadderResurrecting = true;
                        ladderResurrectTimer = 0.0f;
                    }
                } else if (isLadderResurrecting) {
                    ladderResurrectTimer += deltaTime;
                    if (ladderResurrectTimer >= 1.25f) {
                        isLadderResurrecting = false;
                        returnToPlatformerScene(false);
                    }
                }

                animTimer += deltaTime;
            }
            else if (currentScene == SCENE_SKY_FALL)
            {
                scenePlayTime += deltaTime;
                if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN && !sceneDirectionUnlockedNotified) {
                    sceneDirectionUnlockedNotified = true;
                    audioMgr.play(SFX_RELIC);
                    spawnFloatingText(">>> DIRECTION SHIFTS UNLOCKED! <<<", skyFallPos + glm::vec2(0.0f, 1.2f), glm::vec4(0.35f, 1.0f, 0.5f, 1.0f));
                    showCornerNotification("TRIAL COMPLETE", "DIRECTION SHIFTS UNLOCKED!", "Left/Right arrow relics spawned! Steer into arrow to resume runner!", glm::vec4(0.35f, 1.0f, 0.45f, 1.0f), 1, 0);

                    // 1. Immediately convert all uncollected items ahead of the falling player into Left/Right Direction Arrow Relics!
                    for (auto& itm : skyItemsList) {
                        if (!itm.collected && itm.pos.y < skyFallPos.y && itm.pos.y > skyFallPos.y - 35.0f) {
                            itm.type = 2;
                            itm.fruitType = (itm.pos.x <= 0.0f) ? DIR_LEFT : DIR_RIGHT;
                        }
                    }

                    // 2. Immediately spawn staggered Left and Right Direction Arrow gates directly in the descent route!
                    skyItemsList.push_back({ glm::vec2(-2.2f, skyFallPos.y - 5.0f), 2, DIR_LEFT, false });
                    skyItemsList.push_back({ glm::vec2( 2.2f, skyFallPos.y - 5.0f), 2, DIR_RIGHT, false });

                    skyItemsList.push_back({ glm::vec2(-3.0f, skyFallPos.y - 10.0f), 2, DIR_LEFT, false });
                    skyItemsList.push_back({ glm::vec2( 0.0f, skyFallPos.y - 10.0f), 2, (lastHorizontalDir == DIR_RIGHT ? DIR_RIGHT : DIR_LEFT), false });
                    skyItemsList.push_back({ glm::vec2( 3.0f, skyFallPos.y - 10.0f), 2, DIR_RIGHT, false });

                    skyItemsList.push_back({ glm::vec2(-2.2f, skyFallPos.y - 15.0f), 2, DIR_LEFT, false });
                    skyItemsList.push_back({ glm::vec2( 2.2f, skyFallPos.y - 15.0f), 2, DIR_RIGHT, false });
                }

                if (!isSkyFallHit) {
                    skyFallPos.y -= skyFallSpeed * deltaTime;
                    scoreDistance += skyFallSpeed * deltaTime;
                    skyFallPos.x += skyFallVelX * deltaTime;
                    skyFallPos.x = std::clamp(skyFallPos.x, -7.0f, 7.0f);
                    skyFallVelX *= 0.86f;

                    while (skyFallGenBottomY > skyFallPos.y - 25.0f) {
                        skyFallGenBottomY -= 14.0f;
                        // Spaced out bird spawns (50% chance per 14m)
                        if (std::rand() % 2 == 0) {
                            bool fromLeft = (std::rand() % 2 == 0);
                            float birdX = fromLeft ? -9.5f : 9.5f;
                            float birdVx = fromLeft ? (3.0f + (std::rand() % 15) / 10.0f) : -(3.0f + (std::rand() % 15) / 10.0f);
                            skyBirdsList.push_back({ glm::vec2(birdX, skyFallGenBottomY), birdVx, static_cast<float>(std::rand() % 10), true });
                        }

                        // Spaced out spike mines (33% chance per 14m) leaving clear evasive paths
                        if (std::rand() % 3 == 0) {
                            float trapX = ((std::rand() % 60) / 10.0f) - 3.0f;
                            skySpikesList.push_back({ glm::vec2(trapX, skyFallGenBottomY - 3.0f), glm::vec2(0.85f, 0.85f), true });
                        }

                        if (scenePlayTime >= SCENE_DIRECTION_COOLDOWN) {
                            // After 15s trial: Continuously spawn Left and Right Direction Arrow Relics at every tier!
                            skyItemsList.push_back({ glm::vec2(-2.5f, skyFallGenBottomY - 2.5f), 2, DIR_LEFT, false });
                            skyItemsList.push_back({ glm::vec2( 2.5f, skyFallGenBottomY - 2.5f), 2, DIR_RIGHT, false });
                            skyItemsList.push_back({ glm::vec2( 0.0f, skyFallGenBottomY - 7.5f), 2, (std::rand() % 2 == 0 ? DIR_LEFT : DIR_RIGHT), false });
                            if (std::rand() % 2 == 0) {
                                skyItemsList.push_back({ glm::vec2(-4.0f, skyFallGenBottomY - 5.0f), 1, std::rand() % 4, false }); // Fruit shield
                            } else {
                                skyItemsList.push_back({ glm::vec2( 4.0f, skyFallGenBottomY - 5.0f), 0, 0, false }); // Coin
                            }
                        } else {
                            float itemX = ((std::rand() % 80) / 10.0f) - 4.0f;
                            if (std::rand() % 2 == 0) {
                                skyItemsList.push_back({ glm::vec2(itemX, skyFallGenBottomY - 1.5f), 1, std::rand() % 4, false }); // Fruit -> grants shield
                            } else {
                                skyItemsList.push_back({ glm::vec2(itemX, skyFallGenBottomY - 1.5f), 0, 0, false }); // Coin
                            }
                        }
                    }

                    for (auto& bird : skyBirdsList) {
                        bird.pos.x += bird.vx * deltaTime;
                        bird.animTimer += deltaTime;

                        if (bird.active &&
                            std::abs(bird.pos.x - skyFallPos.x) < 0.85f &&
                            std::abs(bird.pos.y - skyFallPos.y) < 0.85f)
                        {
                            if (shieldTimer > 0.0f || isDashing) {
                                bird.active = false;
                                audioMgr.play(SFX_SLIDE);
                                if (shieldTimer > 0.0f && !isDashing) {
                                    shieldTimer = 0.0f; // Removed on first hit!
                                    spawnFloatingText("SHIELD BROKE! [BIRD DEFLECTED]", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                    showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed aerial collision!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                                } else {
                                    spawnFloatingText("DASH DEFLECTED BIRD!", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                }
                            } else {
                                isSkyFallHit = true;
                                skyFallHitTimer = 0.0f;
                                skyFallDarkAlpha = 0.0f;
                                playerSpriteAngle = 90.0f; // 90 degree anticlockwise!
                                audioMgr.play(SFX_DEATH);
                                spawnFloatingText("STRUCK BY RAPTOR!", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 0.35f, 0.2f, 1.0f));
                                showCornerNotification("HIT", "AERIAL COLLISION!", "Rotating 90 deg CCW impact!", glm::vec4(1.0f, 0.4f, 0.2f, 1.0f), 0);
                                break;
                            }
                        }
                    }

                    if (!isSkyFallHit) {
                        for (auto& trap : skySpikesList) {
                            if (trap.active &&
                                std::abs(trap.pos.x - skyFallPos.x) < 0.85f &&
                                std::abs(trap.pos.y - skyFallPos.y) < 0.85f)
                            {
                                if (shieldTimer > 0.0f || isDashing) {
                                    trap.active = false;
                                    audioMgr.play(SFX_SLIDE);
                                    if (shieldTimer > 0.0f && !isDashing) {
                                        shieldTimer = 0.0f; // Removed on first hit!
                                        spawnFloatingText("SHIELD BROKE! [MINE DEFLECTED]", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                        showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed spike mine hit!", glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                                    } else {
                                        spawnFloatingText("DASH DEFLECTED MINE!", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                    }
                                } else {
                                    isSkyFallHit = true;
                                    skyFallHitTimer = 0.0f;
                                    skyFallDarkAlpha = 0.0f;
                                    playerSpriteAngle = 90.0f;
                                    audioMgr.play(SFX_DEATH);
                                    spawnFloatingText("MINE DETONATION!", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 0.35f, 0.2f, 1.0f));
                                    showCornerNotification("HIT", "AERIAL MINE EXPLODED!", "Returning to surface runner!", glm::vec4(1.0f, 0.4f, 0.2f, 1.0f), 0);
                                    break;
                                }
                            }
                        }
                    }

                    for (auto& itm : skyItemsList) {
                        float distThreshX = (itm.type == 2) ? 1.85f : 1.1f;
                        float distThreshY = (itm.type == 2) ? 2.1f : 1.1f;
                        if (!itm.collected &&
                            std::abs(itm.pos.x - skyFallPos.x) < distThreshX &&
                            std::abs(itm.pos.y - skyFallPos.y) < distThreshY)
                        {
                            itm.collected = true;
                            if (itm.type == 0) {
                                coinsCollected++;
                                audioMgr.play(SFX_COIN);
                                spawnFloatingText("+50", skyFallPos + glm::vec2(0.0f, 0.5f), glm::vec4(1.0f, 0.88f, 0.2f, 1.0f));
                            } else if (itm.type == 1) {
                                shieldTimer = 10.0f;
                                audioMgr.play(SFX_RELIC);
                                showCornerNotification("SHIELD", "10s TEMPORARY SHIELD!", "Hazard invulnerability active!", glm::vec4(0.2f, 0.95f, 1.0f, 1.0f), 0);
                                spawnFloatingText("+10s SHIELD ACTIVE!", skyFallPos + glm::vec2(0.0f, 0.8f), glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                            } else if (itm.type == 2) {
                                RunDirection targetDir = static_cast<RunDirection>(itm.fruitType);
                                audioMgr.play(SFX_RELIC);
                                relicsTriggered++;
                                if (targetDir == DIR_LEFT || targetDir == DIR_RIGHT) {
                                    platformerSavedDir = targetDir;
                                    lastHorizontalDir = targetDir;
                                    currentDirection = targetDir;
                                    std::string dirStr = (targetDir == DIR_LEFT) ? "WARP <<< LEFT" : "WARP >>> RIGHT";
                                    spawnFloatingText("DIRECTION TRIGGERED!\n" + dirStr, skyFallPos + glm::vec2(0.0f, 0.8f),
                                                      glm::vec4(0.25f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.8f, 1.3f);
                                    showCornerNotification("WARP", dirStr, "Resuming regular platform runner!", glm::vec4(0.25f, 0.95f, 1.0f, 1.0f), 1, 0);
                                    returnToPlatformerScene(true);
                                    break;
                                } else if (targetDir == DIR_UP) {
                                    spawnFloatingText("WARP ^^^ LADDER ASCENT", skyFallPos + glm::vec2(0.0f, 0.8f), glm::vec4(0.35f, 1.0f, 0.45f, 1.0f));
                                    enterLadderScene();
                                    break;
                                }
                            }
                        }
                    }
                } else if (isSkyFallHit) {
                    skyFallHitTimer += deltaTime;
                    skyFallDarkAlpha = std::min(1.0f, skyFallDarkAlpha + deltaTime * 1.5f);
                    if (skyFallHitTimer >= 1.0f) {
                        isSkyFallHit = false;
                        playerSpriteAngle = 0.0f;
                        returnToPlatformerScene(false);
                    }
                }

                animTimer += deltaTime;
            }
            else
            {
                // -------------------------------------------------------------
                // Standard Platformer Runner Physics & Collision
                // -------------------------------------------------------------
                if (isDashing) {
                    float dirSign = (lastHorizontalDir == DIR_LEFT) ? -1.0f : 1.0f;
                    playerVel.x = dirSign * DASH_SPEED_MAG;
                    dashGhosts.push_back({ playerPos, 0.45f, (lastHorizontalDir == DIR_LEFT), 0 });
                }

                // 1. Proactive Ladder Shaft Detection & Latching
                isOnLadder = false;
                LadderTunnel* activeTunnel = nullptr;

                for (auto& tunnel : gameWorld.tunnels) {
                    bool inVerticalRange = (playerPos.y >= (tunnel.minY - 0.4f) && playerPos.y <= (tunnel.maxY + 1.4f));
                    bool inHorizontalRange = (std::abs(playerPos.x - tunnel.x) <= 1.25f);

                    if (inVerticalRange && inHorizontalRange) {
                        if (currentDirection == DIR_UP || currentDirection == DIR_DOWN ||
                            glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
                            glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
                            glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
                            glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                        {
                            isOnLadder = true;
                            activeTunnel = &tunnel;
                            tunnel.playerInside = true;
                            playerPos.x = glm::mix(playerPos.x, tunnel.x, deltaTime * 16.0f);
                            playerVel.x = 0.0f;
                            break;
                        }
                    } else {
                        tunnel.playerInside = false;
                    }
                }

                // 2. Steep Slope Sliding Detection
                isSliding = false;
                playerSpriteAngle = 0.0f;
                if (slideInterruptCooldown <= 0.0f)
                {
                    for (const auto& slope : gameWorld.slopes) {
                        float minX = std::min(slope.startX, slope.endX);
                        float maxX = std::max(slope.startX, slope.endX);

                        if (playerPos.x >= minX - 0.2f && playerPos.x <= maxX + 0.2f) {
                            float t = (playerPos.x - slope.startX) / (slope.endX - slope.startX);
                            t = std::max(0.0f, std::min(1.0f, t));
                            float slopeY = slope.startY + t * (slope.endY - slope.startY);
                            float targetFeetY = slopeY;

                            if (playerPos.y - (playerHitbox.y * 0.5f) <= targetFeetY + 0.45f &&
                                playerPos.y - (playerHitbox.y * 0.5f) >= targetFeetY - 1.2f)
                            {
                                isSliding = true;
                                playerPos.y = targetFeetY + (playerHitbox.y * 0.5f);
                                playerSpriteAngle = slope.angle * 0.65f;

                                float slideDir = (slope.endX > slope.startX) ? 1.0f : -1.0f;
                                playerVel.x = slideDir * SLIDE_SPEED;
                                playerVel.y = ((slope.endY - slope.startY) / std::abs(slope.endX - slope.startX)) * std::abs(playerVel.x);

                                if (audioMgr.slideCooldown <= 0.0f) {
                                    audioMgr.play(SFX_SLIDE);
                                    audioMgr.slideCooldown = 0.26f;
                                }

                                if (activeNotice.timer <= 0.0f || activeNotice.tag != "SLOPE") {
                                    showCornerNotification("SLOPE", "STEEP SLOPE SLIDE!", "Sliding rapidly! Press SPACE to jump midpoint!",
                                                           glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 0);
                                }
                                break;
                            }
                        }
                    }
                }

                // 3. Movement Velocity Resolution
                if (!isSliding && !isDashing)
                {
                    if (isGameIdleAtStart) {
                        playerVel.x = 0.0f;
                    } else if (freeExplorationMode) {
                        // Free Exploration Mode: Pure manual A/D movement control without auto-sprinting!
                        bool moveLeft  = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
                        bool moveRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);

                        if (moveLeft && !moveRight) {
                            playerVel.x = -playerSpeed;
                            currentDirection = DIR_LEFT;
                            lastHorizontalDir = DIR_LEFT;
                        } else if (moveRight && !moveLeft) {
                            playerVel.x = playerSpeed;
                            currentDirection = DIR_RIGHT;
                            lastHorizontalDir = DIR_RIGHT;
                        } else {
                            // Come to a complete stop when movement keys are released
                            playerVel.x = 0.0f;
                        }
                    } else if (currentDirection == DIR_RIGHT) {
                        playerVel.x = playerSpeed;
                        lastHorizontalDir = DIR_RIGHT;
                    } else if (currentDirection == DIR_LEFT) {
                        playerVel.x = -playerSpeed;
                        lastHorizontalDir = DIR_LEFT;
                    } else if (currentDirection == DIR_UP) {
                        if (isOnLadder && activeTunnel) {
                            playerPos.x = glm::mix(playerPos.x, activeTunnel->x, deltaTime * 16.0f);
                            playerVel.x = 0.0f;
                            if (freeExplorationMode) {
                                bool climbUp = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
                                bool climbDn = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
                                if (climbUp && !climbDn) {
                                    playerVel.y = CLIMB_SPEED;
                                } else if (climbDn && !climbUp) {
                                    playerVel.y = -CLIMB_SPEED;
                                } else {
                                    playerVel.y = 0.0f;
                                }
                            } else {
                                playerVel.y = CLIMB_SPEED;
                            }

                            if (playerPos.y >= activeTunnel->maxY - 0.15f) {
                                playerPos.y = activeTunnel->maxY + (playerHitbox.y * 0.5f) + 0.15f;
                                playerVel.y = 0.0f;
                                isOnLadder = false;
                                currentDirection = lastHorizontalDir;
                                showCornerNotification("TUNNEL", "REACHED SHAFT SUMMIT",
                                                       (lastHorizontalDir == DIR_RIGHT ? "Resuming Forward Right >>>" : "Resuming Reverse Left <<<"),
                                                       glm::vec4(0.35f, 0.95f, 0.4f, 1.0f), 1, 2);
                            }
                        } else {
                            playerVel.x = (lastHorizontalDir == DIR_RIGHT) ? playerSpeed : -playerSpeed;
                        }
                    } else if (currentDirection == DIR_DOWN) {
                        if (isOnLadder && activeTunnel) {
                            playerPos.x = glm::mix(playerPos.x, activeTunnel->x, deltaTime * 16.0f);
                            playerVel.x = 0.0f;
                            if (freeExplorationMode) {
                                bool climbUp = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
                                bool climbDn = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
                                if (climbUp && !climbDn) {
                                    playerVel.y = CLIMB_SPEED;
                                } else if (climbDn && !climbUp) {
                                    playerVel.y = -CLIMB_SPEED;
                                } else {
                                    playerVel.y = 0.0f;
                                }
                            } else {
                                playerVel.y = -CLIMB_SPEED;
                            }

                            if (playerPos.y <= activeTunnel->minY + 0.2f) {
                                playerPos.y = activeTunnel->minY + (playerHitbox.y * 0.5f);
                                playerVel.y = 0.0f;
                                isOnLadder = false;
                                currentDirection = lastHorizontalDir;
                                showCornerNotification("TUNNEL", "REACHED SHAFT BASE",
                                                       (lastHorizontalDir == DIR_RIGHT ? "Resuming Forward Right >>>" : "Resuming Reverse Left <<<"),
                                                       glm::vec4(0.35f, 0.95f, 0.4f, 1.0f), 1, 0);
                            }
                        } else {
                            playerVel.x = (lastHorizontalDir == DIR_RIGHT) ? playerSpeed : -playerSpeed;
                        }
                    }

                    if (!(isOnLadder && (currentDirection == DIR_UP || currentDirection == DIR_DOWN))) {
                        playerVel.y += GRAVITY * deltaTime;
                        if (playerVel.y < -28.0f) playerVel.y = -28.0f;
                    }
                }

                // Integrate Position
                playerPos.x += playerVel.x * deltaTime;
                playerPos.y += playerVel.y * deltaTime;
                scoreDistance += std::abs(playerVel.x) * deltaTime;

                // 4. Platform Collision Resolution
                isGrounded = false;
                if (!isSliding)
                {
                    for (const auto& tile : gameWorld.tiles)
                    {
                        if (tile.isLadder) continue;

                        // When actively climbing or descending inside a ladder tunnel, do not let shaft interior tiles block movement
                        if (isOnLadder && activeTunnel) {
                            if (std::abs(tile.pos.x - activeTunnel->x) < 1.15f &&
                                tile.pos.y >= activeTunnel->minY - 0.5f && tile.pos.y <= activeTunnel->maxY + 0.8f)
                            {
                                continue;
                            }
                        }

                        if (tile.isOneWay && (currentDirection == DIR_DOWN ||
                                              glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
                                              glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
                            continue;
                        }

                        float playerHalfW = playerHitbox.x * 0.5f;
                        float playerHalfH = playerHitbox.y * 0.5f;
                        float tileHalfW   = tile.size.x * 0.5f;
                        float tileHalfH   = tile.size.y * 0.5f;

                        bool overlapX = (playerPos.x + playerHalfW > tile.pos.x - tileHalfW + 0.12f) &&
                                        (playerPos.x - playerHalfW < tile.pos.x + tileHalfW - 0.12f);

                        if (overlapX)
                        {
                            float tileTop = tile.pos.y + tileHalfH;
                            float playerBottom = playerPos.y - playerHalfH;

                            if (playerVel.y <= 0.0f && playerBottom >= tileTop - 0.45f && playerBottom <= tileTop + 0.28f)
                            {
                                playerPos.y = tileTop + playerHalfH;
                                playerVel.y = 0.0f;
                                isGrounded = true;
                            }
                        }
                    }
                }

                // Bedrock Foundation (Safety Catch: Zero Out-of-Bounds Deaths!)
                if (playerPos.y - (playerHitbox.y * 0.5f) <= BEDROCK_Y + 0.5f) {
                    playerPos.y = BEDROCK_Y + 0.5f + (playerHitbox.y * 0.5f);
                    playerVel.y = 0.0f;
                    isGrounded = true;
                }

                // Movement Audio
                if (isGrounded && !isSliding && std::abs(playerVel.x) > 1.0f) {
                    if (audioMgr.stepCooldown <= 0.0f) {
                        audioMgr.play(SFX_STEP);
                        audioMgr.stepCooldown = 0.32f;
                    }
                } else if (isOnLadder && std::abs(playerVel.y) > 1.0f) {
                    if (audioMgr.climbCooldown <= 0.0f) {
                        audioMgr.play(SFX_CLIMB);
                        audioMgr.climbCooldown = 0.32f;
                    }
                }

                // 5. Spiky Trap Collisions (Phased through safely during DASH or TEMPORARY SHIELD!)
                if (!isDashing && postHitInvulnTimer <= 0.0f) {
                    for (const auto& spike : gameWorld.spikes) {
                        if (gameWorld.checkCollision(playerPos, playerHitbox, spike.pos, spike.size)) {
                            if (shieldTimer > 0.0f) {
                                shieldTimer = 0.0f; // Shield is consumed on first hit!
                                postHitInvulnTimer = 0.75f; // Grace window to safely clear current spike
                                audioMgr.play(SFX_SLIDE);
                                spawnFloatingText("SHIELD BROKE! [TRAP DEFLECTED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                  glm::vec4(0.3f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.2f, 1.4f);
                                showCornerNotification("SHIELD", "SHIELD BROKEN!", "Absorbed spike hit and broke!",
                                                       glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                                break;
                            } else {
                                currentGameState = STATE_GAME_OVER;
                                gameOverReason = "IMPALED BY SPIKY TRAP";
                                audioMgr.play(SFX_DEATH);
                                break;
                            }
                        }
                    }
                }

                // 6. Direction-Shift Collectible Relics (Arrowhead Symbols)
                for (auto& relic : gameWorld.relics)
                {
                    if (!relic.collected && gameWorld.checkCollision(playerPos, playerHitbox, relic.pos, glm::vec2(1.1f, 1.1f)))
                    {
                        relic.collected = true;
                        relicsTriggered++;
                        audioMgr.play(SFX_RELIC);
                        triggerRandomDirectionShift("DIRECTION ARROW", relic.targetDir);

                        std::string dirWarpStr = (relic.targetDir == DIR_RIGHT) ? "WARP >>> RIGHT" :
                                                 (relic.targetDir == DIR_LEFT)  ? "WARP <<< LEFT" :
                                                 (relic.targetDir == DIR_UP)    ? "WARP ^^^ ASCENT" : "WARP vvv SKY-FALL";
                        spawnFloatingText("DIRECTION CHANGE!\n" + dirWarpStr, relic.pos + glm::vec2(0.0f, 0.8f),
                                          glm::vec4(0.25f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.6f, 1.3f);
                    }
                    relic.animPhase += deltaTime * 3.5f;
                }

                // 6b. Fruit Pickups (Provide Temporary Shield!)
                for (auto& fruit : gameWorld.fruits)
                {
                    if (!fruit.collected && gameWorld.checkCollision(playerPos, playerHitbox, fruit.pos, glm::vec2(1.1f, 1.1f)))
                    {
                        fruit.collected = true;
                        shieldTimer = 10.0f;
                        audioMgr.play(SFX_RELIC);
                        showCornerNotification("SHIELD", "10s TEMPORARY SHIELD!", "Hazard invulnerability active!",
                                               glm::vec4(0.2f, 0.95f, 1.0f, 1.0f), 0);
                        spawnFloatingText("+10s SHIELD ACTIVE!", fruit.pos + glm::vec2(0.0f, 0.8f),
                                          glm::vec4(0.3f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.6f, 1.3f);
                    }
                    fruit.animPhase += deltaTime * 3.5f;
                }

                // 7. Animated Coins Collection
                for (auto& coin : gameWorld.coins)
                {
                    if (!coin.collected && gameWorld.checkCollision(playerPos, playerHitbox, coin.pos, glm::vec2(1.0f, 1.0f)))
                    {
                        coin.collected = true;
                        coinsCollected++;
                        audioMgr.play(SFX_COIN);
                        showCornerNotification("COLLECTIBLE", "+50 GOLD COIN!",
                                               "Total Gold Coins: " + std::to_string(coinsCollected),
                                               glm::vec4(1.0f, 0.82f, 0.15f, 1.0f), 0);
                        spawnFloatingText("+50", coin.pos + glm::vec2(0.0f, 0.5f),
                                          glm::vec4(1.0f, 0.88f, 0.22f, 1.0f), 0.24f, 0.34f, 1.4f, 1.1f);
                    }
                    coin.animTimer += deltaTime * 10.0f;
                }

                // 8. Procedural World Streaming
                gameWorld.updateProcedural(playerPos.x, activeTheme);

                // 9. Animation State Resolution
                if (isSliding) {
                    currentAnim = ANIM_SLIDE;
                } else if (isOnLadder && (currentDirection == DIR_UP || playerVel.y > 0.5f)) {
                    currentAnim = ANIM_CLIMB_UP;
                } else if (isOnLadder && (currentDirection == DIR_DOWN || playerVel.y < -0.5f)) {
                    currentAnim = ANIM_CLIMB_DOWN;
                } else if (!isGrounded && playerVel.y < -1.8f) {
                    currentAnim = ANIM_FALLING;
                } else if (!isGrounded && playerVel.y >= -1.8f) {
                    currentAnim = ANIM_JUMPING;
                } else if (isGameIdleAtStart || (freeExplorationMode && std::abs(playerVel.x) < 0.2f)) {
                    currentAnim = ANIM_IDLE;
                } else if (currentDirection == DIR_LEFT || (isOnLadder && lastHorizontalDir == DIR_LEFT)) {
                    currentAnim = ANIM_RUN_LEFT;
                } else {
                    currentAnim = ANIM_RUN_RIGHT;
                }

                animTimer += deltaTime;
            }
        }

        // ---------------------------------------------------------------------
        // Camera Follow
        // ---------------------------------------------------------------------
        if (currentScene == SCENE_LADDER_CLIMB) {
            cameraPos.x = glm::mix(cameraPos.x, ladderPlayerX, rawDeltaTime * 6.0f);
            cameraPos.y = glm::mix(cameraPos.y, ladderPlayerY + 1.2f, rawDeltaTime * 5.0f);
        } else if (currentScene == SCENE_SKY_FALL) {
            cameraPos.x = glm::mix(cameraPos.x, skyFallPos.x, rawDeltaTime * 6.0f);
            cameraPos.y = glm::mix(cameraPos.y, skyFallPos.y - 1.2f, rawDeltaTime * 5.0f);
        } else {
            float lookAhead = (currentDirection == DIR_RIGHT || (isOnLadder && lastHorizontalDir == DIR_RIGHT)) ? 3.0f : -3.0f;
            float targetCamX = playerPos.x + lookAhead;
            float targetCamY = playerPos.y + 0.9f;

            cameraPos.x = glm::mix(cameraPos.x, targetCamX, rawDeltaTime * 4.5f);
            cameraPos.y = glm::mix(cameraPos.y, targetCamY, rawDeltaTime * 3.8f);
        }

        // ---------------------------------------------------------------------
        // Render Frame
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        // Render Frame: Calculate Dynamic Celestial Sky Color (1.5m Day/Night Cycle)
        // ---------------------------------------------------------------------
        glm::vec4 nightSkyCol(0.035f, 0.05f, 0.12f, 1.0f);
        glm::vec4 twilightSkyCol(0.38f, 0.16f, 0.20f, 1.0f);
        glm::vec4 daySkyCol(0.24f, 0.52f, 0.85f, 1.0f);

        float twilightFactor = 0.0f;
        if (daylightFactor > 0.05f && daylightFactor < 0.95f) {
            twilightFactor = std::sin(daylightFactor * 3.14159f);
        }
        glm::vec4 dynamicSky = glm::mix(nightSkyCol, daySkyCol, daylightFactor);
        dynamicSky = glm::mix(dynamicSky, twilightSkyCol, twilightFactor * 0.70f);

        if (currentGameState == STATE_GAME_OVER) {
            glClearColor(0.24f, 0.06f, 0.09f, 1.0f);
        } else if (currentScene == SCENE_LADDER_CLIMB) {
            glClearColor(dynamicSky.r * 0.65f, dynamicSky.g * 0.65f, dynamicSky.b * 0.75f, 1.0f);
        } else if (currentScene == SCENE_SKY_FALL) {
            glClearColor(dynamicSky.r, dynamicSky.g, dynamicSky.b, 1.0f);
        } else {
            if (playerPos.y < -2.0f) {
                glClearColor(dynamicSky.r * 0.35f, dynamicSky.g * 0.35f, dynamicSky.b * 0.42f, 1.0f);
            } else {
                glClearColor(dynamicSky.r, dynamicSky.g, dynamicSky.b, 1.0f);
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gameRenderer.getProgram());

        float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
        float halfH  = 6.8f;
        float halfW  = halfH * aspect;
        glm::mat4 projection = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));

        if (currentScene == SCENE_LADDER_CLIMB)
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
                                  (texScenery[activeScenery] != 0), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texScenery[activeScenery]);

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
                float lx = LADDER_TRACK_X[lane];
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
                    gameRenderer.drawQuad(modelRung, view, projection, glm::vec4(1.0f), (worldTilesetTexture != 0), rungUV, worldTilesetTexture);
                }
            }

            // 3. Render Ladder Traps (Spikes / Sawblades)
            for (const auto& trap : ladderTraps) {
                if (!trap.active) continue;
                float tx = LADDER_TRACK_X[trap.lane];
                glm::mat4 modelTrap = glm::mat4(1.0f);
                modelTrap = glm::translate(modelTrap, glm::vec3(tx, trap.y, 0.0f));
                modelTrap = glm::scale(modelTrap, glm::vec3(1.35f, 1.35f, 1.0f));
                glm::vec4 trapUV = getTilesetUV(4, 8);
                gameRenderer.drawQuad(modelTrap, view, projection, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), (worldTilesetTexture != 0), trapUV, worldTilesetTexture);
            }

            // 4. Render Ladder Collectibles (Coins, Fruit Shields & Direction Arrow Relics)
            for (const auto& itm : ladderItems) {
                if (itm.collected) continue;
                float ix = LADDER_TRACK_X[itm.lane];
                if (itm.type == 0) {
                    glm::mat4 modelCoin = glm::mat4(1.0f);
                    modelCoin = glm::translate(modelCoin, glm::vec3(ix, itm.y, 0.0f));
                    modelCoin = glm::scale(modelCoin, glm::vec3(0.9f, 0.9f, 1.0f));
                    gameRenderer.drawQuad(modelCoin, view, projection, glm::vec4(1.0f), (coinTexture != 0), getCoinUV(0), coinTexture);
                } else if (itm.type == 1) {
                    glm::mat4 modelFruit = glm::mat4(1.0f);
                    modelFruit = glm::translate(modelFruit, glm::vec3(ix, itm.y, 0.0f));
                    modelFruit = glm::scale(modelFruit, glm::vec3(1.2f, 1.2f, 1.0f));
                    gameRenderer.drawQuad(modelFruit, view, projection, glm::vec4(1.0f), (fruitTexture != 0), getFruitUV(itm.fruitType), fruitTexture);
                } else if (itm.type == 2) {
                    RunDirection relicDir = static_cast<RunDirection>(itm.fruitType);
                    float pulse = 1.35f + 0.15f * std::sin(static_cast<float>(glfwGetTime()) * 5.0f);
                    glm::mat4 modelRelic = glm::mat4(1.0f);
                    modelRelic = glm::translate(modelRelic, glm::vec3(ix, itm.y, 0.0f));
                    modelRelic = glm::scale(modelRelic, glm::vec3(pulse, pulse, 1.0f));

                    glm::vec4 arrowUV = getDirectionArrowUV(relicDir);
                    glm::vec4 arrowTint(1.0f);
                    if (relicDir == DIR_LEFT)       arrowTint = glm::vec4(0.2f, 0.95f, 1.0f, 1.0f);
                    else if (relicDir == DIR_RIGHT) arrowTint = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
                    else if (relicDir == DIR_DOWN)  arrowTint = glm::vec4(0.3f, 1.0f, 0.5f, 1.0f);

                    gameRenderer.drawQuad(modelRelic, view, projection, arrowTint, (directionArrowsTexture != 0), arrowUV, directionArrowsTexture);
                }
            }

            // 5. Render Character on Ladder (Climber, Falling, or Reverse Death Resurrection!)
            CharacterAnimSet& activeSet = samuraiAnim;
            if (isLadderResurrecting) {
                // Dark screen overlay
                glm::mat4 modelDark = glm::mat4(1.0f);
                modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
                modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
                gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.01f, 0.02f, 0.04f, 0.94f), false, glm::vec4(0), 0);

                // Dramatic Resurrection Spotlight
                glm::mat4 modelSpot = glm::mat4(1.0f);
                modelSpot = glm::translate(modelSpot, glm::vec3(cameraPos.x, cameraPos.y - 0.2f, 0.0f));
                modelSpot = glm::scale(modelSpot, glm::vec3(4.8f, 4.8f, 1.0f));
                gameRenderer.drawQuad(modelSpot, view, projection, glm::vec4(0.25f, 0.85f, 1.0f, 0.28f), false, glm::vec4(0), 0);

                // REVERSE Death Animation (frames from 5 down to 0)!
                int totalFrames = activeSet.deathFrames;
                int frameIdx = std::clamp(totalFrames - 1 - static_cast<int>(ladderResurrectTimer * 5.0f), 0, totalFrames - 1);
                float u = static_cast<float>(frameIdx) / static_cast<float>(totalFrames);
                float du = 1.0f / static_cast<float>(totalFrames);
                glm::vec4 uvRect(u, 0.0f, du, 1.0f);

                glm::mat4 modelHero = glm::mat4(1.0f);
                modelHero = glm::translate(modelHero, glm::vec3(cameraPos.x, cameraPos.y - 0.10f, 0.0f));
                modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
                gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeSet.texDeath != 0), uvRect, activeSet.texDeath);

                // Resurrection Title
                std::string rezText = "=== RESURRECTION... RISE! ===";
                float rezW = rezText.length() * 0.30f;
                gameRenderer.drawText(rezText, cameraPos.x - rezW * 0.5f, cameraPos.y + 2.2f, 0.30f, 0.42f,
                                      glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), projection, view);
            } else {
                unsigned int activeTex = isLadderLeaping ? activeSet.texJump : activeSet.texJump;
                int totalFrames = activeSet.jumpFrames;
                int frameIdx = 0;

                if (isLadderHit) {
                    activeTex = activeSet.texFall;
                    totalFrames = activeSet.fallFrames;
                    frameIdx = 0;
                } else if (!isLadderLeaping) {
                    int climbStep = static_cast<int>(animTimer * 4.0f) % 2;
                    activeTex = (climbStep == 0) ? activeSet.texJump : activeSet.texFall;
                    totalFrames = (climbStep == 0) ? activeSet.jumpFrames : activeSet.fallFrames;
                }

                float u = static_cast<float>(frameIdx) / static_cast<float>(totalFrames);
                float du = 1.0f / static_cast<float>(totalFrames);
                glm::vec4 uvRect(u, 0.0f, du, 1.0f);

                glm::mat4 modelHero = glm::mat4(1.0f);
                modelHero = glm::translate(modelHero, glm::vec3(ladderPlayerX, ladderPlayerY - 0.10f, 0.0f));
                gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeTex != 0), uvRect, activeTex);

                // Render Circular Shield Bubble wrapped around hero if active
                if (shieldTimer > 0.0f) {
                    float shieldPulse = 1.0f + std::sin(animTimer * 8.0f) * 0.06f;
                    glm::mat4 modelShield = glm::mat4(1.0f);
                    modelShield = glm::translate(modelShield, glm::vec3(ladderPlayerX, ladderPlayerY, 0.0f));
                    modelShield = glm::scale(modelShield, glm::vec3(2.35f * shieldPulse, 2.35f * shieldPulse, 1.0f));
                    float sAlpha = std::min(0.70f, 0.35f + (shieldTimer / 10.0f) * 0.35f);
                    gameRenderer.drawCircle(modelShield, view, projection, glm::vec4(0.2f, 0.85f, 1.0f, sAlpha), 2.2f);
                }

                if (ladderDarkAlpha > 0.01f) {
                    glm::mat4 modelDark = glm::mat4(1.0f);
                    modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
                    modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
                    gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.0f, 0.0f, 0.0f, ladderDarkAlpha), false, glm::vec4(0), 0);
                }
            }
        }
        else if (currentScene == SCENE_SKY_FALL)
        {
            // Dynamic Celestial Objects in Sky
            if (daylightFactor < 0.75f) {
                float starAlpha = (1.0f - daylightFactor) * 0.95f;
                for (const auto& star : skyStars) {
                    float tw = 0.5f + 0.5f * std::sin(glfwGetTime() * star.twinkleFreq + star.twinklePhase);
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
                                  (texScenery[activeScenery] != 0), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texScenery[activeScenery]);

            // 1. Endless Sky Clouds Backdrop
            for (int i = -4; i <= 4; ++i) {
                float bgX = i * 4.0f;
                glm::mat4 modelBg = glm::mat4(1.0f);
                modelBg = glm::translate(modelBg, glm::vec3(bgX, cameraPos.y, 0.0f));
                modelBg = glm::scale(modelBg, glm::vec3(2.5f, 30.0f, 1.0f));
                gameRenderer.drawQuad(modelBg, view, projection, glm::vec4(0.14f, 0.24f, 0.40f, 0.75f), false, glm::vec4(0), 0);
            }

            // 2. Render Floating Spike Traps
            for (const auto& trap : skySpikesList) {
                if (!trap.active) continue;
                glm::mat4 modelTrap = glm::mat4(1.0f);
                modelTrap = glm::translate(modelTrap, glm::vec3(trap.pos.x, trap.pos.y, 0.0f));
                modelTrap = glm::scale(modelTrap, glm::vec3(1.3f, 1.3f, 1.0f));
                glm::vec4 trapUV = getTilesetUV(4, 8);
                gameRenderer.drawQuad(modelTrap, view, projection, glm::vec4(1.0f, 0.35f, 0.35f, 1.0f), (worldTilesetTexture != 0), trapUV, worldTilesetTexture);
            }

            // 3. Render Moving Birds (Flapping Raptor Asset)
            for (const auto& bird : skyBirdsList) {
                if (!bird.active) continue;
                int bFrame = static_cast<int>(bird.animTimer * 7.0f) % 4;
                float u = bFrame * 0.25f;
                float du = 0.25f;
                glm::vec4 birdUV = (bird.vx < 0.0f) ? glm::vec4(u + du, 0.0f, -du, 1.0f) : glm::vec4(u, 0.0f, du, 1.0f);

                glm::mat4 modelBird = glm::mat4(1.0f);
                modelBird = glm::translate(modelBird, glm::vec3(bird.pos.x, bird.pos.y, 0.0f));
                modelBird = glm::scale(modelBird, glm::vec3(2.4f, 2.4f, 1.0f));
                gameRenderer.drawQuad(modelBird, view, projection, glm::vec4(1.0f), (birdTexture != 0), birdUV, birdTexture);
            }

            // 4. Render Sky Collectibles (Coins, Fruit Shields & Direction Arrow Relics)
            for (const auto& itm : skyItemsList) {
                if (itm.collected) continue;
                if (itm.type == 0) {
                    glm::mat4 modelCoin = glm::mat4(1.0f);
                    modelCoin = glm::translate(modelCoin, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
                    modelCoin = glm::scale(modelCoin, glm::vec3(0.9f, 0.9f, 1.0f));
                    gameRenderer.drawQuad(modelCoin, view, projection, glm::vec4(1.0f), (coinTexture != 0), getCoinUV(0), coinTexture);
                } else if (itm.type == 1) {
                    glm::mat4 modelFruit = glm::mat4(1.0f);
                    modelFruit = glm::translate(modelFruit, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
                    modelFruit = glm::scale(modelFruit, glm::vec3(1.2f, 1.2f, 1.0f));
                    gameRenderer.drawQuad(modelFruit, view, projection, glm::vec4(1.0f), (fruitTexture != 0), getFruitUV(itm.fruitType), fruitTexture);
                } else if (itm.type == 2) {
                    RunDirection relicDir = static_cast<RunDirection>(itm.fruitType);
                    float pulse = 1.65f + 0.20f * std::sin(static_cast<float>(glfwGetTime()) * 5.0f);
                    glm::mat4 modelRelic = glm::mat4(1.0f);
                    modelRelic = glm::translate(modelRelic, glm::vec3(itm.pos.x, itm.pos.y, 0.0f));
                    modelRelic = glm::scale(modelRelic, glm::vec3(pulse, pulse, 1.0f));

                    glm::vec4 arrowUV = getDirectionArrowUV(relicDir);
                    glm::vec4 arrowTint(1.0f);
                    if (relicDir == DIR_LEFT)       arrowTint = glm::vec4(0.2f, 0.95f, 1.0f, 1.0f);
                    else if (relicDir == DIR_RIGHT) arrowTint = glm::vec4(1.0f, 0.85f, 0.2f, 1.0f);
                    else if (relicDir == DIR_UP)    arrowTint = glm::vec4(1.0f, 0.35f, 0.35f, 1.0f);

                    gameRenderer.drawQuad(modelRelic, view, projection, arrowTint, (directionArrowsTexture != 0), arrowUV, directionArrowsTexture);
                }
            }

            // 5. Render Character in Sky-Fall (or Take Hit rotated 90 deg CCW!)
            CharacterAnimSet& activeSet = samuraiAnim;
            if (isSkyFallHit) {
                // Show Take Hit rotated 90 degrees anti-clockwise!
                unsigned int hitTex = (activeSet.texTakeHit != 0) ? activeSet.texTakeHit : activeSet.texDeath;
                int totalFrames = (activeSet.texTakeHit != 0) ? activeSet.takeHitFrames : activeSet.deathFrames;
                int fIdx = std::clamp(static_cast<int>(skyFallHitTimer * 6.0f), 0, totalFrames - 1);
                float u = static_cast<float>(fIdx) / static_cast<float>(totalFrames);
                float du = 1.0f / static_cast<float>(totalFrames);
                glm::vec4 uvRect(u, 0.0f, du, 1.0f);

                glm::mat4 modelHero = glm::mat4(1.0f);
                modelHero = glm::translate(modelHero, glm::vec3(skyFallPos.x, skyFallPos.y, 0.0f));
                modelHero = glm::rotate(modelHero, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 90 DEGREE ANTICLOCKWISE ROTATION!
                modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
                gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f, 0.6f, 0.6f, 1.0f), (hitTex != 0), uvRect, hitTex);

                if (skyFallDarkAlpha > 0.01f) {
                    glm::mat4 modelDark = glm::mat4(1.0f);
                    modelDark = glm::translate(modelDark, glm::vec3(cameraPos.x, cameraPos.y, 0.0f));
                    modelDark = glm::scale(modelDark, glm::vec3(30.0f, 20.0f, 1.0f));
                    gameRenderer.drawQuad(modelDark, view, projection, glm::vec4(0.0f, 0.0f, 0.0f, skyFallDarkAlpha), false, glm::vec4(0), 0);
                }
            } else {
                // Falling pose
                unsigned int activeTex = activeSet.texFall;
                int totalFrames = activeSet.fallFrames;
                int fIdx = 0;
                float u = static_cast<float>(fIdx) / static_cast<float>(totalFrames);
                float du = 1.0f / static_cast<float>(totalFrames);
                bool faceLeft = (skyFallVelX < -0.1f);
                glm::vec4 uvRect = faceLeft ? glm::vec4(u + du, 0.0f, -du, 1.0f) : glm::vec4(u, 0.0f, du, 1.0f);

                glm::mat4 modelHero = glm::mat4(1.0f);
                modelHero = glm::translate(modelHero, glm::vec3(skyFallPos.x, skyFallPos.y, 0.0f));
                modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
                gameRenderer.drawQuad(modelHero, view, projection, glm::vec4(1.0f), (activeTex != 0), uvRect, activeTex);

                if (shieldTimer > 0.0f) {
                    float shieldPulse = 1.0f + std::sin(animTimer * 8.0f) * 0.06f;
                    glm::mat4 modelShield = glm::mat4(1.0f);
                    modelShield = glm::translate(modelShield, glm::vec3(skyFallPos.x, skyFallPos.y, 0.0f));
                    modelShield = glm::scale(modelShield, glm::vec3(2.35f * shieldPulse, 2.35f * shieldPulse, 1.0f));
                    float sAlpha = std::min(0.70f, 0.35f + (shieldTimer / 10.0f) * 0.35f);
                    gameRenderer.drawCircle(modelShield, view, projection, glm::vec4(0.2f, 0.85f, 1.0f, sAlpha), 2.2f);
                }
            }
        }
        else
        {
            // -----------------------------------------------------------------
            // Standard Platformer Runner World Rendering
            // -----------------------------------------------------------------
            // 1. Celestial Objects in Sky (Twinkling Stars, Moon, Sun)
            if (playerPos.y > -2.5f) {
                // Twinkling Stars (visible at night)
                if (daylightFactor < 0.75f) {
                    float starAlpha = (1.0f - daylightFactor) * 0.95f;
                    for (const auto& star : skyStars) {
                        float tw = 0.5f + 0.5f * std::sin(glfwGetTime() * star.twinkleFreq + star.twinklePhase);
                        float a = starAlpha * tw;
                        if (a > 0.04f) {
                            glm::mat4 modelStar = glm::mat4(1.0f);
                            modelStar = glm::translate(modelStar, glm::vec3(cameraPos.x * 0.05f + star.x, cameraPos.y * 0.05f + star.y, 0.0f));
                            modelStar = glm::scale(modelStar, glm::vec3(star.size, star.size, 1.0f));
                            gameRenderer.drawQuad(modelStar, view, projection, glm::vec4(1.0f, 1.0f, 0.95f, a), false, glm::vec4(0), 0);
                        }
                    }
                }

                // Celestial Moon (Night)
                if (daylightFactor < 0.75f) {
                    float moonAlpha = (1.0f - daylightFactor);
                    glm::vec3 moonPos(cameraPos.x * 0.06f + 5.5f, cameraPos.y * 0.06f + 4.2f, 0.0f);
                    // Soft lunar glow
                    glm::mat4 modelGlow = glm::mat4(1.0f);
                    modelGlow = glm::translate(modelGlow, moonPos);
                    modelGlow = glm::scale(modelGlow, glm::vec3(2.6f, 2.6f, 1.0f));
                    gameRenderer.drawQuad(modelGlow, view, projection, glm::vec4(0.65f, 0.8f, 1.0f, moonAlpha * 0.28f), false, glm::vec4(0), 0);
                    // Moon core disk
                    glm::mat4 modelMoon = glm::mat4(1.0f);
                    modelMoon = glm::translate(modelMoon, moonPos);
                    modelMoon = glm::scale(modelMoon, glm::vec3(1.6f, 1.6f, 1.0f));
                    gameRenderer.drawQuad(modelMoon, view, projection, glm::vec4(0.92f, 0.95f, 1.0f, moonAlpha * 0.95f), false, glm::vec4(0), 0);
                }

                // Celestial Sun (Day)
                if (daylightFactor > 0.25f) {
                    float sunAlpha = daylightFactor;
                    glm::vec3 sunPos(cameraPos.x * 0.06f - 5.5f, cameraPos.y * 0.06f + 4.5f, 0.0f);
                    // Solar flare halo
                    glm::mat4 modelHalo = glm::mat4(1.0f);
                    modelHalo = glm::translate(modelHalo, sunPos);
                    modelHalo = glm::scale(modelHalo, glm::vec3(3.2f, 3.2f, 1.0f));
                    gameRenderer.drawQuad(modelHalo, view, projection, glm::vec4(1.0f, 0.85f, 0.4f, sunAlpha * 0.35f), false, glm::vec4(0), 0);
                    // Sun core disk
                    glm::mat4 modelSun = glm::mat4(1.0f);
                    modelSun = glm::translate(modelSun, sunPos);
                    modelSun = glm::scale(modelSun, glm::vec3(2.0f, 2.0f, 1.0f));
                    gameRenderer.drawQuad(modelSun, view, projection, glm::vec4(1.0f, 0.96f, 0.75f, sunAlpha * 0.98f), false, glm::vec4(0), 0);
                }
            }

            // 2. Thematic Parallax Scenery Layer (Silvergrass, Autumn, Forest, Urban)
            glm::vec4 nightSceneryTint(0.42f, 0.48f, 0.70f, 1.0f);
            glm::vec4 sunsetSceneryTint(1.08f, 0.72f, 0.58f, 1.0f);
            glm::vec4 daySceneryTint(1.02f, 1.0f, 0.98f, 1.0f);
            glm::vec4 curSceneryTint = glm::mix(nightSceneryTint, daySceneryTint, daylightFactor);
            curSceneryTint = glm::mix(curSceneryTint, sunsetSceneryTint, twilightFactor * 0.55f);

            float sceneryW = 26.0f;
            float sceneryH = 13.0f;
            float sceneryY = 3.2f;
            float parallaxFactor = 0.32f;
            float pOffset = cameraPos.x * parallaxFactor;
            float startSceneryX = std::floor((cameraPos.x - pOffset) / sceneryW) * sceneryW + pOffset;

            for (int i = -2; i <= 2; ++i) {
                float sx = startSceneryX + i * sceneryW;
                glm::mat4 modelScenery = glm::mat4(1.0f);
                modelScenery = glm::translate(modelScenery, glm::vec3(sx, sceneryY, 0.0f));
                modelScenery = glm::scale(modelScenery, glm::vec3(sceneryW, sceneryH, 1.0f));
                gameRenderer.drawQuad(modelScenery, view, projection, curSceneryTint,
                                      (texScenery[activeScenery] != 0), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texScenery[activeScenery]);
            }

            // 3. Ambient Parallax Silhouettes (Distant depth pillars)
            for (int i = -10; i <= 10; ++i) {
                float bgX = std::floor(cameraPos.x / 14.0f) * 14.0f + (i * 14.0f);
                glm::mat4 modelBg = glm::mat4(1.0f);
                modelBg = glm::translate(modelBg, glm::vec3(bgX, 0.0f, 0.0f));
                modelBg = glm::scale(modelBg, glm::vec3(1.5f, 36.0f, 1.0f));
                gameRenderer.drawQuad(modelBg, view, projection, curPillarColor * curSceneryTint, false, glm::vec4(0), 0);
            }

            // 2. Render Vertical Ladder Tunnel Shaftways
            for (const auto& tunnel : gameWorld.tunnels)
            {
                float tunnelH = tunnel.maxY - tunnel.minY;
                float tunnelMidY = tunnel.minY + tunnelH * 0.5f;

                glm::mat4 modelShaft = glm::mat4(1.0f);
                modelShaft = glm::translate(modelShaft, glm::vec3(tunnel.x, tunnelMidY, 0.0f));
                modelShaft = glm::scale(modelShaft, glm::vec3(tunnel.width, tunnelH + 1.0f, 1.0f));
                gameRenderer.drawQuad(modelShaft, view, projection, glm::vec4(0.02f, 0.03f, 0.06f, 0.88f), false, glm::vec4(0), 0);

                if (tunnel.playerInside || std::abs(playerPos.x - tunnel.x) < 2.0f) {
                    glm::mat4 modelGlow = glm::mat4(1.0f);
                    modelGlow = glm::translate(modelGlow, glm::vec3(tunnel.x, tunnelMidY, 0.0f));
                    modelGlow = glm::scale(modelGlow, glm::vec3(0.12f, tunnelH, 1.0f));
                    gameRenderer.drawQuad(modelGlow, view, projection, glm::vec4(0.3f, 0.95f, 0.6f, 0.45f), false, glm::vec4(0), 0);
                }
            }

            // 3. Render World Platform & Ladder Tiles
            for (const auto& tile : gameWorld.tiles)
            {
                glm::vec4 uv = getTilesetUV(tile.tileX, tile.tileY);
                glm::mat4 modelTile = glm::mat4(1.0f);
                modelTile = glm::translate(modelTile, glm::vec3(tile.pos.x, tile.pos.y, 0.0f));
                modelTile = glm::scale(modelTile, glm::vec3(tile.size.x, tile.size.y, 1.0f));
                gameRenderer.drawQuad(modelTile, view, projection, curTileTint, (worldTilesetTexture != 0), uv, worldTilesetTexture);
            }

            // 4. Render Steep Slopes
            for (const auto& slope : gameWorld.slopes)
            {
                float midX = (slope.startX + slope.endX) * 0.5f;
                float midY = (slope.startY + slope.endY) * 0.5f;
                float dx = slope.endX - slope.startX;
                float dy = slope.endY - slope.startY;
                float length = std::sqrt(dx * dx + dy * dy);

                glm::mat4 modelSlope = glm::mat4(1.0f);
                modelSlope = glm::translate(modelSlope, glm::vec3(midX, midY, 0.0f));
                modelSlope = glm::rotate(modelSlope, slope.angle, glm::vec3(0.0f, 0.0f, 1.0f));
                modelSlope = glm::scale(modelSlope, glm::vec3(length, 0.65f, 1.0f));

                glm::vec4 slopeUV = getTilesetUV(1, 0);
                gameRenderer.drawQuad(modelSlope, view, projection, curTileTint, (worldTilesetTexture != 0), slopeUV, worldTilesetTexture);
            }

            // 5. Render Spiky Traps
            glm::vec4 spikeUV = getTilesetUV(4, 8);
            for (const auto& spike : gameWorld.spikes)
            {
                glm::mat4 modelSpike = glm::mat4(1.0f);
                modelSpike = glm::translate(modelSpike, glm::vec3(spike.pos.x, spike.pos.y, 0.0f));
                modelSpike = glm::scale(modelSpike, glm::vec3(spike.size.x, spike.size.y, 1.0f));
                gameRenderer.drawQuad(modelSpike, view, projection, glm::vec4(1.0f, 0.85f, 0.85f, 1.0f), (worldTilesetTexture != 0), spikeUV, worldTilesetTexture);
            }

            // 6. Render Direction Relics (Clear Arrowhead Symbols: Left, Right, Up, Down)
            for (const auto& relic : gameWorld.relics)
            {
                if (relic.collected) continue;
                float hoverY = relic.pos.y + std::sin(relic.animPhase) * 0.18f;
                float pulseScale = 1.0f + std::cos(relic.animPhase * 1.5f) * 0.10f;

                // Glowing directional aura tint
                glm::vec4 auraCol(0.2f, 0.85f, 1.0f, 0.35f);
                if (relic.targetDir == DIR_LEFT) auraCol = glm::vec4(1.0f, 0.55f, 0.15f, 0.35f);
                else if (relic.targetDir == DIR_RIGHT) auraCol = glm::vec4(0.1f, 0.85f, 1.0f, 0.35f);
                else if (relic.targetDir == DIR_UP) auraCol = glm::vec4(0.2f, 0.95f, 0.45f, 0.35f);
                else if (relic.targetDir == DIR_DOWN) auraCol = glm::vec4(1.0f, 0.85f, 0.2f, 0.35f);

                glm::mat4 modelAura = glm::mat4(1.0f);
                modelAura = glm::translate(modelAura, glm::vec3(relic.pos.x, hoverY, 0.0f));
                modelAura = glm::scale(modelAura, glm::vec3(1.5f * pulseScale, 1.5f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelAura, view, projection, auraCol, false, glm::vec4(0), 0);

                glm::vec4 arrowUV = getDirectionArrowUV(relic.targetDir);
                glm::mat4 modelRelic = glm::mat4(1.0f);
                modelRelic = glm::translate(modelRelic, glm::vec3(relic.pos.x, hoverY, 0.0f));
                modelRelic = glm::scale(modelRelic, glm::vec3(1.35f * pulseScale, 1.35f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelRelic, view, projection, glm::vec4(1.0f), (directionArrowsTexture != 0), arrowUV, directionArrowsTexture);
            }

            // 6b. Render Fruit Shield Pickups (Gives 10s Temporary Shield)
            for (const auto& fruit : gameWorld.fruits)
            {
                if (fruit.collected) continue;
                float hoverY = fruit.pos.y + std::sin(fruit.animPhase) * 0.18f;
                float pulseScale = 1.0f + std::cos(fruit.animPhase * 2.0f) * 0.10f;

                glm::mat4 modelAura = glm::mat4(1.0f);
                modelAura = glm::translate(modelAura, glm::vec3(fruit.pos.x, hoverY, 0.0f));
                modelAura = glm::scale(modelAura, glm::vec3(1.4f * pulseScale, 1.4f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelAura, view, projection, glm::vec4(0.2f, 0.9f, 1.0f, 0.30f), false, glm::vec4(0), 0);

                glm::vec4 fruitUV = getFruitUV(fruit.fruitType);
                glm::mat4 modelFruit = glm::mat4(1.0f);
                modelFruit = glm::translate(modelFruit, glm::vec3(fruit.pos.x, hoverY, 0.0f));
                modelFruit = glm::scale(modelFruit, glm::vec3(1.15f * pulseScale, 1.15f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelFruit, view, projection, glm::vec4(1.0f), (fruitTexture != 0), fruitUV, fruitTexture);
            }

            // 7. Render Animated Gold Coins
            for (const auto& coin : gameWorld.coins)
            {
                if (coin.collected) continue;
                int frame = static_cast<int>(coin.animTimer);
                glm::vec4 coinUV = getCoinUV(frame);

                glm::mat4 modelCoin = glm::mat4(1.0f);
                modelCoin = glm::translate(modelCoin, glm::vec3(coin.pos.x, coin.pos.y, 0.0f));
                modelCoin = glm::scale(modelCoin, glm::vec3(0.9f, 0.9f, 1.0f));
                gameRenderer.drawQuad(modelCoin, view, projection, glm::vec4(1.0f), (coinTexture != 0), coinUV, coinTexture);
            }

            // 8. Render Animated Character Sprite (Samurai Mack)
            CharacterAnimSet& activeSet = samuraiAnim;
            unsigned int activeTexture = activeSet.texRun;
            int totalFrames = activeSet.runFrames;
            int frameIndex = 0;

            bool faceLeft = (currentDirection == DIR_LEFT || (isOnLadder && lastHorizontalDir == DIR_LEFT));

            if (currentGameState == STATE_GAME_OVER) {
                activeTexture = activeSet.texDeath;
                totalFrames = activeSet.deathFrames;
                int dFrame = static_cast<int>(deathAnimTimer);
                frameIndex = std::min(dFrame, totalFrames - 1);
            } else if (isSliding) {
                activeTexture = activeSet.texAttack;
                totalFrames = activeSet.attackFrames;
                frameIndex = std::min(1, totalFrames - 1);
            } else if (isOnLadder) {
                if (std::abs(playerVel.y) > 0.2f) {
                    int climbStep = static_cast<int>(animTimer * 3.5f) % 2;
                    activeTexture = (climbStep == 0) ? activeSet.texJump : activeSet.texFall;
                    totalFrames = (climbStep == 0) ? activeSet.jumpFrames : activeSet.fallFrames;
                    frameIndex = 0;
                } else {
                    activeTexture = activeSet.texJump;
                    totalFrames = activeSet.jumpFrames;
                    frameIndex = 0;
                }
            } else if (!isGrounded && playerVel.y >= 0.0f) {
                activeTexture = activeSet.texJump;
                totalFrames = activeSet.jumpFrames;
                frameIndex = (playerVel.y > 4.0f) ? 0 : 1;
            } else if (!isGrounded && playerVel.y < 0.0f) {
                activeTexture = activeSet.texFall;
                totalFrames = activeSet.fallFrames;
                frameIndex = (playerVel.y < -7.0f) ? 1 : 0;
            } else if (isGameIdleAtStart || (freeExplorationMode && std::abs(playerVel.x) <= 0.5f)) {
                activeTexture = activeSet.texIdle;
                totalFrames = activeSet.idleFrames;
                frameIndex = static_cast<int>(animTimer * 4.0f) % totalFrames;
            } else if (std::abs(playerVel.x) > 0.5f) {
                activeTexture = activeSet.texRun;
                totalFrames = activeSet.runFrames;
                frameIndex = static_cast<int>(animTimer * 8.0f) % totalFrames;
            } else {
                activeTexture = activeSet.texIdle;
                totalFrames = activeSet.idleFrames;
                frameIndex = static_cast<int>(animTimer * 4.0f) % totalFrames;
            }

            float u = static_cast<float>(frameIndex) / static_cast<float>(totalFrames);
            float du = 1.0f / static_cast<float>(totalFrames);
            glm::vec4 uvRect = faceLeft ? glm::vec4(u + du, 0.0f, -du, 1.0f) : glm::vec4(u, 0.0f, du, 1.0f);

            // Render Dash Ghosts
            for (const auto& ghost : dashGhosts) {
                glm::mat4 modelGhost = glm::mat4(1.0f);
                modelGhost = glm::translate(modelGhost, glm::vec3(ghost.pos.x, ghost.pos.y - 0.10f, 0.0f));
                modelGhost = glm::scale(modelGhost, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));
                glm::vec4 ghostColor(0.2f, 0.85f, 1.0f, ghost.alpha);
                gameRenderer.drawQuad(modelGhost, view, projection, ghostColor, (activeSet.texRun != 0), uvRect, activeSet.texRun);
            }

            glm::mat4 modelPlayer = glm::mat4(1.0f);
            modelPlayer = glm::translate(modelPlayer, glm::vec3(playerPos.x, playerPos.y - 0.10f, 0.0f));
            if (std::abs(playerSpriteAngle) > 0.01f) {
                modelPlayer = glm::rotate(modelPlayer, playerSpriteAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            modelPlayer = glm::scale(modelPlayer, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));

            glm::vec4 playerTint = (currentGameState == STATE_GAME_OVER) ?
                                   glm::vec4(1.0f, 0.45f, 0.45f, 1.0f) :
                                   (isDashing ? glm::vec4(0.4f, 0.9f, 1.0f, 1.0f) : glm::vec4(1.0f));

            gameRenderer.drawQuad(modelPlayer, view, projection, playerTint, (activeTexture != 0), uvRect, activeTexture);

            // Render Circular Glowing Shield Bubble if active
            if (shieldTimer > 0.0f) {
                float shieldPulse = 1.0f + std::sin(animTimer * 8.0f) * 0.06f;
                glm::mat4 modelShield = glm::mat4(1.0f);
                modelShield = glm::translate(modelShield, glm::vec3(playerPos.x, playerPos.y, 0.0f));
                modelShield = glm::scale(modelShield, glm::vec3(2.35f * shieldPulse, 2.35f * shieldPulse, 1.0f));
                float sAlpha = std::min(0.70f, 0.35f + (shieldTimer / 10.0f) * 0.35f);
                gameRenderer.drawCircle(modelShield, view, projection, glm::vec4(0.2f, 0.85f, 1.0f, sAlpha), 2.2f);
            }
        }

        // 8b. Render World-Space Floating Texts (Pickups, Direction Warps, Score Popups)
        for (const auto& ft : floatingTexts) {
            float alpha = std::min(1.0f, ft.timer / (ft.maxTime * 0.35f));
            glm::vec4 col = ft.color;
            col.a *= alpha;

            size_t newlinePos = ft.text.find('\n');
            size_t firstLineChars = (newlinePos != std::string::npos) ? newlinePos : ft.text.length();
            float totalW = firstLineChars * ft.charW;
            float startX = ft.pos.x - (totalW * 0.5f);

            gameRenderer.drawText(ft.text, startX, ft.pos.y, ft.charW, ft.charH, col, projection, view);
        }

        // 9. Transition Phase Energy Flash
        if (transitionPhaseTimer > 0.0f) {
            float progress = transitionPhaseTimer / transitionPhaseMax;
            float flashAlpha = std::sin(progress * 3.14159f) * 0.22f;
            glm::mat4 identityView = glm::mat4(1.0f);
            glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

            glm::mat4 modelFlash = glm::mat4(1.0f);
            modelFlash = glm::scale(modelFlash, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
            gameRenderer.drawQuad(modelFlash, identityView, orthoUI, glm::vec4(1.0f, 1.0f, 1.0f, flashAlpha), false, glm::vec4(0), 0);
        }

        // 10. Comprehensible Corner HUD Card
        if (activeNotice.timer > 0.0f)
        {
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
            gameRenderer.drawQuad(modelCard, identityView, orthoUI, glm::vec4(0.03f, 0.05f, 0.09f, 0.92f * alpha), false, glm::vec4(0), 0);

            glm::mat4 modelStripe = glm::mat4(1.0f);
            modelStripe = glm::translate(modelStripe, glm::vec3(cardX - (cardW * 0.5f) + 0.08f, cardY, 0.0f));
            modelStripe = glm::scale(modelStripe, glm::vec3(0.16f, cardH, 1.0f));
            gameRenderer.drawQuad(modelStripe, identityView, orthoUI, glm::vec4(activeNotice.accentColor.r, activeNotice.accentColor.g, activeNotice.accentColor.b, alpha), false, glm::vec4(0), 0);

            glm::mat4 modelIcon = glm::mat4(1.0f);
            modelIcon = glm::translate(modelIcon, glm::vec3(cardX - (cardW * 0.5f) + 0.6f, cardY, 0.0f));
            modelIcon = glm::scale(modelIcon, glm::vec3(0.9f, 0.9f, 1.0f));

            if (activeNotice.iconType == 0) {
                glm::vec4 coinUV = getCoinUV(frameCount / 4);
                gameRenderer.drawQuad(modelIcon, identityView, orthoUI, glm::vec4(1.0f, 1.0f, 1.0f, alpha), (coinTexture != 0), coinUV, coinTexture);
            } else {
                glm::vec4 fruitUV = getFruitUV(activeNotice.fruitType);
                gameRenderer.drawQuad(modelIcon, identityView, orthoUI, glm::vec4(1.0f, 1.0f, 1.0f, alpha), (fruitTexture != 0), fruitUV, fruitTexture);
            }

            glm::mat4 modelPill = glm::mat4(1.0f);
            modelPill = glm::translate(modelPill, glm::vec3(cardX + 0.55f, cardY + 0.32f, 0.0f));
            modelPill = glm::scale(modelPill, glm::vec3(4.3f, 0.36f, 1.0f));
            gameRenderer.drawQuad(modelPill, identityView, orthoUI, glm::vec4(activeNotice.accentColor.r, activeNotice.accentColor.g, activeNotice.accentColor.b, 0.35f * alpha), false, glm::vec4(0), 0);
        }

        // 11. Top-Right On-Screen Score Counter HUD
        {
            glm::mat4 identityView = glm::mat4(1.0f);
            glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

            int totalScore = static_cast<int>(scoreDistance) * 10 + coinsCollected * 50 + relicsTriggered * 100;

            float scoreCardW = 5.6f;
            float scoreCardH = 1.45f;
            float scoreCardX = halfW - (scoreCardW * 0.5f) - 0.6f;
            float scoreCardY = halfH - (scoreCardH * 0.5f) - 0.6f;

            // Glassmorphism background card
            glm::mat4 modelScoreCard = glm::mat4(1.0f);
            modelScoreCard = glm::translate(modelScoreCard, glm::vec3(scoreCardX, scoreCardY, 0.0f));
            modelScoreCard = glm::scale(modelScoreCard, glm::vec3(scoreCardW, scoreCardH, 1.0f));
            gameRenderer.drawQuad(modelScoreCard, identityView, orthoUI, glm::vec4(0.03f, 0.05f, 0.09f, 0.92f), false, glm::vec4(0), 0);

            // Glowing Golden Accent Stripe on Right Edge
            glm::mat4 modelScoreStripe = glm::mat4(1.0f);
            modelScoreStripe = glm::translate(modelScoreStripe, glm::vec3(scoreCardX + (scoreCardW * 0.5f) - 0.08f, scoreCardY, 0.0f));
            modelScoreStripe = glm::scale(modelScoreStripe, glm::vec3(0.16f, scoreCardH, 1.0f));
            gameRenderer.drawQuad(modelScoreStripe, identityView, orthoUI, glm::vec4(1.0f, 0.82f, 0.20f, 1.0f), false, glm::vec4(0), 0);

            // Spinning Gold Coin Icon
            glm::mat4 modelCoinHUD = glm::mat4(1.0f);
            modelCoinHUD = glm::translate(modelCoinHUD, glm::vec3(scoreCardX - (scoreCardW * 0.5f) + 0.6f, scoreCardY, 0.0f));
            modelCoinHUD = glm::scale(modelCoinHUD, glm::vec3(0.9f, 0.9f, 1.0f));
            glm::vec4 coinHUDUV = getCoinUV(frameCount / 4);
            gameRenderer.drawQuad(modelCoinHUD, identityView, orthoUI, glm::vec4(1.0f), (coinTexture != 0), coinHUDUV, coinTexture);

            // Text Lines (Crisp 8x8 font rendered on screen)
            std::string scoreStr = "SCORE " + std::to_string(totalScore);
            std::string statsStr = "COIN " + std::to_string(coinsCollected) + "  DIST " + std::to_string(static_cast<int>(scoreDistance)) + "m";
            if (shieldTimer > 0.0f) {
                statsStr += "  SHIELD " + std::to_string(static_cast<int>(shieldTimer + 0.9f)) + "s";
            }
            gameRenderer.drawText(scoreStr, scoreCardX - (scoreCardW * 0.5f) + 1.25f, scoreCardY + 0.16f, 0.26f, 0.38f,
                                  glm::vec4(1.0f, 0.86f, 0.22f, 1.0f), orthoUI);
            gameRenderer.drawText(statsStr, scoreCardX - (scoreCardW * 0.5f) + 1.25f, scoreCardY - 0.38f, 0.20f, 0.30f,
                                  (shieldTimer > 0.0f) ? glm::vec4(0.3f, 0.95f, 1.0f, 1.0f) : glm::vec4(0.85f, 0.94f, 1.0f, 0.95f), orthoUI);
        }

        // On-Screen Start Hint Banner (when standing still in idle animation at game start)
        if (currentScene == SCENE_PLATFORMER && isGameIdleAtStart && currentGameState == STATE_RUNNING) {
            glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
            float bannerPulse = 0.82f + 0.18f * std::sin(animTimer * 5.0f);
            gameRenderer.drawText("PRESS [A] OR [D] TO RUN  |  [SPACE] TO JUMP", -6.4f, -2.6f, 0.24f, 0.34f,
                                  glm::vec4(1.0f, 0.9f, 0.25f, bannerPulse), orthoUI);
        }

        // 12. Pause Menu Modal Dialog (ESC Key)
        if (currentGameState == STATE_PAUSED)
        {
            glm::mat4 identityView = glm::mat4(1.0f);
            glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

            // Dark translucent overlay backdrop
            glm::mat4 modelOverlay = glm::mat4(1.0f);
            modelOverlay = glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
            gameRenderer.drawQuad(modelOverlay, identityView, orthoUI, glm::vec4(0.02f, 0.03f, 0.06f, 0.78f), false, glm::vec4(0), 0);

            // Modal Box
            float menuW = 12.8f;
            float menuH = 10.7f;
            glm::mat4 modelMenu = glm::mat4(1.0f);
            modelMenu = glm::scale(modelMenu, glm::vec3(menuW, menuH, 1.0f));
            gameRenderer.drawQuad(modelMenu, identityView, orthoUI, glm::vec4(0.05f, 0.07f, 0.12f, 0.96f), false, glm::vec4(0), 0);

            // Golden Top Header Bar
            glm::mat4 modelHeaderStripe = glm::mat4(1.0f);
            modelHeaderStripe = glm::translate(modelHeaderStripe, glm::vec3(0.0f, menuH * 0.5f - 0.1f, 0.0f));
            modelHeaderStripe = glm::scale(modelHeaderStripe, glm::vec3(menuW, 0.2f, 1.0f));
            gameRenderer.drawQuad(modelHeaderStripe, identityView, orthoUI, glm::vec4(1.0f, 0.82f, 0.20f, 1.0f), false, glm::vec4(0), 0);

            int totalScore = static_cast<int>(scoreDistance) * 10 + coinsCollected * 50 + relicsTriggered * 100;
            float textX = -menuW * 0.5f + 0.9f;
            float curY  = menuH * 0.5f - 0.9f;

            gameRenderer.drawText("=== GAME PAUSED ===", textX + 1.8f, curY, 0.34f, 0.48f,
                                  glm::vec4(1.0f, 0.85f, 0.25f, 1.0f), orthoUI);
            curY -= 0.80f;

            gameRenderer.drawText("HERO: SAMURAI MACK [KATANA MASTER]", textX, curY, 0.22f, 0.32f,
                                  glm::vec4(0.35f, 0.95f, 1.0f, 1.0f), orthoUI);
            curY -= 0.48f;

            gameRenderer.drawText("TOTAL SCORE: " + std::to_string(totalScore) + " PTS", textX, curY, 0.23f, 0.34f,
                                  glm::vec4(1.0f, 0.9f, 0.35f, 1.0f), orthoUI);
            curY -= 0.45f;

            gameRenderer.drawText("DISTANCE: " + std::to_string(static_cast<int>(scoreDistance)) + "m   COINS: " +
                                  std::to_string(coinsCollected) + "   RELICS: " + std::to_string(relicsTriggered),
                                  textX, curY, 0.19f, 0.28f, glm::vec4(0.85f, 0.92f, 0.98f, 0.95f), orthoUI);
            curY -= 0.55f;

            // Thin divider
            glm::mat4 modelDiv = glm::mat4(1.0f);
            modelDiv = glm::translate(modelDiv, glm::vec3(0.0f, curY + 0.12f, 0.0f));
            modelDiv = glm::scale(modelDiv, glm::vec3(menuW - 1.8f, 0.05f, 1.0f));
            gameRenderer.drawQuad(modelDiv, identityView, orthoUI, glm::vec4(0.3f, 0.45f, 0.6f, 0.6f), false, glm::vec4(0), 0);
            curY -= 0.42f;

            gameRenderer.drawText("[ESC / SPACE]   RESUME GAME", textX, curY, 0.23f, 0.32f,
                                  glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), orthoUI);
            curY -= 0.48f;

            std::string modeMenuStr = freeExplorationMode ? "FREE EXPLORATION [MANUAL A/D]" : "AUTO-RUNNER [CONTINUOUS SPRINT]";
            glm::vec4 modeMenuCol = freeExplorationMode ? glm::vec4(0.35f, 1.0f, 0.55f, 1.0f) : glm::vec4(0.25f, 0.85f, 1.0f, 1.0f);
            gameRenderer.drawText("[F]             MODE: " + modeMenuStr, textX, curY, 0.21f, 0.30f,
                                  modeMenuCol, orthoUI);
            curY -= 0.48f;

            std::string scenStr = getSceneryName(activeScenery);
            gameRenderer.drawText("[B]             SCENERY: " + scenStr, textX, curY, 0.23f, 0.32f,
                                  glm::vec4(0.3f, 0.95f, 0.85f, 1.0f), orthoUI);
            curY -= 0.48f;

            std::string cycleStr = (isNight ? "NIGHT TIME (" : "DAY TIME (") + std::to_string(static_cast<int>(DAY_NIGHT_CYCLE_DURATION - phaseTime)) + "s left)";
            gameRenderer.drawText("[N]             CYCLE: " + cycleStr, textX, curY, 0.23f, 0.32f,
                                  isNight ? glm::vec4(0.5f, 0.75f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.85f, 0.35f, 1.0f), orthoUI);
            curY -= 0.48f;

            std::string autoShiftStatus = autoShiftEnabled ? "ENABLED (TIMER + RELICS)" : "DISABLED (RELICS ONLY)";
            glm::vec4 autoShiftCol = autoShiftEnabled ? glm::vec4(1.0f, 0.65f, 0.2f, 1.0f) : glm::vec4(0.4f, 0.95f, 1.0f, 1.0f);
            gameRenderer.drawText("[T]             AUTO SHIFTS: " + autoShiftStatus, textX, curY, 0.21f, 0.30f,
                                  autoShiftCol, orthoUI);
            curY -= 0.48f;

            gameRenderer.drawText("[R]             RESTART RUN", textX, curY, 0.23f, 0.32f,
                                  glm::vec4(0.95f, 0.85f, 0.3f, 1.0f), orthoUI);
            curY -= 0.48f;

            gameRenderer.drawText("[Q]             QUIT TO DESKTOP", textX, curY, 0.23f, 0.32f,
                                  glm::vec4(1.0f, 0.4f, 0.4f, 1.0f), orthoUI);
            curY -= 0.56f;

            gameRenderer.drawText("CONTROLS: [A/D] MOVE  [W/SPACE] JUMP/CLIMB  [S] SLIDE/DROP  [SHIFT] DASH  [F] FREE EXPLORE",
                                  textX - 0.45f, curY, 0.15f, 0.22f, glm::vec4(0.6f, 0.7f, 0.85f, 0.8f), orthoUI);
        }

        // 13. Game Over Screen
        if (currentGameState == STATE_GAME_OVER)
        {
            glm::mat4 identityView = glm::mat4(1.0f);
            glm::mat4 orthoUI = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

            glm::mat4 modelOverlay = glm::mat4(1.0f);
            modelOverlay = glm::scale(modelOverlay, glm::vec3(halfW * 2.0f, halfH * 2.0f, 1.0f));
            gameRenderer.drawQuad(modelOverlay, identityView, orthoUI, glm::vec4(0.12f, 0.02f, 0.04f, 0.82f), false, glm::vec4(0), 0);

            float overW = 11.5f;
            float overH = 7.0f;
            glm::mat4 modelOver = glm::mat4(1.0f);
            modelOver = glm::scale(modelOver, glm::vec3(overW, overH, 1.0f));
            gameRenderer.drawQuad(modelOver, identityView, orthoUI, glm::vec4(0.08f, 0.03f, 0.05f, 0.96f), false, glm::vec4(0), 0);

            glm::mat4 modelOverStripe = glm::mat4(1.0f);
            modelOverStripe = glm::translate(modelOverStripe, glm::vec3(0.0f, overH * 0.5f - 0.1f, 0.0f));
            modelOverStripe = glm::scale(modelOverStripe, glm::vec3(overW, 0.2f, 1.0f));
            gameRenderer.drawQuad(modelOverStripe, identityView, orthoUI, glm::vec4(1.0f, 0.25f, 0.3f, 1.0f), false, glm::vec4(0), 0);

            int totalScore = static_cast<int>(scoreDistance) * 10 + coinsCollected * 50 + relicsTriggered * 100;
            float textX = -overW * 0.5f + 1.0f;
            float curY  = overH * 0.5f - 0.9f;

            gameRenderer.drawText("=== GAME OVER ===", textX + 2.0f, curY, 0.34f, 0.48f,
                                  glm::vec4(1.0f, 0.28f, 0.32f, 1.0f), orthoUI);
            curY -= 0.85f;

            gameRenderer.drawText("CAUSE: " + gameOverReason, textX, curY, 0.22f, 0.32f,
                                  glm::vec4(1.0f, 0.7f, 0.7f, 1.0f), orthoUI);
            curY -= 0.65f;

            gameRenderer.drawText("FINAL SCORE : " + std::to_string(totalScore) + " PTS", textX, curY, 0.26f, 0.38f,
                                  glm::vec4(1.0f, 0.88f, 0.22f, 1.0f), orthoUI);
            curY -= 0.60f;

            gameRenderer.drawText("DISTANCE    : " + std::to_string(static_cast<int>(scoreDistance)) + " METERS",
                                  textX, curY, 0.20f, 0.30f, glm::vec4(0.9f, 0.95f, 1.0f, 1.0f), orthoUI);
            curY -= 0.50f;

            gameRenderer.drawText("GOLD COINS  : " + std::to_string(coinsCollected) + " (+ " +
                                  std::to_string(coinsCollected * 50) + " PTS)",
                                  textX, curY, 0.20f, 0.30f, glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), orthoUI);
            curY -= 0.85f;

            gameRenderer.drawText("PRESS [SPACE] OR [R] TO RETRY RUN", textX + 0.8f, curY, 0.24f, 0.35f,
                                  glm::vec4(0.35f, 1.0f, 0.6f, 1.0f), orthoUI);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup Resources
    introVideoPlayer.close();
    gameRenderer.cleanup();
    if (samuraiAnim.texRun) glDeleteTextures(1, &samuraiAnim.texRun);
    if (samuraiAnim.texIdle) glDeleteTextures(1, &samuraiAnim.texIdle);
    if (samuraiAnim.texJump) glDeleteTextures(1, &samuraiAnim.texJump);
    if (samuraiAnim.texFall) glDeleteTextures(1, &samuraiAnim.texFall);
    if (samuraiAnim.texAttack) glDeleteTextures(1, &samuraiAnim.texAttack);
    if (samuraiAnim.texDeath) glDeleteTextures(1, &samuraiAnim.texDeath);
    if (samuraiAnim.texTakeHit) glDeleteTextures(1, &samuraiAnim.texTakeHit);

    if (worldTilesetTexture) glDeleteTextures(1, &worldTilesetTexture);
    if (platformsTexture) glDeleteTextures(1, &platformsTexture);
    if (coinTexture) glDeleteTextures(1, &coinTexture);
    if (fruitTexture) glDeleteTextures(1, &fruitTexture);
    if (birdTexture) glDeleteTextures(1, &birdTexture);
    if (directionArrowsTexture) glDeleteTextures(1, &directionArrowsTexture);
    for (int s = 0; s < SCENERY_COUNT; ++s) {
        if (texScenery[s]) glDeleteTextures(1, &texScenery[s]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// Process User Input with Pause Menu & Direction Interrupts
// -----------------------------------------------------------------------------
void processInput(GLFWwindow* window, float deltaTime)
{
    (void)deltaTime;
    static bool escPressedLast = false;
    static bool rKeyLast = false;

    // Intro Video Input: Press Space, Enter, or Esc to skip intro
    if (currentGameState == STATE_INTRO_VIDEO) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            introVideoPlayer.skip();
            introVideoPlayer.close();
            currentGameState = STATE_RUNNING;
            isGameIdleAtStart = true;
        }
        return;
    }

    // ESC Key Toggle Pause
    bool escPressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    if (escPressed && !escPressedLast) {
        if (currentGameState == STATE_RUNNING) {
            currentGameState = STATE_PAUSED;
        } else if (currentGameState == STATE_PAUSED) {
            currentGameState = STATE_RUNNING;
        }
    }
    escPressedLast = escPressed;

    // Restart Key (R key)
    bool rKeyPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
    if (rKeyPressed && !rKeyLast) {
        if (currentGameState == STATE_PAUSED || currentGameState == STATE_GAME_OVER) {
            resetGame();
        }
    }
    rKeyLast = rKeyPressed;

    // Toggle Autonomous Direction Shifts (T key)
    static bool tKeyLast = false;
    bool tKeyPressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);
    if (tKeyPressed && !tKeyLast) {
        autoShiftEnabled = !autoShiftEnabled;
        showCornerNotification("SHIFT MODE", autoShiftEnabled ? "AUTO SHIFTS ON" : "AUTO SHIFTS OFF",
                               autoShiftEnabled ? "Timer shifts enabled (every 6.5s)" : "Direction shifts via Mystery Fruits only!",
                               autoShiftEnabled ? glm::vec4(1.0f, 0.65f, 0.2f, 1.0f) : glm::vec4(0.35f, 0.95f, 0.45f, 1.0f), 1, 0);
    }
    tKeyLast = tKeyPressed;

    // Cycle Scenery Theme (B key): Silvergrass -> Autumn -> Forest -> Urban
    static bool bKeyLast = false;
    bool bKeyPressed = (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS);
    if (bKeyPressed && !bKeyLast) {
        setSceneryTheme(static_cast<SceneryTheme>((activeScenery + 1) % SCENERY_COUNT));
    }
    bKeyLast = bKeyPressed;

    // Advance Day/Night Cycle (N key) - Instant half-cycle fast-forward
    static bool nKeyLast = false;
    bool nKeyPressed = (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS);
    if (nKeyPressed && !nKeyLast) {
        dayNightTimer += DAY_NIGHT_CYCLE_DURATION;
        int newIdx = static_cast<int>(dayNightTimer / DAY_NIGHT_CYCLE_DURATION);
        bool nowNight = (newIdx % 2 == 0);
        showCornerNotification("CYCLE", nowNight ? "TOGGLED: NIGHT TIME" : "TOGGLED: DAY TIME",
                               nowNight ? "Moonlight blankets the landscape" : "Sunlight bathes the scenery",
                               nowNight ? glm::vec4(0.45f, 0.65f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.85f, 0.3f, 1.0f), 0);
        spawnFloatingText(nowNight ? "=== NIGHT TIME ===" : "=== DAY TIME ===",
                          (currentScene == SCENE_PLATFORMER ? playerPos + glm::vec2(0.0f, 1.5f) : glm::vec2(0.0f, 1.5f)),
                          nowNight ? glm::vec4(0.5f, 0.75f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.9f, 0.35f, 1.0f));
    }
    nKeyLast = nKeyPressed;

    // Toggle Free Exploration Mode (F key) - Pauses auto-sprint and allows manual A/D platform exploration
    static bool fKeyLast = false;
    bool fKeyPressed = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
    if (fKeyPressed && !fKeyLast && (currentGameState == STATE_RUNNING || currentGameState == STATE_PAUSED)) {
        freeExplorationMode = !freeExplorationMode;
        if (freeExplorationMode) {
            isGameIdleAtStart = false;
            if (!isSliding && !isDashing) {
                playerVel.x = 0.0f;
            }
            audioMgr.play(SFX_SHIFT);
            showCornerNotification("EXPLORATION", "FREE EXPLORE: ON",
                                   "Auto-run paused! Full manual A/D movement control.",
                                   glm::vec4(0.35f, 1.0f, 0.55f, 1.0f), 0);
            spawnFloatingText("FREE EXPLORATION [ON]", playerPos + glm::vec2(0.0f, 1.6f), glm::vec4(0.35f, 1.0f, 0.6f, 1.0f));
        } else {
            float dirSign = (lastHorizontalDir == DIR_LEFT) ? -1.0f : 1.0f;
            currentDirection = lastHorizontalDir;
            if (!isSliding && !isDashing && !isOnLadder) {
                playerVel.x = dirSign * playerSpeed;
            }
            audioMgr.play(SFX_SHIFT);
            showCornerNotification("RUNNER MODE", "AUTO-RUN: RESUMED",
                                   "Continuous platform sprint active! [F to Toggle]",
                                   glm::vec4(0.25f, 0.85f, 1.0f, 1.0f), 0);
            spawnFloatingText("AUTO-RUNNER [ON]", playerPos + glm::vec2(0.0f, 1.6f), glm::vec4(0.2f, 0.85f, 1.0f, 1.0f));
        }
    }
    fKeyLast = fKeyPressed;

    // If Paused:
    if (currentGameState == STATE_PAUSED) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            currentGameState = STATE_RUNNING;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        return;
    }

    if (currentGameState == STATE_GAME_OVER) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            resetGame();
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        return;
    }

    // DASH Input (Left or Right Shift)
    bool shiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    if (shiftPressed && !isDashing && dashCooldown <= 0.0f && currentGameState == STATE_RUNNING) {
        isDashing = true;
        dashTimer = DASH_BURST_TIME;
        dashCooldown = DASH_COOLDOWN_TIME;
        audioMgr.play(SFX_SLIDE);

        if (currentScene == SCENE_PLATFORMER) {
            float dirSign = (lastHorizontalDir == DIR_LEFT) ? -1.0f : 1.0f;
            playerVel.x = dirSign * DASH_SPEED_MAG;
            spawnFloatingText(">> SHADOW DASH >>", playerPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.2f, 0.9f, 1.0f, 1.0f));
        } else if (currentScene == SCENE_SKY_FALL) {
            float steerSign = (skyFallVelX >= 0.0f) ? 1.0f : -1.0f;
            skyFallVelX = steerSign * DASH_SPEED_MAG;
            spawnFloatingText(">> SKY DASH >>", skyFallPos + glm::vec2(0.0f, 1.0f), glm::vec4(0.2f, 0.9f, 1.0f, 1.0f));
        } else if (currentScene == SCENE_LADDER_CLIMB) {
            ladderPlayerY += 2.2f;
            spawnFloatingText(">> LADDER SURGE >>", glm::vec2(ladderPlayerX, ladderPlayerY + 1.0f), glm::vec4(0.3f, 1.0f, 0.5f, 1.0f));
        }
        showCornerNotification("SKILL", "SHADOW DASH", "Burst speed & trap invulnerability!", glm::vec4(0.25f, 0.85f, 1.0f, 1.0f), 0);
    }

    // -------------------------------------------------------------------------
    // Scene-Specific Controls: 3-Ladder Climb & Sky-Fall Dive (with 15s Cooldown)
    // -------------------------------------------------------------------------
    if (currentScene == SCENE_LADDER_CLIMB) {
        if (!isLadderHit && !isLadderResurrecting) {
            static bool ladderLeftLast = false;
            static bool ladderRightLast = false;

            bool leftPressed  = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
            bool rightPressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);

            if (leftPressed && !ladderLeftLast && !isLadderLeaping) {
                if (ladderCurrentLane > 0) {
                    ladderTargetLane = ladderCurrentLane - 1;
                    isLadderLeaping = true;
                    ladderLeapProgress = 0.0f;
                    audioMgr.play(SFX_JUMP);
                    spawnFloatingText("LEAP LEFT <<<", glm::vec2(ladderPlayerX - 1.0f, ladderPlayerY + 0.8f), glm::vec4(0.3f, 1.0f, 0.8f, 1.0f));
                }
            }
            if (rightPressed && !ladderRightLast && !isLadderLeaping) {
                if (ladderCurrentLane < 2) {
                    ladderTargetLane = ladderCurrentLane + 1;
                    isLadderLeaping = true;
                    ladderLeapProgress = 0.0f;
                    audioMgr.play(SFX_JUMP);
                    spawnFloatingText(">>> LEAP RIGHT", glm::vec2(ladderPlayerX + 1.0f, ladderPlayerY + 0.8f), glm::vec4(0.3f, 1.0f, 0.8f, 1.0f));
                }
            }

            ladderLeftLast  = leftPressed;
            ladderRightLast = rightPressed;
        }
        return;
    }

    if (currentScene == SCENE_SKY_FALL) {
        if (!isSkyFallHit) {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                skyFallVelX -= 40.0f * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                skyFallVelX += 40.0f * deltaTime;
            }
        }
        return;
    }

    // Jump Input: From ground, mid-slope slide, or mid-ladder!
    if ((isGrounded || isSliding || isOnLadder) &&
        (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
    {
        isGameIdleAtStart = false;
        // If near ladder and pressing W, climb up rather than jump away
        bool willClimb = false;
        for (auto& tunnel : gameWorld.tunnels) {
            if (std::abs(playerPos.x - tunnel.x) <= 1.25f &&
                playerPos.y >= (tunnel.minY - 0.4f) && playerPos.y <= (tunnel.maxY + 0.4f))
            {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    currentDirection = DIR_UP;
                    isOnLadder = true;
                    playerPos.x = tunnel.x;
                    playerVel.x = 0.0f;
                    playerVel.y = CLIMB_SPEED;
                    willClimb = true;
                    break;
                }
            }
        }

        if (!willClimb) {
            playerVel.y = JUMP_BURST;
            isGrounded = false;

            if (isSliding) {
                isSliding = false;
                slideInterruptCooldown = 0.45f;
                playerSpriteAngle = 0.0f;
                currentDirection = lastHorizontalDir;
                showCornerNotification("INTERRUPT", "MID-SLOPE JUMP!", "Leaped out of slide trajectory!",
                                       glm::vec4(0.25f, 0.9f, 1.0f, 1.0f), 0);
            }

            if (isOnLadder) {
                isOnLadder = false;
                currentDirection = lastHorizontalDir;
                showCornerNotification("INTERRUPT", "LADDER JUMP!", "Leaped off shaft rungs!",
                                       glm::vec4(0.35f, 0.95f, 0.4f, 1.0f), 1, 2);
            }

            audioMgr.play(SFX_JUMP);
        }
    }

    // Downward Ladder Entry: Press S/Down near a ladder top to descend into underground shafts!
    if (isGrounded && !isOnLadder && !isSliding &&
        (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
    {
        for (auto& tunnel : gameWorld.tunnels) {
            if (std::abs(playerPos.x - tunnel.x) <= 1.25f &&
                playerPos.y >= (tunnel.maxY - 1.0f) && playerPos.y <= (tunnel.maxY + 1.4f))
            {
                currentDirection = DIR_DOWN;
                isOnLadder = true;
                playerPos.x = tunnel.x;
                playerVel.x = 0.0f;
                playerVel.y = -CLIMB_SPEED;
                playerPos.y -= 0.35f;
                isGrounded = false;
                showCornerNotification("TUNNEL", "DESCENDING SHAFT",
                                       "Climbing down into underground!",
                                       glm::vec4(0.4f, 0.85f, 1.0f, 1.0f), 1, 0);
                break;
            }
        }
    }

    // Ladder Upward/Downward Controls
    if (isOnLadder) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            playerVel.y = CLIMB_SPEED;
            currentDirection = DIR_UP;
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            playerVel.y = -CLIMB_SPEED;
            currentDirection = DIR_DOWN;
        } else if (freeExplorationMode) {
            playerVel.y = 0.0f;
        }
    }

    // Midpoint Horizontal Direction Interrupts / Run from Idle
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (isGameIdleAtStart || currentDirection != DIR_LEFT) {
            isGameIdleAtStart = false;
            currentDirection = DIR_LEFT;
            lastHorizontalDir = DIR_LEFT;
            if (isOnLadder) {
                isOnLadder = false;
            }
            if (isSliding) {
                isSliding = false;
                slideInterruptCooldown = 0.45f;
                playerSpriteAngle = 0.0f;
            }
            playerVel.x = -playerSpeed;
            showCornerNotification("MANUAL", "RUN LEFT <<<", "Direction oriented to Left",
                                   glm::vec4(1.0f, 0.55f, 0.2f, 1.0f), 1, 1);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (isGameIdleAtStart || currentDirection != DIR_RIGHT) {
            isGameIdleAtStart = false;
            currentDirection = DIR_RIGHT;
            lastHorizontalDir = DIR_RIGHT;
            if (isOnLadder) {
                isOnLadder = false;
            }
            if (isSliding) {
                isSliding = false;
                slideInterruptCooldown = 0.45f;
                playerSpriteAngle = 0.0f;
            }
            playerVel.x = playerSpeed;
            showCornerNotification("MANUAL", "RUN RIGHT >>>", "Direction oriented to Right",
                                   glm::vec4(0.2f, 0.85f, 1.0f, 1.0f), 1, 0);
        }
    }
}

// -----------------------------------------------------------------------------
// Window Resize Callback
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}
