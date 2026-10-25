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
#include "UIManager.h"
#include "LadderScene.h"
#include "SkyFallScene.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

// -----------------------------------------------------------------------------
// Global Game State
// -----------------------------------------------------------------------------
GameState currentGameState = STATE_INTRO_VIDEO;
std::string gameOverReason = "";

// Global Character Set (Samurai Mack)
CharacterAnimSet samuraiAnim;
float deathAnimTimer = 0.0f;

// Temporary Shield & Idle-Start State
float shieldTimer        = 0.0f; // Temporary shield duration gained from shield pickups
float postHitInvulnTimer = 0.0f; // Brief post-shield break invulnerability to safely clear the hazard
bool isGameIdleAtStart   = true; // Character stands still at start until player moves
bool freeExplorationMode = true;  // Default Free Exploration mode: full manual A/D movement (toggle auto-run with 'F')

// Tutorial Modal Dialog State
bool showTutorialModal        = false; // Tutorial window active
bool hasShownInitialTutorial  = false; // Ensures tutorial appears only once after launch
bool tutorialOpenedFromPause  = false; // Tracks if opened via [T] inside Pause Menu
bool tutDismissLast           = false; // Prevents accidental key dismiss bleed-through
float exitAppearedBannerTimer = 0.0f;  // Pulsing banner timer when mini-game exit unlocks

// Player Lives & Combat State
int playerLives            = 3; // 3 hits before death, restored by heart pickups
bool isAttacking           = false;
float attackAnimTimer      = 0.0f;
bool attackDamageDealt     = false;

// Block / Defend System (Activated via Right Click / K)
bool isBlocking            = false;
float blockTimer           = 0.0f;     // Up to 1.0s maximum defend duration
float blockCooldown        = 0.0f;     // 2.0s cooldown after blocking an enemy attack

// World & Object Textures
unsigned int worldTilesetTexture     = 0;
unsigned int platformsTexture        = 0;
unsigned int coinTexture             = 0;
unsigned int fruitTexture            = 0; // Shield pickup icon texture
unsigned int heartTexture            = 0; // Heart pickup icon texture
unsigned int spikesTexture           = 0; // Spikes hazard texture
unsigned int birdTexture             = 0;
unsigned int directionArrowsTexture = 0;

// Enemy Textures (Bringer of Death)
unsigned int texBodWalk   = 0;
unsigned int texBodAttack = 0;
unsigned int texBodDeath  = 0;
unsigned int texBodHurt   = 0;
unsigned int texBodIdle   = 0;
unsigned int texBodCast   = 0;
unsigned int texBodSpell  = 0;

// Active Spell Hazards summoned by Bringer of Death
std::vector<SpellHazard> activeSpells;

// Boss Demon Slime Textures
unsigned int texDemonIdle    = 0;
unsigned int texDemonWalk    = 0;
unsigned int texDemonAttack  = 0;
unsigned int texDemonTakeHit = 0;
unsigned int texDemonDeath   = 0;
unsigned int texDemonSmash   = 0;

// Initial Slime Textures
unsigned int texSlimeIdle      = 0;
unsigned int texSlimeMove      = 0;
unsigned int texSlimeTakeHit   = 0;
unsigned int texSlimeTransform = 0;

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

// Celestial Stars for Night Sky (struct defined in SkyFallScene.h)
std::vector<CelestialStar> skyStars;

// Sprite Animation Management
AnimationState currentAnim = ANIM_RUN_RIGHT;
float animTimer = 0.0f;
int currentAnimFrame = 0;

// Side-View Camera
glm::vec2 cameraPos(0.0f, 0.0f);

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
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;
    exitAppearedBannerTimer = 0.0f;
    gameLadderScene.enter(playerPos, lastHorizontalDir);
}

void enterSkyFallScene() {
    currentScene = SCENE_SKY_FALL;
    platformerSavedPos = playerPos;
    platformerSavedDir = lastHorizontalDir;
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;
    exitAppearedBannerTimer = 0.0f;
    gameSkyFallScene.enter(playerPos, lastHorizontalDir);
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
    playerLives = 3;
    isAttacking = false;
    attackAnimTimer = 0.0f;
    attackDamageDealt = false;
    isBlocking = false;
    blockTimer = 0.0f;
    blockCooldown = 0.0f;
    freeExplorationMode = true; // Reset defaults to Free Exploration Mode

    currentDirection  = DIR_RIGHT;
    lastHorizontalDir = DIR_RIGHT;
    directionSwitchTimer = 6.5f;

    currentAnim = ANIM_IDLE;
    animTimer = 0.0f;
    currentAnimFrame = 0;
    deathAnimTimer = 0.0f;

    cameraPos = glm::vec2(0.0f, 0.0f);
    UIManager::instance().clear();
    activeSpells.clear();
    audioMgr.stopTransformationAudio();

    dayNightTimer = 0.0f; // Initial scene will be night time
    lastCycleIndex = 0;
    daylightFactor = 0.0f;
    scenePlayTime = 0.0f;
    sceneDirectionUnlockedNotified = false;
    exitAppearedBannerTimer = 0.0f;

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
    samuraiAnim.texDefend  = gameRenderer.loadTexture("assets/samurai/Defend_nobg.png");
    samuraiAnim.runFrames     = 8;
    samuraiAnim.idleFrames    = 8;
    samuraiAnim.jumpFrames    = 2;
    samuraiAnim.fallFrames    = 2;
    samuraiAnim.attackFrames  = 6;
    samuraiAnim.deathFrames   = 6;
    samuraiAnim.takeHitFrames = 4;
    samuraiAnim.defendFrames  = 8;

    worldTilesetTexture    = gameRenderer.loadTexture("assets/world_tileset.png");
    platformsTexture       = gameRenderer.loadTexture("assets/platforms.png");
    coinTexture            = gameRenderer.loadTexture("assets/coin.png");
    fruitTexture           = gameRenderer.loadTexture("assets/shield_pickup.png");
    heartTexture           = gameRenderer.loadTexture("assets/heart_pickup.png");
    spikesTexture          = gameRenderer.loadTexture("assets/spikes.png");
    birdTexture            = gameRenderer.loadTexture("assets/bird.png");
    directionArrowsTexture = gameRenderer.loadTexture("assets/direction_arrows.png");

    // Load Enemies Textures
    texBodWalk   = gameRenderer.loadTexture("assets/enemies/bod_walk.png");
    texBodAttack = gameRenderer.loadTexture("assets/enemies/bod_attack.png");
    texBodDeath  = gameRenderer.loadTexture("assets/enemies/bod_death.png");
    texBodHurt   = gameRenderer.loadTexture("assets/enemies/bod_hurt.png");
    texBodIdle   = gameRenderer.loadTexture("assets/enemies/bod_idle.png");
    texBodCast   = gameRenderer.loadTexture("assets/enemies/bod_cast.png");
    texBodSpell  = gameRenderer.loadTexture("assets/enemies/bod_spell.png");

    texDemonIdle    = gameRenderer.loadTexture("assets/enemies/demon_idle.png");
    texDemonWalk    = gameRenderer.loadTexture("assets/enemies/demon_walk.png");
    texDemonAttack  = gameRenderer.loadTexture("assets/enemies/demon_attack.png");
    texDemonTakeHit = gameRenderer.loadTexture("assets/enemies/demon_take_hit.png");
    texDemonDeath   = gameRenderer.loadTexture("assets/enemies/demon_death.png");
    texDemonSmash   = gameRenderer.loadTexture("assets/enemies/demon_smash.png");

    texSlimeIdle      = gameRenderer.loadTexture("assets/enemies/slime_idle.png");
    texSlimeMove      = gameRenderer.loadTexture("assets/enemies/slime_move.png");
    texSlimeTakeHit   = gameRenderer.loadTexture("assets/enemies/slime_take_hit.png");
    texSlimeTransform = gameRenderer.loadTexture("assets/enemies/slime_transform.png");

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
        if (!hasShownInitialTutorial) {
            showTutorialModal = true;
            hasShownInitialTutorial = true;
            tutorialOpenedFromPause = false;
        }
        audioMgr.startBGM();
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

        float deltaTime = (currentGameState == STATE_PAUSED || showTutorialModal) ? 0.0f : rawDeltaTime;
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
            if (exitAppearedBannerTimer > 0.0f) {
                exitAppearedBannerTimer -= deltaTime;
                if (exitAppearedBannerTimer < 0.0f) exitAppearedBannerTimer = 0.0f;
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
                playerVel = glm::vec2(0.0f, 0.0f);
                if (!hasShownInitialTutorial) {
                    showTutorialModal = true;
                    hasShownInitialTutorial = true;
                    tutorialOpenedFromPause = false;
                    tutDismissLast = true;
                }
                audioMgr.startBGM();
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

            UIManager::instance().updateCornerNotification(rawDeltaTime);
            UIManager::instance().updateFloatingTexts(rawDeltaTime);

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

            // -----------------------------------------------------------------
            // Branch By Current Game Scene
            // -----------------------------------------------------------------
            if (currentScene == SCENE_LADDER_CLIMB)
            {
                scenePlayTime = gameLadderScene.getTrialTime();
                bool reqExit = false;
                RunDirection exitDir = DIR_RIGHT;
                bool reqSky = false;
                gameLadderScene.update(deltaTime, window, shieldTimer, postHitInvulnTimer, isDashing,
                                       coinsCollected, relicsTriggered, scoreDistance,
                                       reqExit, exitDir, reqSky);
                if (reqExit) {
                    platformerSavedDir = exitDir;
                    lastHorizontalDir = exitDir;
                    currentDirection = exitDir;
                    returnToPlatformerScene(true);
                } else if (reqSky) {
                    enterSkyFallScene();
                }
                animTimer += deltaTime;
            }
            else if (currentScene == SCENE_SKY_FALL)
            {
                scenePlayTime = gameSkyFallScene.getTrialTime();
                bool reqExit = false;
                RunDirection exitDir = DIR_RIGHT;
                gameSkyFallScene.update(deltaTime, window, shieldTimer, postHitInvulnTimer, isDashing,
                                        coinsCollected, relicsTriggered, scoreDistance,
                                        reqExit, exitDir);
                if (reqExit) {
                    platformerSavedDir = exitDir;
                    lastHorizontalDir = exitDir;
                    currentDirection = exitDir;
                    returnToPlatformerScene(true);
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

                                if (!UIManager::instance().isNoticeActive("SLOPE")) {
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
                                playerLives--;
                                postHitInvulnTimer = 1.25f;
                                audioMgr.play(SFX_ENEMY_HIT);
                                if (playerLives <= 0) {
                                    currentGameState = STATE_GAME_OVER;
                                    gameOverReason = "IMPALED BY SPIKY TRAP";
                                    audioMgr.play(SFX_DEATH);
                                } else {
                                    spawnFloatingText("LIFE LOST! (" + std::to_string(playerLives) + "/3)", playerPos + glm::vec2(0.0f, 1.2f),
                                                      glm::vec4(1.0f, 0.25f, 0.25f, 1.0f), 0.28f, 0.40f, 2.0f, 1.3f);
                                    showCornerNotification("DAMAGE", "SPIKE DAMAGE TAKEN!",
                                                           "Remaining Lives: " + std::to_string(playerLives) + "/3",
                                                           glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), 0);
                                }
                                break;
                            }
                        }
                    }
                }

                // 5b. Active Thunder Spells Lifecycle & Damage Check
                // Telegraph cloud gathering: frames 0-6 (0.0s - 0.58s) - player can dodge/parry!
                // Mid-animation strike window: frames 7-10 (0.58s - 0.85s) - lightning bolt hits the ground!
                // Dissipation phase: frames 11-15 (0.85s - 1.35s) - lingering spark fadeout
                for (auto& spell : activeSpells) {
                    if (!spell.active) continue;
                    spell.animTimer += deltaTime;
                    int sFrame = std::min(static_cast<int>(spell.animTimer * 12.0f), 15);

                    // Deal damage strictly at mid-animation (frames 7-10) when the lightning bolt strikes the ground
                    if (sFrame >= 7 && sFrame <= 10 && !spell.hasDealtDamage && !isDashing && postHitInvulnTimer <= 0.0f) {
                        float sDistX = std::abs(playerPos.x - spell.pos.x);
                        float sDistY = std::abs((playerPos.y - 1.2f) - spell.pos.y);
                        if (sDistX < 1.4f && sDistY < 2.2f) {
                            spell.hasDealtDamage = true;
                            if (isBlocking) {
                                blockCooldown = 2.0f; // 2s cooldown after blocking an attack from enemies
                                postHitInvulnTimer = 0.6f;
                                audioMgr.play(SFX_BLOCK);
                                spawnFloatingText("SPELL BLOCKED! [DEFLECTED]", playerPos + glm::vec2(0.0f, 1.3f),
                                                  glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 1.8f, 1.3f);
                                showCornerNotification("DEFENSE", "SPELL BLOCKED!", "Deflected thunder spell strike!",
                                                       glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 1, 0);
                            } else if (shieldTimer > 0.0f) {
                                shieldTimer = 0.0f;
                                postHitInvulnTimer = 0.75f;
                                audioMgr.play(SFX_SLIDE);
                                spawnFloatingText("SHIELD BROKE! [SPELL DEFLECTED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                  glm::vec4(0.3f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 2.2f, 1.4f);
                                showCornerNotification("SHIELD", "SPELL DEFLECTED!", "Shield absorbed the thunder strike!",
                                                       glm::vec4(0.3f, 0.9f, 1.0f, 1.0f), 1, 0);
                            } else {
                                playerLives--;
                                postHitInvulnTimer = 1.25f;
                                audioMgr.play(SFX_ENEMY_HIT);
                                if (playerLives <= 0) {
                                    currentGameState = STATE_GAME_OVER;
                                    gameOverReason = "STRUCK BY DEATH SPELL";
                                    audioMgr.play(SFX_DEATH);
                                } else {
                                    spawnFloatingText("THUNDER STRIKE! (" + std::to_string(playerLives) + "/3)",
                                                      playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(1.0f, 0.25f, 0.25f, 1.0f),
                                                      0.28f, 0.40f, 2.0f, 1.3f);
                                }
                            }
                        }
                    }
                    if (spell.animTimer >= 1.35f) {
                        spell.active = false;
                    }
                }
                activeSpells.erase(std::remove_if(activeSpells.begin(), activeSpells.end(),
                    [](const SpellHazard& s) { return !s.active; }), activeSpells.end());

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

                // 6c. Heart Pickups (Restores 1 Life pip up to 3)
                for (auto& heart : gameWorld.hearts)
                {
                    if (!heart.collected && gameWorld.checkCollision(playerPos, playerHitbox, heart.pos, glm::vec2(1.1f, 1.1f)))
                    {
                        heart.collected = true;
                        audioMgr.play(SFX_HEART_PICKUP);
                        if (playerLives < 3) {
                            playerLives++;
                            showCornerNotification("HEART", "+1 LIFE RESTORED!",
                                                   "Health: " + std::to_string(playerLives) + "/3 Lives",
                                                   glm::vec4(1.0f, 0.35f, 0.55f, 1.0f), 0);
                            spawnFloatingText("+1 LIFE! (" + std::to_string(playerLives) + "/3)", heart.pos + glm::vec2(0.0f, 0.8f),
                                              glm::vec4(1.0f, 0.3f, 0.55f, 1.0f), 0.30f, 0.42f, 2.4f, 1.3f);
                        } else {
                            coinsCollected += 3;
                            showCornerNotification("HEART", "FULL HEALTH BONUS!", "+150 Points (Max Lives)",
                                                   glm::vec4(1.0f, 0.4f, 0.6f, 1.0f), 0);
                            spawnFloatingText("FULL HP! +150", heart.pos + glm::vec2(0.0f, 0.8f),
                                              glm::vec4(1.0f, 0.4f, 0.6f, 1.0f), 0.28f, 0.40f, 2.2f, 1.2f);
                        }
                    }
                    heart.animPhase += deltaTime * 3.5f;
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

                // 7b. Player Attack Resolution
                if (isAttacking) {
                    attackAnimTimer += deltaTime;
                    float dirSign = (lastHorizontalDir == DIR_LEFT) ? -1.0f : 1.0f;
                    // Active damage window during sword slash swing (frames 2-4)
                    if (attackAnimTimer >= 0.10f && attackAnimTimer <= 0.32f && !attackDamageDealt) {
                        glm::vec2 attackCenter = playerPos + glm::vec2(dirSign * 1.35f, 0.0f);
                        glm::vec2 attackReach(2.4f, 2.0f);

                        for (auto& enemy : gameWorld.enemies) {
                            if (!enemy.active || enemy.state == ENEMY_STATE_DEATH || enemy.state == ENEMY_STATE_TRANSFORM) continue;
                            if (gameWorld.checkCollision(attackCenter, attackReach, enemy.pos, enemy.size)) {
                                enemy.hp -= 1;
                                enemy.hurtTimer = 0.25f;
                                attackDamageDealt = true;
                                audioMgr.play(SFX_ENEMY_HIT);

                                if (enemy.hp <= 0) {
                                    enemy.hp = 0;
                                    if (enemy.type == ENEMY_SLIME_INITIAL) {
                                        float footY = enemy.pos.y - 1.2f;
                                        // Slime defeated! Initiates transformation into Boss Demon Slime!
                                        enemy.state = ENEMY_STATE_TRANSFORM;
                                        enemy.stateTimer = 0.0f;
                                        enemy.animTimer = 0.0f;
                                        enemy.pos.y = footY + 3.0f; // Transform quad is 6.0m tall, rests on platform footY
                                        audioMgr.playSlimeDeathSplat(); // Sequence SFX 1: Wet Splat Impact upon slime death
                                        spawnFloatingText("SLIME DEFEATED! TRANSFORMING...", enemy.pos + glm::vec2(0.0f, 1.4f),
                                                          glm::vec4(1.0f, 0.4f, 0.2f, 1.0f), 0.28f, 0.40f, 4.0f, 1.2f);
                                        showCornerNotification("TRANSFORMATION", "BOSS AWAKENING!",
                                                               "The defeated slime begins demon transformation!",
                                                               glm::vec4(1.0f, 0.35f, 0.2f, 1.0f), 1, 0);
                                    } else {
                                        enemy.state = ENEMY_STATE_DEATH;
                                        enemy.animTimer = 0.0f;
                                        audioMgr.play(SFX_ENEMY_DEATH);
                                        if (enemy.type == ENEMY_BRINGER_OF_DEATH) {
                                            coinsCollected += 5;
                                            spawnFloatingText("+250 BRINGER OF DEATH SLAIN!", enemy.pos + glm::vec2(0.0f, 2.2f),
                                                              glm::vec4(0.4f, 1.0f, 0.6f, 1.0f), 0.30f, 0.42f, 2.8f, 1.4f);
                                            showCornerNotification("DEFEATED", "BRINGER OF DEATH SLAIN!", "+250 Points!",
                                                                   glm::vec4(0.4f, 1.0f, 0.6f, 1.0f), 0);
                                        } else if (enemy.type == ENEMY_BOSS_DEMON_SLIME) {
                                            coinsCollected += 20;
                                            audioMgr.playBossDyingRoar(); // dyingRoar.mp3 for 05_d_death.gif
                                            spawnFloatingText("+1000 DEMON SLIME VANQUISHED!", enemy.pos + glm::vec2(0.0f, 3.5f),
                                                              glm::vec4(1.0f, 0.85f, 0.2f, 1.0f), 0.36f, 0.48f, 3.5f, 1.6f);
                                            showCornerNotification("VICTORY", "DEMON SLIME SLAIN!", "+1000 Boss Defeat Bonus!",
                                                                   glm::vec4(1.0f, 0.85f, 0.2f, 1.0f), 0);
                                        }
                                    }
                                } else {
                                    spawnFloatingText("-1 HIT! [" + std::to_string(enemy.hp) + "/" + std::to_string(enemy.maxHp) + " HP]",
                                                      enemy.pos + glm::vec2(0.0f, enemy.size.y * 0.6f + 0.5f),
                                                      glm::vec4(1.0f, 0.85f, 0.25f, 1.0f), 0.26f, 0.38f, 1.6f, 1.2f);
                                }
                                break;
                            }
                        }
                    }
                    if (attackAnimTimer >= 0.50f) {
                        isAttacking = false;
                        attackAnimTimer = 0.0f;
                        attackDamageDealt = false;
                    }
                }

                // 7c. Player Block / Defend Resolution
                if (blockCooldown > 0.0f) {
                    blockCooldown -= deltaTime;
                }
                if (isBlocking) {
                    blockTimer += deltaTime;
                    // While blocking, hero braces in defensive guard stance
                    if (isGrounded && !isDashing && !isSliding) {
                        playerVel.x = 0.0f;
                    }
                    if (blockTimer >= 1.0f) {
                        isBlocking = false;
                        blockTimer = 0.0f;
                        if (blockCooldown < 1.0f) {
                            blockCooldown = 1.0f; // Minimal recovery cooldown if no attack was blocked
                        }
                    }
                }

                // 7d. Enemy Entity AI & Combat Updates
                for (auto& enemy : gameWorld.enemies) {
                    if (!enemy.active) continue;

                    if (enemy.hurtTimer > 0.0f) enemy.hurtTimer -= deltaTime;
                    if (enemy.attackCooldown > 0.0f) enemy.attackCooldown -= deltaTime;
                    if (enemy.jumpCooldown > 0.0f) enemy.jumpCooldown -= deltaTime;
                    if (enemy.spellCooldown > 0.0f) enemy.spellCooldown -= deltaTime;

                    float distX = playerPos.x - enemy.pos.x;
                    float absDist = std::abs(distX);
                    float absY = std::abs(playerPos.y - enemy.pos.y);

                    // =========================================================
                    // Bringer of Death (1-Hit Roaming Enemy with Melee & Spell Casting)
                    // =========================================================
                    if (enemy.type == ENEMY_BRINGER_OF_DEATH) {
                        float footY = enemy.pos.y - 1.45f;
                        if (enemy.state == ENEMY_STATE_DEATH) {
                            enemy.animTimer += deltaTime;
                            if (enemy.animTimer >= 1.0f) { // 10 frames at 10 fps
                                enemy.active = false;
                            }
                        } else {
                            if (enemy.state != ENEMY_STATE_ATTACK && enemy.state != ENEMY_STATE_CAST) {
                                enemy.facingLeft = (distX < 0.0f);
                            }

                            // 1. Trigger close-range telegraphed melee attack when within 2.4m
                            if (absDist < 2.4f && absY < 2.0f && enemy.attackCooldown <= 0.0f &&
                                enemy.state != ENEMY_STATE_ATTACK && enemy.state != ENEMY_STATE_CAST)
                            {
                                enemy.state = ENEMY_STATE_ATTACK;
                                enemy.animTimer = 0.0f;
                                enemy.facingLeft = (distX < 0.0f); // Lock attack direction
                                enemy.hasHitPlayerThisAttack = false;
                                enemy.attackCooldown = 2.0f;
                                audioMgr.play(SFX_ENEMY_ATTACK);
                                spawnFloatingText("[ ! ]", enemy.pos + glm::vec2(0.0f, 1.8f),
                                                  glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), 0.28f, 0.38f, 0.8f, 1.0f);
                            }
                            // 2. Trigger ranged thunder spell cast when player is at a distance (3.0m to 14.0m)
                            // Allows targeting across multi-layer platform elevations (underground cavern, ground surface, elevated platforms, sky canopy)
                            else if (absDist >= 3.0f && absDist <= 14.0f && absY < 14.0f && enemy.spellCooldown <= 0.0f &&
                                     enemy.state != ENEMY_STATE_ATTACK && enemy.state != ENEMY_STATE_CAST)
                            {
                                enemy.state = ENEMY_STATE_CAST;
                                enemy.animTimer = 0.0f;
                                enemy.stateTimer = 0.0f; // 0 = hazard not yet spawned
                                enemy.facingLeft = (distX < 0.0f); // Face target
                                enemy.spellCooldown = 5.0f + static_cast<float>(std::rand() % 25) * 0.1f;
                                audioMgr.playThunderSpell(); // Starts thunder crackle immediately with cast animation!
                                spawnFloatingText("[ ! SPELL CAST ! ]", enemy.pos + glm::vec2(0.0f, 1.8f),
                                                  glm::vec4(0.85f, 0.45f, 1.0f, 1.0f), 0.26f, 0.36f, 1.0f, 1.0f);
                            }

                            if (enemy.state == ENEMY_STATE_ATTACK) {
                                enemy.animTimer += deltaTime;
                                int atkFrame = static_cast<int>(enemy.animTimer * 8.0f); // 8 fps animation (10 frames = 1.25s)

                                // Telegraph wind-up: frames 0-5 (0.0s to 0.75s) - Scythe raised, direction locked
                                // Cleave impact window: frames 6-7 (0.75s to 1.0s) - only hits in front
                                bool playerInFront = (enemy.facingLeft && distX <= 0.3f) || (!enemy.facingLeft && distX >= -0.3f);
                                if (atkFrame >= 6 && atkFrame <= 7 && !enemy.hasHitPlayerThisAttack && playerInFront &&
                                    absDist < 2.6f && absY < 2.0f && !isDashing && postHitInvulnTimer <= 0.0f)
                                {
                                    enemy.hasHitPlayerThisAttack = true;
                                    if (isBlocking) {
                                        blockCooldown = 2.0f; // 2s cooldown after blocking an attack from enemies
                                        postHitInvulnTimer = 0.6f;
                                        audioMgr.play(SFX_BLOCK);
                                        spawnFloatingText("ATTACK BLOCKED! [PARRY]", playerPos + glm::vec2(0.0f, 1.2f),
                                                          glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 1.8f, 1.4f);
                                        showCornerNotification("PARRY", "ATTACK BLOCKED!", "Successfully parried Bringer of Death!",
                                                               glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 1, 0);
                                    } else if (shieldTimer > 0.0f) {
                                        shieldTimer = 0.0f;
                                        postHitInvulnTimer = 0.75f;
                                        audioMgr.play(SFX_SLIDE);
                                        spawnFloatingText("SHIELD BROKE! [ATTACK BLOCKED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                          glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                    } else {
                                        playerLives--;
                                        postHitInvulnTimer = 1.25f;
                                        audioMgr.play(SFX_ENEMY_HIT);
                                        if (playerLives <= 0) {
                                            currentGameState = STATE_GAME_OVER;
                                            gameOverReason = "SLAIN BY BRINGER OF DEATH";
                                            audioMgr.play(SFX_DEATH);
                                        } else {
                                            spawnFloatingText("LIFE LOST! (" + std::to_string(playerLives) + "/3)",
                                                              playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
                                        }
                                    }
                                }
                                if (atkFrame >= 10 || enemy.animTimer >= 1.25f) {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer = 0.0f;
                                }
                            } else if (enemy.state == ENEMY_STATE_CAST) {
                                enemy.animTimer += deltaTime;
                                int castFrame = static_cast<int>(enemy.animTimer * 7.5f); // 9 frames at 7.5 fps = 1.2s

                                // Summon thunder cloud spell at player position on frame 7 (when thunder boom lands!)
                                if (castFrame >= 7 && enemy.stateTimer == 0.0f) {
                                    enemy.stateTimer = 1.0f;
                                    SpellHazard spell;
                                    spell.pos = glm::vec2(playerPos.x, playerPos.y - 1.2f); // Player ground level
                                    spell.animTimer = 0.0f;
                                    spell.hasDealtDamage = false;
                                    spell.audioPlayed = true; // Audio already started at beginning of cast!
                                    spell.active = true;
                                    activeSpells.push_back(spell);
                                    spawnFloatingText("THUNDER CLOUD GATHERING! MOVE!", spell.pos + glm::vec2(0.0f, 3.2f),
                                                      glm::vec4(1.0f, 0.85f, 0.2f, 1.0f), 0.24f, 0.36f, 1.3f, 1.1f);
                                }

                                if (castFrame >= 9 || enemy.animTimer >= 1.20f) {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer = 0.0f;
                                }
                            } else if (absDist < 10.0f && absY < 2.5f) {
                                // Controlled chase speed: 1.3 m/s with platform ledge verification
                                enemy.state = ENEMY_STATE_PATROL;
                                float dir = (distX > 0.0f) ? 1.0f : -1.0f;
                                float step = dir * 1.3f * deltaTime;
                                float nextX = enemy.pos.x + step;

                                if (gameWorld.hasSolidGroundAt(nextX + dir * 0.7f, footY, 0.4f)) {
                                    enemy.pos.x = nextX;
                                    enemy.animTimer += deltaTime;
                                } else {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer += deltaTime;
                                }
                            } else {
                                // Patrol pacing: 1.0 m/s with ledge turnaround
                                enemy.state = ENEMY_STATE_PATROL;
                                float dir = enemy.facingLeft ? -1.0f : 1.0f;
                                float step = dir * 1.0f * deltaTime;
                                float nextX = enemy.pos.x + step;

                                if (gameWorld.hasSolidGroundAt(nextX + dir * 0.7f, footY, 0.4f)) {
                                    enemy.pos.x = nextX;
                                    enemy.animTimer += deltaTime;
                                } else {
                                    enemy.facingLeft = !enemy.facingLeft;
                                    enemy.animTimer += deltaTime;
                                }
                            }
                        }
                    }
                    // =========================================================
                    // Initial Slime (2-Hit Enemy, transforms into Boss)
                    // =========================================================
                    else if (enemy.type == ENEMY_SLIME_INITIAL) {
                        float footY = (enemy.state == ENEMY_STATE_TRANSFORM) ? (enemy.pos.y - 3.0f) : (enemy.pos.y - 1.2f);
                        if (enemy.state == ENEMY_STATE_TRANSFORM) {
                            enemy.stateTimer += deltaTime;
                            enemy.animTimer += deltaTime;

                            // Sequence SFX 2: Acid bubbling right after slime death
                            if (enemy.stateTimer >= 0.85f && enemy.stateTimer - deltaTime < 0.85f) {
                                audioMgr.playAcidBubbling();
                            }

                            // Sequence SFX 3: Monster roar for large monster rising animation
                            if (enemy.stateTimer >= 2.8f && enemy.stateTimer - deltaTime < 2.8f) {
                                audioMgr.playMonsterRoar();
                            }

                            // 5.5s slower transformation duration matching sequenced audio: transform into Boss!
                            if (enemy.stateTimer >= 5.5f) {
                                enemy.type = ENEMY_BOSS_DEMON_SLIME;
                                enemy.maxHp = 5;
                                enemy.hp = 5;
                                enemy.state = ENEMY_STATE_IDLE;
                                enemy.size = glm::vec2(3.6f, 4.4f);
                                enemy.pos.y = footY + 3.0f; // 3.0f rests on ground (footY)
                                enemy.animTimer = 0.0f;
                                enemy.stateTimer = 0.0f;
                                enemy.attackCooldown = 1.2f;
                                enemy.jumpCooldown = 2.0f;
                                enemy.hasHitPlayerThisAttack = false;
                                spawnFloatingText("DEMON SLIME RISES! [5 HP]", enemy.pos + glm::vec2(0.0f, 3.5f),
                                                  glm::vec4(1.0f, 0.25f, 0.35f, 1.0f), 0.35f, 0.48f, 3.5f, 1.5f);
                                showCornerNotification("BOSS BATTLE", "DEMON SLIME SUMMONED!",
                                                       "Attack to defeat (5 hits) or flee/dash away to despawn!",
                                                       glm::vec4(1.0f, 0.25f, 0.35f, 1.0f), 1, 0);
                            }
                        } else {
                            if (absDist < 8.0f && absY < 2.5f) {
                                enemy.facingLeft = (distX < 0.0f);
                                enemy.state = ENEMY_STATE_PATROL;
                                float dir = (distX > 0.0f) ? 1.0f : -1.0f;
                                float step = dir * 0.9f * deltaTime;
                                float nextX = enemy.pos.x + step;

                                if (gameWorld.hasSolidGroundAt(nextX + dir * 0.6f, footY, 0.4f)) {
                                    enemy.pos.x = nextX;
                                    enemy.animTimer += deltaTime;
                                } else {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer += deltaTime;
                                }
                            } else {
                                enemy.state = ENEMY_STATE_PATROL;
                                float dir = enemy.facingLeft ? -1.0f : 1.0f;
                                float step = dir * 0.75f * deltaTime;
                                float nextX = enemy.pos.x + step;

                                if (gameWorld.hasSolidGroundAt(nextX + dir * 0.6f, footY, 0.4f)) {
                                    enemy.pos.x = nextX;
                                    enemy.animTimer += deltaTime;
                                } else {
                                    enemy.facingLeft = !enemy.facingLeft;
                                    enemy.animTimer += deltaTime;
                                }
                            }

                            // Slime contact damage to player on direct physical collision
                            if (absDist < 1.3f && absY < 1.3f && !isDashing && postHitInvulnTimer <= 0.0f) {
                                if (isBlocking) {
                                    blockCooldown = 2.0f; // 2s cooldown after blocking an attack from enemies
                                    postHitInvulnTimer = 0.6f;
                                    audioMgr.play(SFX_BLOCK);
                                    spawnFloatingText("ATTACK BLOCKED! [PARRY]", playerPos + glm::vec2(0.0f, 1.2f),
                                                      glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 1.6f, 1.2f);
                                    showCornerNotification("PARRY", "ATTACK BLOCKED!", "Deflected slime tackle!",
                                                           glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 1, 0);
                                } else if (shieldTimer > 0.0f) {
                                    shieldTimer = 0.0f;
                                    postHitInvulnTimer = 0.75f;
                                    audioMgr.play(SFX_SLIDE);
                                    spawnFloatingText("SHIELD BROKE! [SLIME DEFLECTED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                      glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                } else {
                                    playerLives--;
                                    postHitInvulnTimer = 1.25f;
                                    audioMgr.play(SFX_ENEMY_HIT);
                                    if (playerLives <= 0) {
                                        currentGameState = STATE_GAME_OVER;
                                        gameOverReason = "DISSOLVED BY SLIME";
                                        audioMgr.play(SFX_DEATH);
                                    } else {
                                        spawnFloatingText("LIFE LOST! (" + std::to_string(playerLives) + "/3)",
                                                          playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
                                    }
                                }
                            }
                        }
                    }
                    // =========================================================
                    // Boss Demon Slime (5-Hit Boss)
                    // =========================================================
                    else if (enemy.type == ENEMY_BOSS_DEMON_SLIME) {
                        float footY = enemy.pos.y - 3.0f;
                        // Boss Avoidance / Despawn:
                        // "can be avoided by going to the opposite direction or dashing through it, so it despawns."
                        if (absDist > 28.0f || (isDashing && absDist > 8.0f)) {
                            enemy.active = false;
                            spawnFloatingText("BOSS EVADED & DESPAWNED", playerPos + glm::vec2(0.0f, 1.8f),
                                              glm::vec4(0.4f, 0.9f, 1.0f, 1.0f));
                            showCornerNotification("BOSS EVADED", "DEMON SLIME RETREATED",
                                                   "Successfully escaped boss confrontation!",
                                                   glm::vec4(0.35f, 0.85f, 1.0f, 1.0f), 0);
                        } else if (enemy.state == ENEMY_STATE_DEATH) {
                            enemy.animTimer += deltaTime;
                            if (enemy.animTimer >= 4.0f) { // Synchronized with 3.96s dyingRoar.mp3 duration
                                enemy.active = false;
                            }
                        } else {
                            // Boss commits to attack/jump direction: cannot turn while attacking or jumping!
                            if (enemy.state != ENEMY_STATE_ATTACK && enemy.state != ENEMY_STATE_JUMP) {
                                enemy.facingLeft = (distX < 0.0f);
                            }

                            // Cleave attack trigger when within 4.2m
                            if (absDist < 4.2f && absY < 3.2f && enemy.attackCooldown <= 0.0f &&
                                enemy.state != ENEMY_STATE_ATTACK && enemy.state != ENEMY_STATE_JUMP)
                            {
                                enemy.state = ENEMY_STATE_ATTACK;
                                enemy.animTimer = 0.0f;
                                enemy.facingLeft = (distX < 0.0f); // Lock direction for this attack cycle!
                                enemy.hasHitPlayerThisAttack = false;
                                enemy.attackCooldown = 2.4f;
                                audioMgr.play(SFX_BOSS_ATTACK);
                                spawnFloatingText("[ ! ] DODGE!", enemy.pos + glm::vec2(0.0f, 3.2f),
                                                  glm::vec4(1.0f, 0.3f, 0.2f, 1.0f), 0.35f, 0.48f, 1.1f, 1.2f);
                            }

                            if (enemy.state == ENEMY_STATE_ATTACK) {
                                enemy.animTimer += deltaTime;
                                int atkFrame = static_cast<int>(enemy.animTimer * 8.0f); // 8 fps animation (15 frames = 1.875s)

                                // Telegraph wind-up: frames 0-8 (0.0s to 1.125s) - Demon raises arm and charges dark flame!
                                // Slam impact: frames 9-11 (1.125s to 1.5s) - Shockwave damage strictly in front!
                                bool playerInFront = (enemy.facingLeft && distX <= 0.4f) || (!enemy.facingLeft && distX >= -0.4f);
                                if (atkFrame >= 9 && atkFrame <= 11 && !enemy.hasHitPlayerThisAttack && playerInFront &&
                                    absDist < 4.6f && absY < 3.0f && !isDashing && postHitInvulnTimer <= 0.0f)
                                {
                                    enemy.hasHitPlayerThisAttack = true;
                                    if (isBlocking) {
                                        blockCooldown = 2.0f; // 2s cooldown after blocking an attack from enemies
                                        postHitInvulnTimer = 0.6f;
                                        audioMgr.play(SFX_BLOCK);
                                        spawnFloatingText("BOSS ATTACK BLOCKED! [PARRY]", playerPos + glm::vec2(0.0f, 1.2f),
                                                          glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 1.8f, 1.4f);
                                        showCornerNotification("PARRY", "BOSS ATTACK BLOCKED!", "Parried Demon Slime cleave!",
                                                               glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 1, 0);
                                    } else if (shieldTimer > 0.0f) {
                                        shieldTimer = 0.0f;
                                        postHitInvulnTimer = 0.75f;
                                        audioMgr.play(SFX_SLIDE);
                                        spawnFloatingText("SHIELD BROKE! [DEMON CLEAVE BLOCKED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                          glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                    } else {
                                        playerLives--;
                                        postHitInvulnTimer = 1.25f;
                                        audioMgr.play(SFX_ENEMY_HIT);
                                        if (playerLives <= 0) {
                                            currentGameState = STATE_GAME_OVER;
                                            gameOverReason = "CRUSHED BY DEMON SLIME";
                                            audioMgr.play(SFX_DEATH);
                                        } else {
                                            spawnFloatingText("LIFE LOST! (" + std::to_string(playerLives) + "/3)",
                                                              playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
                                        }
                                    }
                                }
                                if (atkFrame >= 15 || enemy.animTimer >= 1.875f) {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer = 0.0f;
                                }
                            } else if (enemy.state == ENEMY_STATE_JUMP) {
                                // Sub-phase 0 (stateTimer == 0.0f): Crouch wind-up on current ground (frames 0 to 4)
                                if (enemy.stateTimer < 0.5f) {
                                    enemy.vel = glm::vec2(0.0f);
                                    enemy.animTimer += deltaTime;
                                    int crouchFrame = static_cast<int>(enemy.animTimer * 12.0f);
                                    if (crouchFrame >= 5) {
                                        // Launch into airborne leap!
                                        enemy.stateTimer = 1.0f;
                                        float jDir = enemy.facingLeft ? -1.0f : 1.0f;
                                        float targetDx = std::max(2.5f, absDist);
                                        enemy.vel.x = jDir * std::clamp(targetDx / 0.85f, 4.0f, 7.5f);
                                        // High leap if player is above; standard leap otherwise
                                        enemy.vel.y = (playerPos.y > enemy.pos.y + 1.0f) ? 12.5f : 9.5f;
                                        audioMgr.play(SFX_BOSS_ATTACK);
                                        spawnFloatingText("DEMON LEAP!", enemy.pos + glm::vec2(0.0f, 3.2f),
                                                          glm::vec4(1.0f, 0.45f, 0.2f, 1.0f), 0.32f, 0.45f, 1.2f, 1.2f);
                                    }
                                }
                                // Sub-phase 1 (stateTimer == 1.0f): Ballistic airborne flight with gravity & platform landing
                                else if (enemy.stateTimer < 1.5f) {
                                    enemy.vel.y += GRAVITY * deltaTime;
                                    enemy.pos.x += enemy.vel.x * deltaTime;
                                    enemy.pos.y += enemy.vel.y * deltaTime;

                                    // Mid-air poses: 6 = rising, 7 = apex, 8 = falling
                                    int airFrame = 7;
                                    if (enemy.vel.y > 2.5f) airFrame = 6;
                                    else if (enemy.vel.y < -2.5f) airFrame = 8;
                                    enemy.animTimer = static_cast<float>(airFrame) / 12.0f;

                                    // When falling, check landing on platforms or ground
                                    if (enemy.vel.y <= 0.0f) {
                                        float bFeet = enemy.pos.y - 3.0f;
                                        bool landed = false;
                                        float landedTop = 0.0f;

                                        for (const auto& tile : gameWorld.tiles) {
                                            if (tile.isLadder) continue;
                                            float tileHalfW = tile.size.x * 0.5f;
                                            float tileHalfH = tile.size.y * 0.5f;
                                            float tileTop   = tile.pos.y + tileHalfH;

                                            // Horizontal overlap
                                            if (enemy.pos.x + 1.0f > tile.pos.x - tileHalfW &&
                                                enemy.pos.x - 1.0f < tile.pos.x + tileHalfW)
                                            {
                                                if (bFeet <= tileTop + 0.35f && bFeet >= tileTop - 0.85f) {
                                                    landed = true;
                                                    landedTop = tileTop;
                                                    break;
                                                }
                                            }
                                        }

                                        if (!landed) {
                                            float groundTop = SURFACE_GROUND_Y + 0.5f;
                                            if (bFeet <= groundTop + 0.35f && bFeet >= groundTop - 0.85f) {
                                                landed = true;
                                                landedTop = groundTop;
                                            } else if (bFeet <= BEDROCK_Y + 0.5f) {
                                                landed = true;
                                                landedTop = BEDROCK_Y + 0.5f;
                                            }
                                        }

                                        if (landed) {
                                            enemy.pos.y = landedTop + 3.0f;
                                            enemy.vel = glm::vec2(0.0f);
                                            enemy.stateTimer = 2.0f; // Landed! Transition to slam impact
                                            enemy.animTimer = 10.0f / 12.0f; // Frame 10 is touchdown slam
                                            audioMgr.playSmashLandingBang(); // Distant heavy bang on smash touchdown
                                            spawnFloatingText("GROUND SLAM!", enemy.pos + glm::vec2(0.0f, 0.5f),
                                                              glm::vec4(1.0f, 0.3f, 0.2f, 1.0f), 0.35f, 0.50f, 1.2f, 1.4f);
                                        }
                                    }
                                }
                                // Sub-phase 2 (stateTimer == 2.0f): Landed Ground Slam shockwave & recovery
                                else {
                                    enemy.vel = glm::vec2(0.0f);
                                    enemy.animTimer += deltaTime;
                                    int smashFrame = static_cast<int>(enemy.animTimer * 12.0f);

                                    // Shockwave damage window: frames 11 to 14
                                    if (smashFrame >= 11 && smashFrame <= 14 && !enemy.hasHitPlayerThisAttack) {
                                        if (absDist < 4.2f && absY < 3.0f && !isDashing && postHitInvulnTimer <= 0.0f) {
                                            enemy.hasHitPlayerThisAttack = true;
                                            if (isBlocking) {
                                                blockCooldown = 2.0f; // 2s cooldown after blocking an attack from enemies
                                                postHitInvulnTimer = 0.6f;
                                                audioMgr.play(SFX_BLOCK);
                                                spawnFloatingText("SLAM BLOCKED! [DEFLECTED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                                  glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 0.28f, 0.40f, 1.8f, 1.4f);
                                                showCornerNotification("PARRY", "SLAM BLOCKED!", "Braced against boss ground slam!",
                                                                       glm::vec4(0.35f, 1.0f, 0.95f, 1.0f), 1, 0);
                                            } else if (shieldTimer > 0.0f) {
                                                shieldTimer = 0.0f;
                                                postHitInvulnTimer = 0.75f;
                                                audioMgr.play(SFX_SLIDE);
                                                spawnFloatingText("SHIELD BROKE! [SLAM DEFLECTED]", playerPos + glm::vec2(0.0f, 1.2f),
                                                                  glm::vec4(0.3f, 1.0f, 0.95f, 1.0f));
                                            } else {
                                                playerLives--;
                                                postHitInvulnTimer = 1.25f;
                                                audioMgr.play(SFX_ENEMY_HIT);
                                                if (playerLives <= 0) {
                                                    currentGameState = STATE_GAME_OVER;
                                                    gameOverReason = "CRUSHED BY DEMON SLAM";
                                                    audioMgr.play(SFX_DEATH);
                                                } else {
                                                    spawnFloatingText("LIFE LOST! (" + std::to_string(playerLives) + "/3)",
                                                                      playerPos + glm::vec2(0.0f, 1.2f), glm::vec4(1.0f, 0.25f, 0.25f, 1.0f));
                                                }
                                            }
                                        }
                                    }

                                    // Animation complete: return to patrol!
                                    if (smashFrame >= 18 || enemy.animTimer >= 1.5f) {
                                        enemy.state = ENEMY_STATE_PATROL;
                                        enemy.animTimer = 0.0f;
                                        enemy.stateTimer = 0.0f;
                                        enemy.jumpCooldown = 3.5f;
                                        enemy.vel = glm::vec2(0.0f);
                                    }
                                }
                            } else {
                                // Deliberate, menacing advance towards player at 0.9 m/s with platform traversal & jump checks
                                float dir = (distX > 0.0f) ? 1.0f : -1.0f;
                                float step = dir * 0.9f * deltaTime;
                                float nextX = enemy.pos.x + step;

                                bool groundAhead = gameWorld.hasSolidGroundAt(nextX + dir * 1.5f, footY, 0.4f);
                                bool playerAbove = (playerPos.y > enemy.pos.y + 1.2f && absDist < 14.0f);

                                // Jump to cross platform gaps, reach elevated platforms, or leap towards player!
                                if (enemy.jumpCooldown <= 0.0f && (!groundAhead || playerAbove || (absDist > 5.5f && absDist < 14.0f))) {
                                    enemy.state = ENEMY_STATE_JUMP;
                                    enemy.animTimer = 0.0f;
                                    enemy.stateTimer = 0.0f;
                                    enemy.facingLeft = (distX < 0.0f);
                                    enemy.vel = glm::vec2(0.0f);
                                    enemy.hasHitPlayerThisAttack = false;
                                    enemy.jumpCooldown = 3.5f;
                                } else if (groundAhead) {
                                    enemy.state = ENEMY_STATE_PATROL;
                                    enemy.pos.x = nextX;
                                    enemy.animTimer += deltaTime;
                                } else {
                                    enemy.state = ENEMY_STATE_IDLE;
                                    enemy.animTimer += deltaTime;
                                }
                            }

                            // Grounding enforcement for Boss Demon Slime in all non-jump states
                            if (enemy.state != ENEMY_STATE_JUMP) {
                                float bFeet = enemy.pos.y - 3.0f;
                                float supportY = SURFACE_GROUND_Y + 0.5f;
                                for (const auto& tile : gameWorld.tiles) {
                                    if (tile.isLadder) continue;
                                    float tileHalfW = tile.size.x * 0.5f;
                                    float tileHalfH = tile.size.y * 0.5f;
                                    if (enemy.pos.x + 1.0f > tile.pos.x - tileHalfW && enemy.pos.x - 1.0f < tile.pos.x + tileHalfW) {
                                        float top = tile.pos.y + tileHalfH;
                                        if (top <= bFeet + 0.35f && top > supportY) {
                                            supportY = top;
                                        }
                                    }
                                }
                                if (bFeet > supportY + 0.05f) {
                                    enemy.vel.y += GRAVITY * deltaTime;
                                    enemy.pos.y += enemy.vel.y * deltaTime;
                                    if (enemy.pos.y - 3.0f <= supportY) {
                                        enemy.pos.y = supportY + 3.0f;
                                        enemy.vel.y = 0.0f;
                                    }
                                } else {
                                    enemy.pos.y = supportY + 3.0f;
                                    enemy.vel.y = 0.0f;
                                }
                            }
                        }
                    }
                }

                // Prune dead/despawned enemies
                for (auto it = gameWorld.enemies.begin(); it != gameWorld.enemies.end(); ) {
                    if (!it->active) {
                        it = gameWorld.enemies.erase(it);
                    } else {
                        ++it;
                    }
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
            cameraPos.x = glm::mix(cameraPos.x, gameLadderScene.getPlayerX(), rawDeltaTime * 6.0f);
            cameraPos.y = glm::mix(cameraPos.y, gameLadderScene.getPlayerY() + 1.2f, rawDeltaTime * 5.0f);
        } else if (currentScene == SCENE_SKY_FALL) {
            cameraPos.x = glm::mix(cameraPos.x, gameSkyFallScene.getPlayerPos().x, rawDeltaTime * 6.0f);
            cameraPos.y = glm::mix(cameraPos.y, gameSkyFallScene.getPlayerPos().y - 1.2f, rawDeltaTime * 5.0f);
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
            gameLadderScene.render(view, projection, cameraPos, animTimer, daylightFactor, twilightFactor,
                                   worldTilesetTexture, coinTexture, fruitTexture, directionArrowsTexture,
                                   texScenery[activeScenery], spikesTexture, shieldTimer, playerRenderSize);
        }
        else if (currentScene == SCENE_SKY_FALL)
        {
            gameSkyFallScene.render(view, projection, cameraPos, animTimer, daylightFactor, twilightFactor,
                                    birdTexture, worldTilesetTexture, coinTexture, fruitTexture,
                                    directionArrowsTexture, texScenery[activeScenery], spikesTexture, shieldTimer, playerRenderSize);
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

            // 5. Render Spiky Traps (Procedural Metallic Spike Hazard Texture)
            glm::vec4 spikeUV(0.0f, 0.0f, 1.0f, 1.0f);
            for (const auto& spike : gameWorld.spikes)
            {
                glm::mat4 modelSpike = glm::mat4(1.0f);
                modelSpike = glm::translate(modelSpike, glm::vec3(spike.pos.x, spike.pos.y, 0.0f));
                modelSpike = glm::scale(modelSpike, glm::vec3(spike.size.x, spike.size.y, 1.0f));
                gameRenderer.drawQuad(modelSpike, view, projection, glm::vec4(1.0f), (spikesTexture != 0), spikeUV, spikesTexture);
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

            // 6c. Render Heart Pickups (Restores 1 Life Pip)
            for (const auto& heart : gameWorld.hearts)
            {
                if (heart.collected) continue;
                float hoverY = heart.pos.y + std::sin(heart.animPhase) * 0.18f;
                float pulseScale = 1.0f + std::cos(heart.animPhase * 2.2f) * 0.10f;

                glm::mat4 modelAura = glm::mat4(1.0f);
                modelAura = glm::translate(modelAura, glm::vec3(heart.pos.x, hoverY, 0.0f));
                modelAura = glm::scale(modelAura, glm::vec3(1.4f * pulseScale, 1.4f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelAura, view, projection, glm::vec4(1.0f, 0.25f, 0.45f, 0.32f), false, glm::vec4(0), 0);

                int hFrame = static_cast<int>(heart.animPhase * 2.0f) % 4;
                glm::vec4 heartUV = getHeartUV(hFrame);
                glm::mat4 modelHeart = glm::mat4(1.0f);
                modelHeart = glm::translate(modelHeart, glm::vec3(heart.pos.x, hoverY, 0.0f));
                modelHeart = glm::scale(modelHeart, glm::vec3(1.15f * pulseScale, 1.15f * pulseScale, 1.0f));
                gameRenderer.drawQuad(modelHeart, view, projection, glm::vec4(1.0f), (heartTexture != 0), heartUV, heartTexture);
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
            } else if (isBlocking) {
                activeTexture = activeSet.texDefend;
                totalFrames = activeSet.defendFrames;
                int bFrame = static_cast<int>(blockTimer * 8.0f);
                frameIndex = std::min(bFrame, totalFrames - 1);
            } else if (isAttacking) {
                activeTexture = activeSet.texAttack;
                totalFrames = activeSet.attackFrames;
                int aFrame = static_cast<int>(attackAnimTimer * 12.0f);
                frameIndex = std::min(aFrame, totalFrames - 1);
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

            // 8c. Render Active Enemies (Bringer of Death, Slime, Boss Demon Slime)
            for (const auto& enemy : gameWorld.enemies)
            {
                if (!enemy.active) continue;

                unsigned int eTex = 0;
                int eTotalFrames = 1;
                int eFrame = 0;
                glm::vec2 eRenderSize(2.4f, 2.4f);

                if (enemy.type == ENEMY_BRINGER_OF_DEATH) {
                    eRenderSize = glm::vec2(4.5f, 3.0f);
                    if (enemy.state == ENEMY_STATE_DEATH) {
                        eTex = texBodDeath;
                        eTotalFrames = 10;
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 10.0f), eTotalFrames - 1);
                    } else if (enemy.state == ENEMY_STATE_ATTACK) {
                        eTex = texBodAttack;
                        eTotalFrames = 10;
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 8.0f), eTotalFrames - 1);
                    } else if (enemy.state == ENEMY_STATE_CAST) {
                        eTex = texBodCast;
                        eTotalFrames = 9;
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 9.0f), eTotalFrames - 1);
                    } else if (enemy.hurtTimer > 0.0f) {
                        eTex = texBodHurt;
                        eTotalFrames = 3;
                        eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                    } else if (enemy.state == ENEMY_STATE_PATROL) {
                        eTex = texBodWalk;
                        eTotalFrames = 8;
                        eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                    } else {
                        eTex = texBodIdle;
                        eTotalFrames = 8;
                        eFrame = static_cast<int>(enemy.animTimer * 5.0f) % eTotalFrames;
                    }
                } else if (enemy.type == ENEMY_SLIME_INITIAL) {
                    if (enemy.state == ENEMY_STATE_TRANSFORM) {
                        eRenderSize = glm::vec2(6.0f, 6.0f);
                        eTex = texSlimeTransform;
                        eTotalFrames = 33;
                        // Slower animation speed (6 fps, 33 frames = 5.5s) matching the 3-part audio sequence
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 6.0f), eTotalFrames - 1);
                    } else {
                        eRenderSize = glm::vec2(2.4f, 2.4f);
                        if (enemy.hurtTimer > 0.0f) {
                            eTex = texSlimeTakeHit;
                            eTotalFrames = 6;
                            eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                        } else if (enemy.state == ENEMY_STATE_PATROL) {
                            eTex = texSlimeMove;
                            eTotalFrames = 8;
                            eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                        } else {
                            eTex = texSlimeIdle;
                            eTotalFrames = 6;
                            eFrame = static_cast<int>(enemy.animTimer * 5.0f) % eTotalFrames;
                        }
                    }
                } else if (enemy.type == ENEMY_BOSS_DEMON_SLIME) {
                    eRenderSize = glm::vec2(10.8f, 6.0f);
                    if (enemy.state == ENEMY_STATE_DEATH) {
                        eTex = texDemonDeath;
                        eTotalFrames = 22;
                        // Slower, dramatic death animation (6.0 FPS, 22 frames over ~3.67s) synchronized with dyingRoar.mp3 (3.96s)
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 6.0f), eTotalFrames - 1);
                    } else if (enemy.state == ENEMY_STATE_JUMP) {
                        eTex = texDemonSmash;
                        eTotalFrames = 18;
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 12.0f), eTotalFrames - 1);
                    } else if (enemy.state == ENEMY_STATE_ATTACK) {
                        eTex = texDemonAttack;
                        eTotalFrames = 15;
                        eFrame = std::min(static_cast<int>(enemy.animTimer * 8.0f), eTotalFrames - 1);
                    } else if (enemy.hurtTimer > 0.0f) {
                        eTex = texDemonTakeHit;
                        eTotalFrames = 5;
                        eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                    } else if (enemy.state == ENEMY_STATE_PATROL) {
                        eTex = texDemonWalk;
                        eTotalFrames = 12;
                        eFrame = static_cast<int>(enemy.animTimer * 8.0f) % eTotalFrames;
                    } else {
                        eTex = texDemonIdle;
                        eTotalFrames = 6;
                        eFrame = static_cast<int>(enemy.animTimer * 5.0f) % eTotalFrames;
                    }
                }

                if (eTex != 0 && eTotalFrames > 0) {
                    float eu = static_cast<float>(eFrame) / static_cast<float>(eTotalFrames);
                    float edu = 1.0f / static_cast<float>(eTotalFrames);
                    // Raw enemy sprite sheets (Bringer of Death, Slime, Demon Boss) natively face LEFT in their textures.
                    // Therefore, do not flip when facing left; flip horizontally (-edu) when facing right!
                    bool flipHorizontal = !enemy.facingLeft;
                    glm::vec4 eUV = flipHorizontal ? glm::vec4(eu + edu, 0.0f, -edu, 1.0f) : glm::vec4(eu, 0.0f, edu, 1.0f);

                    glm::vec4 eTint = (enemy.hurtTimer > 0.0f) ?
                                      glm::vec4(1.0f, 0.35f, 0.35f, 1.0f) : glm::vec4(1.0f);

                    // Calibrated pixel centering for Bringer of Death (body at x=103 in 140px canvas = +1.077m)
                    float bodOffsetX = 0.0f;
                    if (enemy.type == ENEMY_BRINGER_OF_DEATH) {
                        bodOffsetX = enemy.facingLeft ? -1.077f : 1.077f;
                    }

                    glm::mat4 modelEnemy = glm::mat4(1.0f);
                    modelEnemy = glm::translate(modelEnemy, glm::vec3(enemy.pos.x + bodOffsetX, enemy.pos.y, 0.0f));
                    modelEnemy = glm::scale(modelEnemy, glm::vec3(eRenderSize.x, eRenderSize.y, 1.0f));
                    gameRenderer.drawQuad(modelEnemy, view, projection, eTint, true, eUV, eTex);
                }
            }

            // Render Active Bringer of Death Thunder Cloud Spells
            for (const auto& spell : activeSpells) {
                if (!spell.active) continue;
                int sFrame = std::min(static_cast<int>(spell.animTimer * 12.0f), 15);
                float su = static_cast<float>(sFrame) / 16.0f;
                float sdu = 1.0f / 16.0f;
                glm::vec4 sUV(su, 0.0f, sdu, 1.0f);

                glm::mat4 modelSpell = glm::mat4(1.0f);
                // 3.5m wide by 4.5m tall; rests on spell.pos.y
                modelSpell = glm::translate(modelSpell, glm::vec3(spell.pos.x, spell.pos.y + 2.25f, 0.0f));
                modelSpell = glm::scale(modelSpell, glm::vec3(3.5f, 4.5f, 1.0f));
                gameRenderer.drawQuad(modelSpell, view, projection, glm::vec4(1.0f, 1.0f, 1.0f, 0.95f), (texBodSpell != 0), sUV, texBodSpell);
            }

            // 8d. Render Overhead Enemy Health Bars
            UIManager::instance().renderEnemyHealthBars(view, projection, gameWorld.enemies, animTimer);
        }

        // 8b. Render World-Space Floating Texts (Pickups, Direction Warps, Score Popups)
        UIManager::instance().renderFloatingTexts(view, projection);

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
        UIManager::instance().renderCornerNotification(halfW, halfH, frameCount, coinTexture, fruitTexture);

        // 11. Top-Right On-Screen Score Counter HUD
        int totalScore = static_cast<int>(scoreDistance) * 10 + coinsCollected * 50 + relicsTriggered * 100;
        UIManager::instance().renderScoreHUD(halfW, halfH, totalScore, coinsCollected, scoreDistance, shieldTimer, playerLives, frameCount, coinTexture, heartTexture, isBlocking, blockCooldown);

        // On-Screen Start Hint Banner (when standing still in idle animation at game start)
        if (currentScene == SCENE_PLATFORMER && isGameIdleAtStart && currentGameState == STATE_RUNNING) {
            UIManager::instance().renderStartHintBanner(halfW, halfH, animTimer);
        }

        // Top-Center Mini-Game Scene Countdown & Exit Banner HUD
        if ((currentScene == SCENE_LADDER_CLIMB || currentScene == SCENE_SKY_FALL) && currentGameState == STATE_RUNNING) {
            float trialTimeLeft = std::max(0.0f, SCENE_DIRECTION_COOLDOWN - scenePlayTime);
            UIManager::instance().renderTrialBadge(halfW, halfH, trialTimeLeft, SCENE_DIRECTION_COOLDOWN, static_cast<float>(glfwGetTime()));
        }

        // 12. Pause Menu Modal Dialog (ESC Key)
        if (currentGameState == STATE_PAUSED) {
            UIManager::instance().renderPauseMenu(halfW, halfH, totalScore, coinsCollected, relicsTriggered,
                                                  scoreDistance, freeExplorationMode, static_cast<int>(activeScenery),
                                                  isNight, DAY_NIGHT_CYCLE_DURATION - phaseTime, autoShiftEnabled,
                                                  audioMgr.isMuted());
        }

        // 13. Game Over Screen
        if (currentGameState == STATE_GAME_OVER) {
            UIManager::instance().renderGameOver(halfW, halfH, gameOverReason, totalScore, coinsCollected, scoreDistance);
        }

        // 14. Tutorial Window Modal Dialog (Shown once after Intro & via [T] in Pause Menu)
        if (showTutorialModal) {
            UIManager::instance().renderTutorialModal(halfW, halfH, tutorialOpenedFromPause);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup Resources
    introVideoPlayer.close();
    audioMgr.stopBGM();
    gameRenderer.cleanup();
    if (samuraiAnim.texRun) glDeleteTextures(1, &samuraiAnim.texRun);
    if (samuraiAnim.texIdle) glDeleteTextures(1, &samuraiAnim.texIdle);
    if (samuraiAnim.texJump) glDeleteTextures(1, &samuraiAnim.texJump);
    if (samuraiAnim.texFall) glDeleteTextures(1, &samuraiAnim.texFall);
    if (samuraiAnim.texAttack) glDeleteTextures(1, &samuraiAnim.texAttack);
    if (samuraiAnim.texDeath) glDeleteTextures(1, &samuraiAnim.texDeath);
    if (samuraiAnim.texTakeHit) glDeleteTextures(1, &samuraiAnim.texTakeHit);
    if (samuraiAnim.texDefend) glDeleteTextures(1, &samuraiAnim.texDefend);

    if (worldTilesetTexture) glDeleteTextures(1, &worldTilesetTexture);
    if (platformsTexture) glDeleteTextures(1, &platformsTexture);
    if (coinTexture) glDeleteTextures(1, &coinTexture);
    if (fruitTexture) glDeleteTextures(1, &fruitTexture);
    if (heartTexture) glDeleteTextures(1, &heartTexture);
    if (spikesTexture) glDeleteTextures(1, &spikesTexture);
    if (birdTexture) glDeleteTextures(1, &birdTexture);
    if (directionArrowsTexture) glDeleteTextures(1, &directionArrowsTexture);
    for (int s = 0; s < SCENERY_COUNT; ++s) {
        if (texScenery[s]) glDeleteTextures(1, &texScenery[s]);
    }
    if (texBodWalk) glDeleteTextures(1, &texBodWalk);
    if (texBodAttack) glDeleteTextures(1, &texBodAttack);
    if (texBodDeath) glDeleteTextures(1, &texBodDeath);
    if (texBodHurt) glDeleteTextures(1, &texBodHurt);
    if (texBodIdle) glDeleteTextures(1, &texBodIdle);
    if (texBodCast) glDeleteTextures(1, &texBodCast);
    if (texBodSpell) glDeleteTextures(1, &texBodSpell);
    if (texDemonIdle) glDeleteTextures(1, &texDemonIdle);
    if (texDemonWalk) glDeleteTextures(1, &texDemonWalk);
    if (texDemonAttack) glDeleteTextures(1, &texDemonAttack);
    if (texDemonTakeHit) glDeleteTextures(1, &texDemonTakeHit);
    if (texDemonDeath) glDeleteTextures(1, &texDemonDeath);
    if (texDemonSmash) glDeleteTextures(1, &texDemonSmash);
    if (texSlimeIdle) glDeleteTextures(1, &texSlimeIdle);
    if (texSlimeMove) glDeleteTextures(1, &texSlimeMove);
    if (texSlimeTakeHit) glDeleteTextures(1, &texSlimeTakeHit);
    if (texSlimeTransform) glDeleteTextures(1, &texSlimeTransform);

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

    // Handle Tutorial Window Dismissal
    if (showTutorialModal) {
        bool dismissPressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
                               glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
                               glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
                               glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);
        if (dismissPressed && !tutDismissLast) {
            showTutorialModal = false;
            tutDismissLast = true;
            if (!tutorialOpenedFromPause) {
                currentGameState = STATE_RUNNING;
                isGameIdleAtStart = true;
                playerVel = glm::vec2(0.0f, 0.0f);
            }
            return; // Modal captures input exclusively and prevents dismiss key bleed-through!
        }
        tutDismissLast = dismissPressed;
        return; // Modal captures input exclusively
    }
    tutDismissLast = false;

    // Intro Video Input: Press Space, Enter, or Esc to skip intro
    if (currentGameState == STATE_INTRO_VIDEO) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            introVideoPlayer.skip();
            introVideoPlayer.close();
            currentGameState = STATE_RUNNING;
            isGameIdleAtStart = true;
            playerVel = glm::vec2(0.0f, 0.0f);
            if (!hasShownInitialTutorial) {
                showTutorialModal = true;
                hasShownInitialTutorial = true;
                tutorialOpenedFromPause = false;
                tutDismissLast = true; // Key held to skip video cannot dismiss tutorial
            }
            audioMgr.startBGM();
            return;
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

    // Tutorial Window Toggle Key (T key - dedicated, opens tutorial from either Pause Menu or Running state)
    static bool tKeyLast = false;
    bool tKeyPressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);
    if (tKeyPressed && !tKeyLast) {
        showTutorialModal = true;
        tutorialOpenedFromPause = (currentGameState == STATE_PAUSED);
        tutDismissLast = true; // Key held: prevents the modal from instantly dismissing on next frame!
    }
    tKeyLast = tKeyPressed;

    // Shift Mode Toggle Key (Y key - toggles autonomous timer-based direction shifts)
    static bool yKeyLast = false;
    bool yKeyPressed = (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS);
    if (yKeyPressed && !yKeyLast) {
        autoShiftEnabled = !autoShiftEnabled;
        showCornerNotification("SHIFT MODE", autoShiftEnabled ? "AUTO SHIFTS ON" : "AUTO SHIFTS OFF",
                               autoShiftEnabled ? "Timer shifts enabled (every 6.5s)" : "Direction shifts via Direction Relics only!",
                               autoShiftEnabled ? glm::vec4(1.0f, 0.65f, 0.2f, 1.0f) : glm::vec4(0.35f, 0.95f, 0.45f, 1.0f), 1, 0);
    }
    yKeyLast = yKeyPressed;

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

    // Toggle Music Mute (M key) - Toggles background music mute on/off
    static bool mKeyLast = false;
    bool mKeyPressed = (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS);
    if (mKeyPressed && !mKeyLast) {
        audioMgr.toggleBgmMute();
        bool muted = audioMgr.isMuted();
        showCornerNotification("MUSIC", muted ? "BGM: MUTED" : "BGM: 50% VOLUME",
                               muted ? "Background music muted [Press M to Unmute]" : "Background music playing at 50% [Quarter in the Slot]",
                               muted ? glm::vec4(1.0f, 0.4f, 0.4f, 1.0f) : glm::vec4(0.4f, 0.9f, 1.0f, 1.0f), 0);
        spawnFloatingText(muted ? "MUSIC: MUTED" : "MUSIC: UNMUTED (50%)",
                          (currentScene == SCENE_PLATFORMER ? playerPos + glm::vec2(0.0f, 1.5f) : glm::vec2(0.0f, 1.5f)),
                          muted ? glm::vec4(1.0f, 0.45f, 0.45f, 1.0f) : glm::vec4(0.45f, 0.9f, 1.0f, 1.0f));
    }
    mKeyLast = mKeyPressed;

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

    static bool gameOverRestartLast = false;
    if (currentGameState == STATE_GAME_OVER) {
        bool restartPressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
                               glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
                               glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
        if (restartPressed && !gameOverRestartLast) {
            resetGame();
            gameOverRestartLast = true;
            return; // Immediately return so Space doesn't jump or start movement!
        }
        gameOverRestartLast = restartPressed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        return;
    }
    gameOverRestartLast = false;

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
            float steerSign = (gameSkyFallScene.getVelX() >= 0.0f) ? 1.0f : -1.0f;
            gameSkyFallScene.applyDash(steerSign * DASH_SPEED_MAG);
            spawnFloatingText(">> SKY DASH >>", gameSkyFallScene.getPlayerPos() + glm::vec2(0.0f, 1.0f), glm::vec4(0.2f, 0.9f, 1.0f, 1.0f));
        } else if (currentScene == SCENE_LADDER_CLIMB) {
            gameLadderScene.applyDash(2.2f);
            spawnFloatingText(">> LADDER SURGE >>", glm::vec2(gameLadderScene.getPlayerX(), gameLadderScene.getPlayerY() + 1.0f), glm::vec4(0.3f, 1.0f, 0.5f, 1.0f));
        }
        showCornerNotification("SKILL", "SHADOW DASH", "Burst speed & trap invulnerability!", glm::vec4(0.25f, 0.85f, 1.0f, 1.0f), 0);
    }

    // ATTACK Input (J key, X key, or Left Mouse Button)
    static bool attackKeyLast = false;
    bool attackKeyPressed = (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS ||
                             glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS ||
                             glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    if (attackKeyPressed && !attackKeyLast && !isAttacking && currentGameState == STATE_RUNNING) {
        isAttacking = true;
        attackAnimTimer = 0.0f;
        attackDamageDealt = false;
        audioMgr.play(SFX_SLASH);
    }
    attackKeyLast = attackKeyPressed;

    // DEFEND / BLOCK Input (Right Mouse Button or K key)
    static bool defendKeyLast = false;
    bool defendKeyPressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
                             glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS);
    if (defendKeyPressed && !defendKeyLast && !isBlocking && !isAttacking && blockCooldown <= 0.0f && currentGameState == STATE_RUNNING) {
        isBlocking = true;
        blockTimer = 0.0f;
        spawnFloatingText("[ DEFENDING ]", playerPos + glm::vec2(0.0f, 1.4f),
                          glm::vec4(0.4f, 0.85f, 1.0f, 1.0f), 0.22f, 0.32f, 0.8f, 0.8f);
    }
    defendKeyLast = defendKeyPressed;

    // Scene-Specific Controls handled directly by LadderScene and SkyFallScene updates
    if (currentScene == SCENE_LADDER_CLIMB || currentScene == SCENE_SKY_FALL) {
        return;
    }

    // Jump Input: From ground, mid-slope slide, or mid-ladder!
    if ((isGrounded || isSliding || isOnLadder) &&
        (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
    {
        if (isGameIdleAtStart) {
            // Jump straight up in place while staying idle until player chooses horizontal movement!
            playerVel.y = JUMP_BURST;
            playerVel.x = 0.0f;
            isGrounded = false;
            audioMgr.play(SFX_JUMP);
        } else {
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
