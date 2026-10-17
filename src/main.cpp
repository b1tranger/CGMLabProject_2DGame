/* =========================================================================
 * The Wenderer: A Multi-Directional Endless Runner
 * Developed for Computer Graphics & Multimedia Lab (CSE0611328)
 *
 * PHASE 1: Core Engine & Base Game (Minimal Sprites)
 *
 * Concrete Milestones Implemented:
 *   - Step 1.1: Reusable 1x1 Quad VAO/VBO & Unified Shader Program (MVP + uColor)
 *   - Step 1.2: Frame-rate Independent Game Loop & Delta Time (dt)
 *   - Step 1.3: Player Movement & Vector-Adaptive Lateral Controls
 *   - Step 1.4: Dynamic Track/Obstacle Spawning & AABB Collision Detection
 *   - Step 1.5: The "Wending" Direction Shift & Smooth Camera Rotation (glm::mix)
 * ========================================================================= */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

// -----------------------------------------------------------------------------
// Constants & Settings
// -----------------------------------------------------------------------------
const unsigned int WINDOW_WIDTH  = 960;
const unsigned int WINDOW_HEIGHT = 720;
const char* WINDOW_TITLE = "The Wenderer: 2D Multi-Directional Runner";

const float TRACK_WIDTH      = 4.0f;
const float SEGMENT_LENGTH   = 35.0f;
const float BASE_SPEED       = 9.0f;
const float LATERAL_SPEED    = 8.0f;
const float WEND_DURATION    = 0.45f; // Duration of camera swing during turns (seconds)

// -----------------------------------------------------------------------------
// Step 1.1: Shader Sources
// -----------------------------------------------------------------------------
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMVP; // Model-View-Projection Matrix

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 uColor; // Dynamically set color per object

void main()
{
    FragColor = vec4(uColor, 1.0f);
}
)";

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------
enum DirectionIndex {
    DIR_RIGHT = 0, // +X (0 deg)
    DIR_UP    = 1, // +Y (90 deg)
    DIR_LEFT  = 2, // -X (180 deg)
    DIR_DOWN  = 3  // -Y (270 deg)
};

const glm::vec2 DIRECTION_VECTORS[4] = {
    glm::vec2( 1.0f,  0.0f), // Right
    glm::vec2( 0.0f,  1.0f), // Up
    glm::vec2(-1.0f,  0.0f), // Left
    glm::vec2( 0.0f, -1.0f)  // Down
};

const float DIRECTION_ANGLES[4] = {
    0.0f,   // Right
    90.0f,  // Up
    180.0f, // Left
    270.0f  // Down
};

struct Obstacle {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
    bool isJumpable;
};

struct TrackSegment {
    glm::vec2 startPos;
    glm::vec2 endPos;
    int directionIdx;
    float length;
    float width;
    std::vector<Obstacle> obstacles;
};

enum GameState {
    STATE_RUNNING,
    STATE_GAME_OVER
};

// -----------------------------------------------------------------------------
// Global Game State
// -----------------------------------------------------------------------------
GameState currentGameState = STATE_RUNNING;

// Step 1.1: Global OpenGL objects
unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
unsigned int shaderProgram = 0;
GLint uMVPLoc   = -1;
GLint uColorLoc = -1;

// Step 1.3: Player State
glm::vec2 playerPos(0.0f, 0.0f);
float playerLateralOffset = 0.0f; // Offset from track center (-1.3 to +1.3)
float playerJumpHeight    = 0.0f; // Visual jump height (Y in local space)
float playerJumpVelocity  = 0.0f;
bool  playerIsJumping     = false;
float playerSpeed         = BASE_SPEED;
float totalDistance       = 0.0f;
int   wendsCompleted      = 0;

// Step 1.5: Wending & Camera Interpolation State
int   currentDirectionIdx  = DIR_RIGHT;
bool  isWending            = false;
float wendTimer            = 0.0f;
float startCameraAngle     = 0.0f;
float targetCameraAngle    = 0.0f;
float currentCameraAngle   = 0.0f;

// Track Segments List
std::vector<TrackSegment> trackSegments;
size_t currentSegmentIndex = 0;

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float deltaTime);
unsigned int compileShaderProgram(const char* vSource, const char* fSource);
void setupReusableQuad();
void drawQuad(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color);
void resetGame();
void spawnNextSegment();
bool checkAABB(const glm::vec2& posA, const glm::vec2& sizeA,
               const glm::vec2& posB, const glm::vec2& sizeB);

// -----------------------------------------------------------------------------
// Main Entry Point
// -----------------------------------------------------------------------------
int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Initialize GLFW
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
    glfwSwapInterval(1); // Enable V-Sync

    // Load OpenGL function pointers via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Step 1.1: Build shader & single reusable quad
    shaderProgram = compileShaderProgram(vertexShaderSource, fragmentShaderSource);
    uMVPLoc   = glGetUniformLocation(shaderProgram, "uMVP");
    uColorLoc = glGetUniformLocation(shaderProgram, "uColor");

    setupReusableQuad();

    // Enable 2D blending for nice overlays
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize track & player state
    resetGame();

    // Step 1.2: Game loop timing
    float lastFrameTime = static_cast<float>(glfwGetTime());
    float titleUpdateTimer = 0.0f;
    int frameCount = 0;

    // -------------------------------------------------------------------------
    // Main Game Loop
    // -------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Calculate Delta Time (dt)
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Cap deltaTime to avoid massive jumps when window is moved/dragged
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        // Frame rate counter
        frameCount++;
        titleUpdateTimer += deltaTime;
        if (titleUpdateTimer >= 0.25f) {
            float fps = frameCount / titleUpdateTimer;
            std::string status = "The Wenderer | Distance: " + std::to_string(static_cast<int>(totalDistance)) +
                                 "m | Wends: " + std::to_string(wendsCompleted) +
                                 " | Speed: " + std::to_string(static_cast<int>(playerSpeed)) +
                                 " | " + (currentGameState == STATE_GAME_OVER ? "GAME OVER (Press SPACE)" : "RUNNING") +
                                 " | FPS: " + std::to_string(static_cast<int>(fps));
            glfwSetWindowTitle(window, status.c_str());
            frameCount = 0;
            titleUpdateTimer = 0.0f;
        }

        // Handle Keyboard Input
        processInput(window, deltaTime);

        // ---------------------------------------------------------------------
        // Step 1.3 & 1.5: Game Logic & Movement Update
        // ---------------------------------------------------------------------
        if (currentGameState == STATE_RUNNING)
        {
            // Player jump physics (arc)
            if (playerIsJumping) {
                playerJumpHeight += playerJumpVelocity * deltaTime;
                playerJumpVelocity -= 28.0f * deltaTime; // Gravity
                if (playerJumpHeight <= 0.0f) {
                    playerJumpHeight = 0.0f;
                    playerJumpVelocity = 0.0f;
                    playerIsJumping = false;
                }
            }

            // Current directional properties
            glm::vec2 forwardVec = DIRECTION_VECTORS[currentDirectionIdx];

            // Move forward along active vector
            float moveStep = playerSpeed * deltaTime;
            playerPos += forwardVec * moveStep;
            totalDistance += moveStep;

            // Slowly increase speed over time for progressive challenge
            playerSpeed = BASE_SPEED + (totalDistance * 0.035f);

            // Check if player reached the end of the current track segment
            const TrackSegment& curSeg = trackSegments[currentSegmentIndex];
            float distFromStart = glm::dot(playerPos - curSeg.startPos, forwardVec);

            if (!isWending && distFromStart >= (curSeg.length - 1.0f))
            {
                // Trigger Step 1.5: The Wending Turn!
                isWending = true;
                wendTimer = 0.0f;
                wendsCompleted++;

                startCameraAngle = currentCameraAngle;

                // Pick next 90-degree turn: Alternate or rotate clockwise/counter-clockwise
                // Turn 90 degrees: (currentDirectionIdx + 1) % 4
                int nextDirIdx = (currentDirectionIdx + 1) % 4;

                // Set camera target angle
                targetCameraAngle = startCameraAngle + 90.0f;

                // Update active direction
                currentDirectionIdx = nextDirIdx;

                // Spawn the next track segment extending from the turn anchor
                spawnNextSegment();
                currentSegmentIndex++;
            }

            // Camera swing interpolation during Wending
            if (isWending)
            {
                wendTimer += deltaTime;
                float t = wendTimer / WEND_DURATION;
                if (t >= 1.0f) {
                    t = 1.0f;
                    isWending = false;
                }
                // Smoothstep interpolation for natural cinematic swing
                float smoothT = t * t * (3.0f - 2.0f * t);
                currentCameraAngle = glm::mix(startCameraAngle, targetCameraAngle, smoothT);
            }

            // Calculate actual player world coordinate:
            // Center of path + lateral offset
            glm::vec2 activeForward = DIRECTION_VECTORS[currentDirectionIdx];
            glm::vec2 activeLateral = glm::vec2(-activeForward.y, activeForward.x);
            glm::vec2 playerWorldCenter = playerPos + activeLateral * playerLateralOffset;

            // -----------------------------------------------------------------
            // Step 1.4: AABB Collision Detection
            // -----------------------------------------------------------------
            glm::vec2 playerHitboxSize(0.9f, 0.9f);

            for (const auto& seg : trackSegments) {
                for (const auto& obs : seg.obstacles) {
                    // If the obstacle is jumpable and player is jumping high enough, ignore collision
                    if (obs.isJumpable && playerJumpHeight > 0.8f) {
                        continue;
                    }

                    if (checkAABB(playerWorldCenter, playerHitboxSize, obs.position, obs.size)) {
                        currentGameState = STATE_GAME_OVER;
                        break;
                    }
                }
                if (currentGameState == STATE_GAME_OVER) break;
            }

            // Cleanup old track segments far behind player
            if (trackSegments.size() > 5) {
                trackSegments.erase(trackSegments.begin());
                if (currentSegmentIndex > 0) currentSegmentIndex--;
            }
        }

        // ---------------------------------------------------------------------
        // Render Frame
        // ---------------------------------------------------------------------
        // Background color: Deep midnight charcoal (or dark red on game over)
        if (currentGameState == STATE_GAME_OVER) {
            glClearColor(0.25f, 0.05f, 0.05f, 1.0f);
        } else {
            glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 1. Projection Matrix: Orthographic 2D View
        float halfW = 8.0f;
        float halfH = 6.0f;
        glm::mat4 projection = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);

        // 2. View Matrix: Rotates and translates to follow the player
        glm::vec2 activeForward = DIRECTION_VECTORS[currentDirectionIdx];
        glm::vec2 activeLateral = glm::vec2(-activeForward.y, activeForward.x);
        glm::vec2 playerWorldCenter = playerPos + activeLateral * playerLateralOffset;

        glm::mat4 view = glm::mat4(1.0f);
        // Step 1.5: Rotate camera plane around Z axis
        view = glm::rotate(view, glm::radians(-currentCameraAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        // Follow player center
        view = glm::translate(view, glm::vec3(-playerWorldCenter.x, -playerWorldCenter.y, 0.0f));

        // ---------------------------------------------------------------------
        // Render Track Segments & Borders
        // ---------------------------------------------------------------------
        for (const auto& seg : trackSegments)
        {
            glm::vec2 segCenter = (seg.startPos + seg.endPos) * 0.5f;
            float angleDeg = DIRECTION_ANGLES[seg.directionIdx];

            // Road Floor
            glm::mat4 modelFloor = glm::mat4(1.0f);
            modelFloor = glm::translate(modelFloor, glm::vec3(segCenter.x, segCenter.y, 0.0f));
            modelFloor = glm::rotate(modelFloor, glm::radians(angleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
            modelFloor = glm::scale(modelFloor, glm::vec3(seg.length, seg.width, 1.0f));

            drawQuad(modelFloor, view, projection, glm::vec3(0.16f, 0.18f, 0.22f)); // Dark road

            // Road Left Rail (Cyan Neon Border)
            glm::mat4 modelRailL = glm::mat4(1.0f);
            modelRailL = glm::translate(modelRailL, glm::vec3(segCenter.x, segCenter.y, 0.0f));
            modelRailL = glm::rotate(modelRailL, glm::radians(angleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
            modelRailL = glm::translate(modelRailL, glm::vec3(0.0f, seg.width * 0.5f, 0.0f));
            modelRailL = glm::scale(modelRailL, glm::vec3(seg.length, 0.12f, 1.0f));
            drawQuad(modelRailL, view, projection, glm::vec3(0.0f, 0.75f, 0.95f));

            // Road Right Rail
            glm::mat4 modelRailR = glm::mat4(1.0f);
            modelRailR = glm::translate(modelRailR, glm::vec3(segCenter.x, segCenter.y, 0.0f));
            modelRailR = glm::rotate(modelRailR, glm::radians(angleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
            modelRailR = glm::translate(modelRailR, glm::vec3(0.0f, -seg.width * 0.5f, 0.0f));
            modelRailR = glm::scale(modelRailR, glm::vec3(seg.length, 0.12f, 1.0f));
            drawQuad(modelRailR, view, projection, glm::vec3(0.0f, 0.75f, 0.95f));

            // Wend Turn Platform (Glowing Amber Corner)
            glm::mat4 modelCorner = glm::mat4(1.0f);
            modelCorner = glm::translate(modelCorner, glm::vec3(seg.endPos.x, seg.endPos.y, 0.0f));
            modelCorner = glm::scale(modelCorner, glm::vec3(seg.width, seg.width, 1.0f));
            drawQuad(modelCorner, view, projection, glm::vec3(1.0f, 0.78f, 0.15f));

            // Render Obstacles on this segment
            for (const auto& obs : seg.obstacles) {
                glm::mat4 modelObs = glm::mat4(1.0f);
                modelObs = glm::translate(modelObs, glm::vec3(obs.position.x, obs.position.y, 0.0f));
                modelObs = glm::scale(modelObs, glm::vec3(obs.size.x, obs.size.y, 1.0f));
                drawQuad(modelObs, view, projection, obs.color);
            }
        }

        // ---------------------------------------------------------------------
        // Render Player
        // ---------------------------------------------------------------------
        // Base shadow / trail when jumping
        if (playerJumpHeight > 0.05f) {
            glm::mat4 modelShadow = glm::mat4(1.0f);
            modelShadow = glm::translate(modelShadow, glm::vec3(playerWorldCenter.x, playerWorldCenter.y, 0.0f));
            modelShadow = glm::scale(modelShadow, glm::vec3(0.8f, 0.8f, 1.0f));
            drawQuad(modelShadow, view, projection, glm::vec3(0.05f, 0.05f, 0.08f));
        }

        // Player Quad (Scaled up with jump height)
        float playerVisualScale = 1.0f + (playerJumpHeight * 0.2f);
        glm::mat4 modelPlayer = glm::mat4(1.0f);
        modelPlayer = glm::translate(modelPlayer, glm::vec3(playerWorldCenter.x, playerWorldCenter.y, 0.0f));
        modelPlayer = glm::rotate(modelPlayer, glm::radians(DIRECTION_ANGLES[currentDirectionIdx]), glm::vec3(0.0f, 0.0f, 1.0f));
        modelPlayer = glm::scale(modelPlayer, glm::vec3(1.0f * playerVisualScale, 1.0f * playerVisualScale, 1.0f));

        // Vibrant Cyan Player Body
        glm::vec3 playerColor = (currentGameState == STATE_GAME_OVER) ? glm::vec3(0.9f, 0.2f, 0.2f) : glm::vec3(0.1f, 0.95f, 0.85f);
        drawQuad(modelPlayer, view, projection, playerColor);

        // Player Front Direction Indicator (Small inner visor)
        glm::mat4 modelVisor = glm::mat4(1.0f);
        modelVisor = glm::translate(modelVisor, glm::vec3(playerWorldCenter.x, playerWorldCenter.y, 0.0f));
        modelVisor = glm::rotate(modelVisor, glm::radians(DIRECTION_ANGLES[currentDirectionIdx]), glm::vec3(0.0f, 0.0f, 1.0f));
        modelVisor = glm::translate(modelVisor, glm::vec3(0.3f, 0.0f, 0.0f));
        modelVisor = glm::scale(modelVisor, glm::vec3(0.3f, 0.6f, 1.0f));
        drawQuad(modelVisor, view, projection, glm::vec3(1.0f, 1.0f, 1.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// Helper: Setup Reusable Quad (Step 1.1)
// -----------------------------------------------------------------------------
void setupReusableQuad()
{
    // Single 1x1 quad centered at (0, 0)
    float quadVertices[] = {
        //  X      Y     Z
        -0.5f,  0.5f, 0.0f, // Top-Left
         0.5f,  0.5f, 0.0f, // Top-Right
         0.5f, -0.5f, 0.0f, // Bottom-Right

        -0.5f,  0.5f, 0.0f, // Top-Left
         0.5f, -0.5f, 0.0f, // Bottom-Right
        -0.5f, -0.5f, 0.0f  // Bottom-Left
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Attribute 0: vec3 Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
// Helper: Draw Quad with MVP Matrix & Color Uniform
// -----------------------------------------------------------------------------
void drawQuad(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color)
{
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(uColorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
// Step 1.4: Axis-Aligned Bounding Box (AABB) Collision Test
// -----------------------------------------------------------------------------
bool checkAABB(const glm::vec2& posA, const glm::vec2& sizeA,
               const glm::vec2& posB, const glm::vec2& sizeB)
{
    float halfAx = sizeA.x * 0.5f;
    float halfAy = sizeA.y * 0.5f;
    float halfBx = sizeB.x * 0.5f;
    float halfBy = sizeB.y * 0.5f;

    bool overlapX = (posA.x + halfAx >= posB.x - halfBx) && (posA.x - halfAx <= posB.x + halfBx);
    bool overlapY = (posA.y + halfAy >= posB.y - halfBy) && (posA.y - halfAy <= posB.y + halfBy);

    return overlapX && overlapY;
}

// -----------------------------------------------------------------------------
// Spawn Next Procedural Track Segment & Obstacles
// -----------------------------------------------------------------------------
void spawnNextSegment()
{
    TrackSegment seg;
    seg.directionIdx = currentDirectionIdx;
    seg.length = SEGMENT_LENGTH;
    seg.width  = TRACK_WIDTH;

    glm::vec2 forwardVec = DIRECTION_VECTORS[seg.directionIdx];
    glm::vec2 lateralVec = glm::vec2(-forwardVec.y, forwardVec.x);

    if (trackSegments.empty()) {
        seg.startPos = glm::vec2(0.0f, 0.0f);
    } else {
        seg.startPos = trackSegments.back().endPos;
    }
    seg.endPos = seg.startPos + forwardVec * seg.length;

    // Generate 3 to 4 obstacles along this new segment
    int numObstacles = 3 + (std::rand() % 2);
    float stepDist = (seg.length - 8.0f) / numObstacles;

    for (int i = 0; i < numObstacles; ++i) {
        float distOnSeg = 5.0f + (i * stepDist) + (static_cast<float>(std::rand() % 100) / 100.0f * 2.0f);
        
        // Lane selection: -1 (Left), 0 (Center), +1 (Right)
        int lane = (std::rand() % 3) - 1;
        float laneOffset = lane * 1.15f;

        Obstacle obs;
        obs.position = seg.startPos + (forwardVec * distOnSeg) + (lateralVec * laneOffset);
        
        // Randomize obstacle type: Ground hazard vs Red block barrier
        bool isJumpableHurdle = (std::rand() % 2 == 0);
        obs.isJumpable = isJumpableHurdle;

        if (isJumpableHurdle) {
            // Low yellow/orange hurdle
            obs.size = glm::vec2(1.2f, 0.45f);
            obs.color = glm::vec3(1.0f, 0.55f, 0.0f);
        } else {
            // Tall crimson block barrier
            obs.size = glm::vec2(1.0f, 1.0f);
            obs.color = glm::vec3(0.95f, 0.22f, 0.25f);
        }

        // Adjust obstacle orientation to match the running direction
        if (seg.directionIdx == DIR_UP || seg.directionIdx == DIR_DOWN) {
            std::swap(obs.size.x, obs.size.y);
        }

        seg.obstacles.push_back(obs);
    }

    trackSegments.push_back(seg);
}

// -----------------------------------------------------------------------------
// Reset Game
// -----------------------------------------------------------------------------
void resetGame()
{
    playerPos = glm::vec2(0.0f, 0.0f);
    playerLateralOffset = 0.0f;
    playerJumpHeight    = 0.0f;
    playerJumpVelocity  = 0.0f;
    playerIsJumping     = false;
    playerSpeed         = BASE_SPEED;
    totalDistance       = 0.0f;
    wendsCompleted      = 0;

    currentDirectionIdx = DIR_RIGHT;
    startCameraAngle    = 0.0f;
    targetCameraAngle   = 0.0f;
    currentCameraAngle  = 0.0f;
    isWending           = false;
    wendTimer           = 0.0f;

    trackSegments.clear();
    currentSegmentIndex = 0;

    // Create initial starting segments
    spawnNextSegment(); // Segment 0
    currentDirectionIdx = DIR_UP;
    spawnNextSegment(); // Segment 1
    currentDirectionIdx = DIR_RIGHT; // Reset direction index to starting direction

    currentGameState = STATE_RUNNING;
}

// -----------------------------------------------------------------------------
// Input Processing (Step 1.3: Vector-Adaptive Controls)
// -----------------------------------------------------------------------------
void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // If game over, press SPACE to reset
    if (currentGameState == STATE_GAME_OVER) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            resetGame();
        }
        return;
    }

    // Step 1.3: Lateral movement along active perpendicular axis
    float maxLateral = (TRACK_WIDTH * 0.5f) - 0.7f; // Keep player inside track rails

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        playerLateralOffset += LATERAL_SPEED * deltaTime;
        if (playerLateralOffset > maxLateral) playerLateralOffset = maxLateral;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        playerLateralOffset -= LATERAL_SPEED * deltaTime;
        if (playerLateralOffset < -maxLateral) playerLateralOffset = -maxLateral;
    }

    // Jump Input (W, Up Arrow, or Space)
    if (!playerIsJumping && (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
                             glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
                             glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS))
    {
        playerIsJumping = true;
        playerJumpVelocity = 11.5f; // Jump burst speed
    }
}

// -----------------------------------------------------------------------------
// Compile & Link Shaders
// -----------------------------------------------------------------------------
unsigned int compileShaderProgram(const char* vSource, const char* fSource)
{
    int success;
    char infoLog[512];

    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// -----------------------------------------------------------------------------
// Window Resize Callback
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}
