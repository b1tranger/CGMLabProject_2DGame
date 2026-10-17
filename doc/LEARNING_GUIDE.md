# The Wenderer: Complete Learning & Implementation Guide 🎮

Welcome to **The Wenderer** project! This guide is written in clear, simple language to help you understand every concept, design decision, and step required to build this 2D multi-directional endless runner from scratch using **C++** and **Modern OpenGL (3.3 Core Profile)**.

---

## Table of Contents
1. [Project Overview & Big Picture](#1-project-overview--big-picture)
2. [Core Concepts in Simple Terms](#2-core-concepts-in-simple-terms)
   - [What is Modern OpenGL?](#what-is-modern-opengl)
   - [The Graphics Pipeline Simplified](#the-graphics-pipeline-simplified)
   - [Why MVP (Model-View-Projection) Matrices?](#why-mvp-model-view-projection-matrices)
   - [What is "Wending"?](#what-is-wending)
3. [Recommended Project File Structure](#3-recommended-project-file-structure)
4. [Third-Party Libraries & Dependencies](#4-third-party-libraries--dependencies)
5. [Step-by-Step Implementation Roadmap](#5-step-by-step-implementation-roadmap)
   - [Phase 1: Core Engine & Geometry (Minimal Sprites)](#phase-1-core-engine--geometry-minimal-sprites)
     - [Step 1.1: Single Reusable Quad & Shader](#step-11-single-reusable-quad--shader)
     - [Step 1.2: Game Loop & Delta Time](#step-12-game-loop--delta-time)
     - [Step 1.3: Player Movement & Dynamic Vectors](#step-13-player-movement--dynamic-vectors)
     - [Step 1.4: Obstacles & AABB Collision](#step-14-obstacles--aabb-collision)
     - [Step 1.5: The "Wending" Direction Shift & Camera Rotation](#step-15-the-wending-direction-shift--camera-rotation)
   - [Phase 2: Sprites, Textures & Sound](#phase-2-sprites-textures--sound)
   - [Phase 3: Game Feel & Polish](#phase-3-game-feel--polish)
   - [Phase 4: Game States & Deployment](#phase-4-game-states--deployment)
6. [Quick Reference & Math Cheatsheet](#6-quick-reference--math-cheatsheet)

---

## 1. Project Overview & Big Picture

### What is the Game?
In most endless runners (like *Subway Surfers* or *Flappy Bird*), the character always moves along one fixed axis (e.g., straight right or forward).

**The Wenderer** subverts this formula:
- You run continuously, dodging obstacles and switching lanes or jumping.
- At certain distance milestones or trigger points, the **running direction violently changes** (for instance, turning 90 degrees from running right `(+X)` to running down `(-Y)`, or running up `(+Y)`).
- The **camera smoothly rotates** to follow the player, aligning the new running vector straight ahead on the screen.
- The player must immediately adapt their controls and mental orientation to survive.

### Why "The Wenderer"?
- A **Wanderer** travels aimlessly across an endless static path.
- A **Wenderer** (from *to wend*, meaning to change direction or proceed along an altered course) is constantly forced to adapt as obstacles block their forward journey. It's a game mechanic that mirrors real-world human adaptability!

---

## 2. Core Concepts in Simple Terms

### What is Modern OpenGL?
Older OpenGL ("Immediate Mode", e.g., `glBegin()` and `glEnd()`) told the graphics card to draw vertices one-by-one. It was easy to write, but extremely slow.

**Modern OpenGL (3.3+ Core Profile)** works differently:
1. You load your vertex data into memory buffers located directly on the GPU (called **VBO**).
2. You tell the GPU how that data is structured using a **VAO**.
3. You run custom mini-programs on the GPU called **Shaders** (written in GLSL) to decide where pixels go and what colors they have.

### The Graphics Pipeline Simplified
```
[Vertex Data on CPU]
        │
        ▼ (Copy to GPU VRAM)
[Vertex Buffer Object (VBO)] ───► [Vertex Array Object (VAO)]
                                            │
                                            ▼
                                   [Vertex Shader] (Calculates positions via MVP matrix)
                                            │
                                            ▼
                                     [Rasterizer] (Turns geometry into pixels/fragments)
                                            │
                                            ▼
                                   [Fragment Shader] (Calculates final pixel color)
                                            │
                                            ▼
                                     [Screen Output]
```

### Why MVP (Model-View-Projection) Matrices?
Instead of creating brand-new vertex arrays every time an object moves or turns, we keep **one single 1x1 quad** in GPU memory and multiply its 4 corners by three matrices:
1. **Model Matrix ($M$):** Moves (translates), scales (sizes), and rotates the individual object in the world.
2. **View Matrix ($V$):** Represents the camera. When the player turns 90 degrees, we rotate the camera's view matrix so the new path faces "up" or "forward" for the player.
3. **Projection Matrix ($P$):** Projects 2D/3D world coordinates onto the 2D computer screen. In our 2D game, we use an **orthographic projection** (`glm::ortho`).

The combined matrix is:
$$\text{MVP} = P \times V \times M$$

### What is "Wending"?
"Wending" is the core game mechanic where the current velocity vector pivots by 90 degrees:
- Running along $+X$ (Right) $\rightarrow$ Pivot to $-Y$ (Down) or $+Y$ (Up).
- Instead of cutting abruptly, the camera view matrix interpolates over a few tenths of a second using `glm::mix` or lerp, producing a dynamic cinematic swing.

---

## 3. Recommended Project File Structure

Here is a clean, modular structure for the C++ project:

```text
CGMLabProject_2DGame/
│
├── assets/                  # Game assets
│   ├── audio/              # Sound effects (jump.wav, turn.wav, crash.wav)
│   ├── fonts/              # Font files (if rendering text)
│   └── textures/           # Spritesheets (player.png, tiles.png, obstacles.png)
│
├── doc/                     # Documentation and research papers
│   ├── LEARNING_GUIDE.md   # This guided documentation
│   ├── prompts/            # Prompts and notes
│   └── research/           # Research PDFs and proposals
│
├── include/                 # Header files (.h / .hpp)
│   ├── Camera.h            # Camera follow, rotation, and screen shake
│   ├── Collision.h         # AABB bounding-box collision math
│   ├── Game.h              # Main Game class: loop, state, update, render
│   ├── Obstacle.h          # Obstacle structs and manager
│   ├── Player.h            # Player state, movement vector, input
│   ├── Renderer.h          # Quad VAO/VBO setup, draw calls
│   ├── Shader.h            # GLSL shader compiler and uniform helpers
│   └── Texture.h           # stb_image wrapper for 2D textures
│
├── src/                     # Source files (.cpp)
│   ├── Camera.cpp
│   ├── Collision.cpp
│   ├── Game.cpp
│   ├── Obstacle.cpp
│   ├── Player.cpp
│   ├── Renderer.cpp
│   ├── Shader.cpp
│   ├── Texture.cpp
│   └── main.cpp            # Entry point: window creation & game loop
│
├── shaders/                 # GLSL Shader programs
│   ├── basic.vert          # Vertex shader for quad transformation (MVP)
│   ├── basic.frag          # Fragment shader for solid colors
│   ├── sprite.vert         # Vertex shader with UV offsets for sprite animation
│   └── sprite.frag         # Fragment shader for textured quads
│
├── vendor/                  # Third-party dependencies
│   ├── glad/               # OpenGL loader (glad.c and glad/glad.h)
│   ├── glfw/               # Windowing library headers & libs
│   ├── glm/                # OpenGL Mathematics header-only library
│   ├── stb_image/          # Single header image loader (stb_image.h)
│   └── miniaudio/          # Single header audio library (miniaudio.h)
│
├── CMakeLists.txt           # Build script (if using CMake)
├── README.md                # Project summary
└── .gitignore               # Git ignore rules for build artifacts
```

---

## 4. Third-Party Libraries & Dependencies

| Library | Role | Why we use it |
| :--- | :--- | :--- |
| **GLFW** | Window & Input | Creates the window and handles keyboard/mouse events. |
| **GLAD** | OpenGL Loader | Locates and loads Modern OpenGL function pointers at runtime. |
| **GLM** | Math Library | Header-only linear algebra library for vectors (`glm::vec2`, `glm::vec3`) and matrices (`glm::mat4`). |
| **stb_image** | Image Loader | Single header file (`stb_image.h`) to load PNG/JPG spritesheets into memory. |
| **miniaudio** | Audio Engine | Single header audio library (`miniaudio.h`) for low-latency SFX playback without heavy installs. |

---

## 5. Step-by-Step Implementation Roadmap

---

### Phase 1: Core Engine & Geometry (Minimal Sprites)

#### Step 1.1: Single Reusable Quad & Shader
Instead of defining separate VAOs and VBOs for every object or triangle, we define **one reusable 1x1 quad centered at (0, 0)**:

```cpp
// 6 vertices forming 2 triangles (counter-clockwise)
float quadVertices[] = {
    //  X      Y     Z
    -0.5f,  0.5f, 0.0f, // Top-Left
     0.5f,  0.5f, 0.0f, // Top-Right
     0.5f, -0.5f, 0.0f, // Bottom-Right

    -0.5f,  0.5f, 0.0f, // Top-Left
     0.5f, -0.5f, 0.0f, // Bottom-Right
    -0.5f, -0.5f, 0.0f  // Bottom-Left
};
```

**Vertex Shader (`basic.vert`):**
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uMVP; // Projection * View * Model

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
```

**Fragment Shader (`basic.frag`):**
```glsl
#version 330 core
out vec4 FragColor;

uniform vec3 uColor; // Dynamically set color per object

void main() {
    FragColor = vec4(uColor, 1.0);
}
```

To draw any object (player, obstacle, floor tile), you simply compute that object's **Model Matrix**, send it via uniform, set its color uniform, and call:
```cpp
glDrawArrays(GL_TRIANGLES, 0, 6);
```

---

#### Step 1.2: Game Loop & Delta Time ($\Delta t$)
Different computers run at different speeds (60 FPS, 144 FPS, etc.). To make movement smooth and identical across all machines, calculate **Delta Time** ($\Delta t$):

```cpp
float lastFrame = 0.0f;

while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process inputs
    processInput(window, deltaTime);

    // Update game logic (movement, collisions)
    update(deltaTime);

    // Render frame
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

---

#### Step 1.3: Player Movement & Dynamic Vectors
The player has:
- A world position: `glm::vec2 position`
- A current forward direction: `glm::vec2 forwardDir` (e.g., `(1.0f, 0.0f)` for right)
- A perpendicular lateral direction: `glm::vec2 lateralDir` (e.g., `(0.0f, 1.0f)` for switching lanes or jumping)

In every frame:
```cpp
// Move forward automatically
player.position += player.forwardDir * (player.speed * deltaTime);

// Move sideways/jump based on input
if (inputLeft)  player.position -= player.lateralDir * (laneSpeed * deltaTime);
if (inputRight) player.position += player.lateralDir * (laneSpeed * deltaTime);
```

---

#### Step 1.4: Obstacles & AABB Collision
**Axis-Aligned Bounding Box (AABB)** checks whether two non-rotated rectangles overlap:

```cpp
struct BoundingBox {
    float minX, maxX;
    float minY, maxY;
};

bool checkCollision(const BoundingBox& a, const BoundingBox& b) {
    bool overlapX = (a.maxX >= b.minX) && (a.minX <= b.maxX);
    bool overlapY = (a.maxY >= b.minY) && (a.minY <= b.maxY);
    return overlapX && overlapY;
}
```

- When an obstacle is generated, place it along the active track path.
- In each update tick, test the player's bounding box against all active obstacles in `std::vector<Obstacle>`.
- If an overlap is detected, trigger the hit event (game over or lose a life).

---

#### Step 1.5: The "Wending" Direction Shift & Camera Rotation
When the player crosses a "Wend Marker" or hits a distance interval:
1. **Rotate the Movement Vector:**
   ```cpp
   // Example 90-degree clockwise turn:
   // (x, y) -> (y, -x)
   glm::vec2 newForward = glm::vec2(player.forwardDir.y, -player.forwardDir.x);
   player.forwardDir = newForward;
   player.lateralDir = glm::vec2(-newForward.y, newForward.x);
   ```

2. **Camera Follow & Rotation:**
   Instead of abruptly snapping, smoothly interpolate the camera's angle:
   ```cpp
   currentCameraAngle = glm::mix(startAngle, targetAngle, transitionProgress);
   
   glm::mat4 view = glm::mat4(1.0f);
   // Rotate around the Z axis
   view = glm::rotate(view, glm::radians(currentCameraAngle), glm::vec3(0.0f, 0.0f, 1.0f));
   // Translate to center on the player
   view = glm::translate(view, glm::vec3(-player.position.x, -player.position.y, 0.0f));
   ```

---

### Phase 2: Sprites, Textures & Sound 🎨
1. **Texture Mapping:** Use `stb_image` to load character and environment PNGs.
2. **UV Coordinates:** Add UV vertex attributes `(u, v)` into the quad buffer.
3. **Sprite Sheet Animation:** Send `uniform vec2 uvOffset` and `uniform vec2 uvScale` to the vertex shader to select the active animation frame without re-uploading buffer data.
4. **Audio Hookup:** Use `miniaudio` to play sounds on key events:
   - `audioEngine.play("turn.wav")` on 90-degree wends
   - `audioEngine.play("hit.wav")` on collision

---

### Phase 3: Game Feel & Polish 🌌
1. **Screen Shake:** When colliding or turning sharply, add a small high-frequency random offset to the camera view matrix for 0.2 seconds.
2. **Color Shifts / Lighting:** As the player progresses deeper, smoothly transition background and track colors (reflecting emotional mood & difficulty).
3. **Particle Trails:** Spawn small colored quads behind the player that shrink and fade over time.

---

### Phase 4: Game States & Deployment 🚀
1. **State Machine:**
   - `STATE_MENU`: Show Title Screen and prompt "Press Space to Wend".
   - `STATE_PLAYING`: Active gameplay loop.
   - `STATE_GAMEOVER`: Display final score, distance covered, and restart prompt.
2. **Score System:** Score = `distanceTraveled + (wendsCompleted * 100)`.
3. **Binary Packaging:** Ensure the C++ executable links statically or bundles necessary DLLs (`glfw3.dll`) and assets folder for distribution.

---

## 6. Quick Reference & Math Cheatsheet

### Orthographic Projection in 2D
```cpp
// Set a virtual screen size of 16 units wide by 9 units high
glm::mat4 projection = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
```

### Quad Model Matrix (Position + Scale + Rotation)
```cpp
glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::scale(model, glm::vec3(width, height, 1.0f));
```

---
*Created for Computer Graphics & Multimedia Lab (CSE0611328) — Autumn 2026*
