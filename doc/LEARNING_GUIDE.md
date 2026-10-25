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
   - [Phase 5: Mini-Game Scene State Machine, Dash Trails & Dynamic Direction Mini-Games](#phase-5-mini-game-scene-state-machine-dash-trails--dynamic-direction-mini-games)
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

### Phase 2: Side-View Platformer Engine & Animated Sprites 🎨

#### Step 2.1: Transition to Side-View Perspective & Platform Physics
Instead of looking down onto a twisting ribbon path from above, the game adopts a classic **2D Side-View Platformer** perspective:
- **Horizontal Axis ($X$):** Represents running distance (both rightward $+X$ and leftward $-X$).
- **Vertical Axis ($Y$):** Represents elevation, jump heights, ladders, and falling under gravity.
- **Constant Gravity:** Applied every frame via `playerVel.y += GRAVITY * deltaTime` ($g \approx -32\,\text{m/s}^2$).
- **AABB Platform Landing:** When the player falls onto a platform surface ($Y_{\text{bottom}} \approx Y_{\text{top}}$ and horizontal overlap exists), vertical velocity is zeroed and `isGrounded = true`.

#### Step 2.2: OpenGL Texture Mapping & UV Atlas Slicing
In Modern OpenGL 3.3 Core Profile, rendering textures requires:
1. **Texture Coordinates (UV):** Added as attribute `1` (`vec2 aTexCoord`) on each vertex of the quad, spanning $(0, 0)$ to $(1, 1)$.
2. **Texture Loading via `stb_image`:** Single-header library loading PNG pixel buffers straight into GPU VRAM with `glTexImage2D()`.
3. **UV Sub-Rectangle Uniform (`uUVRect`):**
   Given a sprite sheet of width $W$, height $H$, and cell size $S \times S$:
   $$u = \frac{\text{col} \times S}{W}, \quad v = \frac{\text{row} \times S}{H}, \quad \Delta u = \frac{S}{W}, \quad \Delta v = \frac{S}{H}$$
   The vertex shader dynamically computes:
   $$\text{TexCoord} = (u + aTexCoord.x \cdot \Delta u, \; v + aTexCoord.y \cdot \Delta v)$$
   This allows sampling any animation frame from an entire character sheet in a single draw call with zero CPU vertex buffer re-allocations!

#### Step 2.3: Animated Character Sprite States
Using the Liberated Pixel Cup (LPC) composite character sprite sheet:
- **Running Right:** Row 37 (8-frame horizontal run cycle, 12 FPS).
- **Running Left:** Row 35 (8-frame leftward run cycle, 12 FPS).
- **Climbing:** Row 30 (6-frame upward ladder climb cycle, 10 FPS).
- **Falling:** Row 26, Frame 3 (in-air downward fall posture with spread limbs).
- **Jumping:** Row 26, Frame 1 (initial rising jump frame).

#### Step 2.4: Autonomous Dynamic Direction Shifts
To preserve the core theme of adaptability without requiring manual turn points:
- An internal countdown timer randomly triggers shifts between `MIN_SWITCH_TIME` (3.0s) and `MAX_SWITCH_TIME` (6.0s).
- When triggered, the runner dynamically switches:
  - $\text{Right} \longleftrightarrow \text{Left}$ or pivots to vertical ladder climbing.
  - The character animation immediately changes to match the new direction.
  - The camera smoothly interpolates its look-ahead anchor toward the new direction of travel.
#### Step 2.5: Brackeys World Assets, Vertical Traversal & Spiky Traps 🏔️

Instead of flat, plain-color geometric rectangles, the world is constructed from the acclaimed **Brackeys 2D Platformer Bundle**:

1. **Brackeys Tile Atlas (`assets/world_tileset.png`):**
   - A $256 \times 256$ pixel atlas composed of $16 \times 16$ pixel tiles ($16 \times 16$ grid).
   - **Grass & Dirt Blocks:** Left cap $(0, 0)$, Middle grass $(1, 0)$, Right cap $(2, 0)$, and Underneath dirt $(1, 1)$.
   - **Climbable Ladders:** Ladder top cap $(9, 3)$, Ladder body rungs $(9, 4)$.
   - **Lethal Spikes:** Spikes hazard $(4, 8)$.

2. **Mystery Direction Relics (`assets/fruit.png`):**
   - Picking up a floating fruit relic triggers an immediate randomized direction shift (`DIR_RIGHT`, `DIR_LEFT`, `DIR_UP`, `DIR_DOWN`).
   - The HUD flashes a dynamic direction alert banner (e.g. `[DIRECTION SHIFT: UP (CLIMB)]`).

3. **Vertical Traversal: Ladders & Pitfalls:**
   - **Ladders:** Connecting the Bedrock Ground ($Y = -3.5$) with Middle ($Y = -0.4$) and Upper ($Y = +2.6$) tiers. Entering the ladder volume removes gravitational fall and allows free vertical climb using `W` / Up and `S` / Down.
   - **Pitfalls & Drop-Throughs:** Platforms feature deliberate pitfall gaps. Furthermore, pressing `S` / Down enables dropping down through one-way elevated platform surfaces to lower tiers.

4. **Abyss Immunity (No Out-of-Bounds Deaths):**
   - Unlike standard platformers where falling into an empty pit triggers instant death, **The Wenderer** guarantees the player never falls out of bounds.
   - An endless solid Bedrock floor spans the lower boundary of the level.

5. **Spiky Traps & Game Over:**
   - Instead of abyss deaths, danger comes from **Spiky Traps** placed on platform edges and pitfall floors.
   - Touching or falling into spikes detects an AABB overlap and triggers immediate impalement and Game Over.

#### Step 2.6: Endless Procedural Generation, Ladder Tunnels, Steep Slopes & Audio Synthesis 🎶

1. **In-Memory Audio Synthesis (`winmm`):**
   - Zero external audio files or dependencies required.
   - Generates 16-bit mono PCM samples at 22,050 Hz packed into in-memory RIFF/WAVE byte buffers.
   - Played asynchronously with Windows standard `PlaySoundA((LPCSTR)buffer.data(), NULL, SND_MEMORY | SND_ASYNC)`.
   - Distinct procedural waveforms:
     - **Footsteps (`SFX_STEP`):** Low resonant frequency decays ($130 \rightarrow 45\,\text{Hz}$).
     - **Jump (`SFX_JUMP`):** Upward pitch glide ($190 \rightarrow 620\,\text{Hz}$).
     - **Climb (`SFX_CLIMB`):** Dual harmonic wooden rung clacks ($460\,\text{Hz} + 880\,\text{Hz}$).
     - **Slide (`SFX_SLIDE`):** Rushing friction noise + wind whoosh.
     - **Coin (`SFX_COIN`):** Two-tone bright chime ($987\,\text{Hz} \rightarrow 1318\,\text{Hz}$).
     - **Relic (`SFX_RELIC`):** 4-note ascending chord arpeggio ($C_5, E_5, G_5, C_6$).
     - **Shift (`SFX_SHIFT`):** Resonant frequency whoosh & transition sweep.
     - **Death (`SFX_DEATH`):** Low distorted crunch & impact.

2. **Vertical Ladder Tunnel Shafts:**
   - To make vertical travel intuitive and visible, ladders are encased in structured **masonry shafts**.
   - Left and right stone columns enclose a dark interior tunnel shaft backdrop with an illuminated emerald guide line.
   - Entering the shaft smoothly centers the player's horizontal coordinate onto the ladder centerline, disabling gravity and enabling upward/downward traversal using `W`/Up or `S`/Down.

3. **Steep Slopes for Downward Traversal:**
   - Steep angled ramps connecting higher elevations to lower platform floors.
   - Entering a slope switches the player into the `isSliding` state:
     - Player is locked to the angled surface contour $Y_{\text{surf}}(x) = Y_1 + \frac{x - x_1}{x_2 - x_1} \cdot (Y_2 - Y_1)$.
     - High-speed slide acceleration ($11.5\,\text{m/s}$).
     - Character sprite rotates along the incline angle $\theta$.
     - Loops the rushing wind slide audio effect and launches the player with residual momentum upon exiting the slope!

4. **Dynamic Biomes & Transition Phase:**
   - 4 environment biomes: Emerald Forest, Frozen Glacier, Sunset Canyon, Mystic Cavern.
   - When direction shifts, a cinematic transition phase activates:
     - Subtle time dilation ($0.45\times$ speed for $0.25\,\text{s}$).
     - Full-screen energy flash.
     - Smooth interpolation (`glm::mix`) of sky colors, backdrop pillar colors, and platform tints.

5. **Comprehensible Corner HUD Notifications:**
   - Top-left glassmorphism card rendering rotating item sprites (animated coin or fruit relic), category tags, titles, and explanations.

#### Step 2.7: Multi-Elevation Worlds, Direction Memory & Midpoint Interrupts 🧗

1. **Directional Trajectory Memory:**
   - The engine maintains `lastHorizontalDir` (`DIR_RIGHT` or `DIR_LEFT`).
   - Entering an upward ladder shaft does not reset or force a rightward default upon summiting.
   - When reaching the top or base of a vertical shaft, the character smoothly resumes whichever horizontal course they were travelling before the climb.

2. **Midpoint Traversal Interrupts:**
   - **Downhill Slope Slide Interrupt:** Pressing `Space`, `W`, or Up while sliding instantly releases the slope attachment constraint, launching the character into a mid-slope jump (`playerVel.y = JUMP_BURST`).
   - **Mid-Ladder Dismount:** Pressing `A` (Left) or `D` (Right) or `Space` (Jump) while on a ladder shaft immediately breaks vertical locking, letting the player leap or dismount onto adjacent ledges.
   - **Pure Vertical Ladder Traversal:** Sidewise velocity is strictly zero ($v_x = 0$) while climbing, preventing unwanted lateral drift.

3. **Multi-Elevation Vertical Strata:**
   - **Sky Canopy Tier ($Y \approx +5.5$):** Reached by extended upward ladder shafts; features floating platforms, crystals, and grand steep return slopes returning to the surface.
   - **Surface Platforms ($Y \approx +1.0$ to $+2.8$):** Standard platformer terrain with jumps and hazards.
   - **Surface Ground ($Y = -0.5$):** Continuous baseline with chasm openings.
   - **Underground Cavern Zone ($Y = -4.5$):** Deep subterranean layer entered via downward dive slopes; features cavern floors, stalactite hazards, and multiple upward ladder shafts connecting to surface openings.
   - **Bedrock Foundation ($Y = -7.5$):** Continuous bottom safety floor guaranteeing zero out-of-bounds abyss deaths.

#### Step 2.8: Modular Engine Architecture & Traversal Chasm Carving 🏛️

1. **Modular Code Decoupling:**
   To maintain code readability and separation of concerns, the monolithic `main.cpp` was decomposed into distinct modules:
   - **[`/include/Common.h`](/include/Common.h):** Global data models, tuned physics constants, and UV computation math.
   - **[`/include/Audio.h`](/include/Audio.h) & [`/src/Audio.cpp`](/src/Audio.cpp):** Procedural WinMM WAV synthesizer and asynchronous sound effects dispatcher.
   - **[`/include/Renderer.h`](/include/Renderer.h) & [`/src/Renderer.cpp`](/src/Renderer.cpp):** Modern OpenGL 3.3 Core shaders, quad VAO/VBO memory buffers, texture caching, and quad drawing pipeline.
   - **[`/include/World.h`](/include/World.h) & [`/src/World.cpp`](/src/World.cpp):** Procedural level streaming, multi-elevation stratum generation, and AABB collision resolution.
   - **[`/src/main.cpp`](/src/main.cpp):** Game loop, player physics state machine, input handling, and HUD notification overlay.

2. **Proactive Ladder Stopping & Latching:**
   - When entering or passing a vertical ladder tunnel while `currentDirection == DIR_UP` (or pressing `W`/Up within range), the player immediately latches onto the shaft.
   - Horizontal velocity is instantly zeroed ($v_x = 0$), preventing running past the ladder and ensuring fluid, deterministic vertical ascent.

3. **Unblocked Cavern Chasm Carving:**
   - Downward slopes leading to the underground cavern require an unobstructed path.
   - The chunk generator pre-calculates the slope's horizontal span and carves an open chasm, preventing surface ground or elevated platforms from generating across the slope.

4. **Pacing & Speed Calibration:**
   - Run speed is tuned to $4.8\,\text{m/s}$, climb speed to $3.8\,\text{m/s}$, and slide speed to $7.8\,\text{m/s}$, producing smooth, readable, and responsive platformer controls.

---

### Phase 3: Game Feel & Polish 🌌
1. **Screen Shake:** When colliding or turning sharply, add a small high-frequency random offset to the camera view matrix for 0.2 seconds.
2. **Color Shifts / Lighting:** As the player progresses deeper, smoothly transition background and track colors (reflecting emotional mood & difficulty).
3. **Particle Trails:** Spawn small colored quads behind the player that shrink and fade over time.

---

### Phase 4: Game States, Pause Menu & Font Rendering 🚀
1. **Interactive State Machine:**
   - `STATE_RUNNING`: Active gameplay loop (physics simulation, procedural platform streaming, audio playback).
   - `STATE_PAUSED`: Triggered by pressing `ESC`. Simulation deltaTime freezes ($dt = 0$), and a glassmorphism modal dialog is rendered displaying live run statistics and control actions (`[ESC/SPACE]` Resume, `[C]` Toggle Hero, `[R]` Restart Run, `[Q]` Quit).
   - `STATE_GAME_OVER`: Triggered upon spike hazard collision. Shows death defeat animation, reason, score breakdown, and retry prompt.
2. **In-Memory 8x8 Bitmap Font Pipeline:**
   - Implemented zero-dependency text rendering via public-domain ASCII bitmap glyphs (`font8x8_basic.h`).
   - The CPU unrolls 1-bit monochrome glyphs into a 128x64 RGBA OpenGL texture during `Renderer::init()`.
   - `Renderer::drawText()` computes sub-rect UVs on the fly, rendering arbitrary strings with scaling and color tinting anywhere on screen.
3. **Multi-Hero Character System:**
   - Hand-crafted pixel-art sprite sets for **Samurai Mack** and **Shinobi Kenji**.
   - Supports 8 distinct movement animations: Run Right, Flipped Run Left, Jump Rise, Downward Fall, Slope Slide Dash, Ladder Climbing, Breathing Idle, and Ground Defeat.
   - Dynamic hot-swapping via `C` key or Pause Menu with corner HUD toast feedback.
4. **On-Screen Real-Time Score Counter HUD:**
   - Top-right corner HUD card displaying total calculated score, coin count, and traveled distance in meters.
   - Formula: $\text{Total Score} = \text{Distance (m)} \times 10 + \text{Coins} \times 50 + \text{Relics} \times 100$.
5. **World-Space Floating Text Engine & Pickup Indicators:**
   - Extended `Renderer::drawText` with an optional `const glm::mat4& view` parameter. When supplied, text is transformed by camera view and projection matrices into 2D world space rather than static screen UI space.
   - Mystery Fruit Relic pickups spawn vibrant floating text above the fruit: `RELIC COLLECTED!\nWARP >>> RIGHT` (or `<<< LEFT`, `^^^ ASCEND`, `vvv DESCEND`), floating upward and fading over 2.6 seconds.
   - Gold Coin pickups spawn floating golden `+50` score indicators.
   - Disabled the background autonomous direction switch timer by default (`autoShiftEnabled = false`), ensuring direction shifts only trigger on visible Mystery Fruit pickups, while providing a `[T]` key toggle and Pause Menu option to re-enable them.

---

### Phase 5: Mini-Game Scene State Machine, Dash Trails & Dynamic Direction Mini-Games ⚡

1. **Scene State Machine Architecture (`GameScene`)**:
   - The game engine supports three major scenes:
     - `SCENE_PLATFORMER`: Standard multi-elevation infinite runner with slopes, ladders, spikes, coins, and relics.
     - `SCENE_LADDER_CLIMB`: Triggered by UP direction changes. Dedicated vertical climbing minigame across 3 parallel tracks.
     - `SCENE_SKY_FALL`: Triggered by DOWN direction changes. High-altitude free-fall diving minigame.
   - Scene transitions reset velocity vectors, camera offsets, and spawn obstacle buffers while maintaining running score and hero choice.

2. **Dash Mechanic & Ghost Afterimage Trail Rendering (`SHIFT` Key)**:
   - **Physics & Cooldown**: Pressing `SHIFT` initiates an immediate speed burst ($v_x = \text{sign} \times 15.0\,\text{m/s}$) for $0.22\,\text{s}$ with a $0.65\,\text{s}$ cooldown.
   - **Ghost Trail Queue**: While dashing, the engine appends player positions every $0.04\,\text{s}$ to `std::vector<DashGhost> dashGhosts`.
   - **Render Pass**: Before drawing the player, each ghost afterimage is rendered with decaying alpha ($\alpha = \text{remaining} / \text{duration}$) and tinted with cyan/gold ethereal tones using additive blending (`glBlendFunc(GL_SRC_ALPHA, GL_ONE)`).
   - **Hazard Phasing**: Spikes and hazard collisions are bypassed while `dashTimer > 0`, allowing skilled players to phase straight through obstacles.

3. **3-Parallel Endless Ladder Scene (`SCENE_LADDER_CLIMB`)**:
   - **Ladder Track Geometry**: Three vertical ladder tracks are placed at $x = -1.5, 0.0, 1.5$.
   - **Horizontal Leaping**: Pressing `A`/`D` transitions the player between lane indices $0, 1, 2$ via smooth interpolation ($x(t) = \text{lerp}(x_{\text{start}}, x_{\text{target}}, t / 0.20)$).
   - **Boundary Scene Exit**: Leaping past the outer ladders ($A$ on lane 0 or $D$ on lane 2) smoothly returns the player to the regular platform runner with a horizontal jump arc.
   - **Trap Hazards & Reverse Resurrection**:
     - Moving blade/spike traps ascend from above or below in random lanes.
     - Upon trap hit, gravity takes over and the player falls downward while the screen fades to black (`ladderDarkAlpha \to 1.0`).
     - At full darkness, the reverse death animation plays: frames $5 \to 0$ of `Death.png` are cycled sequentially under a spotlight, visually resurrecting the warrior before returning to run on the main platform.

4. **Sky-Fall Aerial Diving Scene (`SCENE_SKY_FALL`)**:
   - **Free-Fall Physics**: Gravity accelerates the character downward ($v_y \approx -9.5\,\text{m/s}$) against an expansive sky backdrop.
   - **Aerial Steering**: `A` and `D` apply horizontal air resistance drift, while `SHIFT` enables high-speed lateral dodges.
   - **Animated Bird Obstacles (`assets/bird.png`)**:
     - Procedurally generated 4-frame animated raptor sprite sheet ($256 \times 64$, four $64 \times 64$ frames).
     - Birds swoop across the player's descent vector with sinusoidal flight paths: $y_{\text{bird}}(t) = y_0 + A \sin(\omega t)$.
   - **$90^\circ$ Counter-Clockwise Take Hit Rotation**:
     - Colliding with a bird or floating sky spike rotates the `Take Hit.png` sprite $90^\circ$ anti-clockwise to reflect falling aerodynamic impact:
       $$\mathbf{M} = \mathbf{T}(p_x, p_y) \cdot \mathbf{R}_z(+90^\circ) \cdot \mathbf{S}(w, h)$$
     - After impact, the player fades through a dark transition and safely lands back on the regular platform runner.

5. **Thematic Background Scenery Assets & Parallax Tiling**:
   - Four distinct backdrop themes rendered as seamless $512 \times 256$ looping parallax layers:
     - **Silvergrass Field**: Silvery plumes of Japanese Susuki grass swaying across gentle moonlit and sunlit hills.
     - **Autumn Foliage**: Blazing maple and ginkgo crowns with rich crimson, amber, and golden-orange leaf canopies.
     - **Green Forest Foliage**: Dense evergreen firs and lush woodland oaks with deep viridian layers.
     - **Urban Area**: High-rise metropolitan towers, spires, and lit windows that glow vibrantly at night and blend into glass at daytime.
   - **Seamless Parallax Formula**: To ensure infinite wrapping without seam tears, the tile X coordinate is computed via:
     $$x_{\text{start}} = \lfloor (c_x - c_x \cdot f_p) / W_{\text{layer}} \rfloor \cdot W_{\text{layer}} + c_x \cdot f_p$$
     where $f_p = 0.32$ is the camera parallax tracking factor and $W_{\text{layer}} = 26.0$ units.

6. **Day / Night Lighting Cycle Simulation (1.5-Minute Half-Cycle)**:
   - **Initial State**: Begins in **Night time** ($t = 0.0\,\text{s}$).
   - **Phase Determination**:
     $$\text{cycleIndex} = \lfloor t_{\text{elapsed}} / 90.0 \rfloor, \quad \text{isNight} = (\text{cycleIndex} \bmod 2 == 0)$$
   - **Twilight Transition Curve**: In the final 15 seconds of each 90-second half-cycle ($t_{\text{phase}} \in [75, 90]$), a smooth interpolation factor $s \in [0, 1]$ blends lighting:
     $$L = \begin{cases} s & \text{if Night} \to \text{Day (Dawn)} \\ 1.0 - s & \text{if Day} \to \text{Night (Dusk)} \end{cases}$$
   - **Celestial Bodies & Lighting**:
     - Stars ($N = 64$) and Moon with lunar halo render when $L < 0.75$.
     - Radiant Golden Sun with solar corona renders when $L > 0.25$.
     - Dynamic background tint blends between cool moonlight indigo $[0.42, 0.48, 0.70]$, direct sunlight $[1.02, 1.0, 0.98]$, and twilight rose-gold $[1.08, 0.72, 0.58]$.

7. **15-Second Mini-Game Scene Trial Cooldown**:
   - Entering `SCENE_LADDER_CLIMB` or `SCENE_SKY_FALL` resets `scenePlayTime = 0.0f`.
   - Direction shifts (autonomous timer, Mystery Relics, and key shortcuts `S` or outer-lane leaps) are strictly locked for the first 15 seconds ($t < 15.0\,\text{s}$), giving players dedicated time to experience the minigame mechanics.
   - Reaching $15.0\,\text{s}$ triggers an on-screen banner `"=== DIRECTION SHIFTS UNLOCKED! ==="`, unlocking all directional exits and transitions.

8. **Left Chunk Persistence, Ground Snapping & Fruit Energy Shield (`v0.5.6`)**:
   - **Continuous Leftward Procedural Generation**:
     - *Issue Identified*: When running far left after culling distant right chunks, the generation boundary `leftGenEdge` remained frozen at its historical minimum, failing the condition $p_x - 35.0 < \text{leftGenEdge}$ and creating an infinite void.
     - *Solution*: `World::updateProcedural` now dynamically computes the bounding box of all active tiles $[\min(x_i), \max(x_i)]$, clamping `leftGenEdge = minTileX` and `rightGenEdge = maxTileX`. Running leftward immediately triggers new chunk instantiation.
   - **Zero Mid-Air Floating on Scene Return**:
     - *Ground Raycast Snapping*: When returning from `SCENE_LADDER_CLIMB` or `SCENE_SKY_FALL`, the engine calls `World::ensureGroundAround(p_x)` to guarantee platform existence across $[p_x - 45, p_x + 45]$. It then conducts a vertical scan over all active tiles directly underneath the player's horizontal coordinate $p_x$:
       $$y_{\text{ground}} = \max \{ t_y + 0.5 \cdot t_h \mid |t_x - p_x| \le 0.5 \cdot t_w + 0.4 \}$$
     - If solid ground is detected within $4.0\,\text{m}$ below the player, the player's feet are snapped flush ($p_y = y_{\text{ground}} + 0.5 \cdot h$) and `isGrounded = true`. If returning over an intentional gap/chasm, `isGrounded = false` is preserved and normal downward gravity accelerates the character, eliminating mid-air freezing.
   - **Direction Arrowheads vs. Shield Fruits**:
     - *Direction Arrowhead Relics*: Custom 4-frame sprite sheet (`assets/direction_arrows.png`, $256 \times 64$) provides unambiguous visual cues (Left, Right, Up, Down). `Common.h` provides `getDirectionArrowUV(RunDirection dir)` mapping normalized UV coordinates $[\frac{i}{4}, \frac{i+1}{4}]$. Relics trigger the exact direction indicated by their arrowhead.
     - *Fruit Energy Shield*: Separated fruit pickups from direction warps. Picking up a fruit activates `shieldTimer = 10.0f`. While active, a pulsating cyan energy bubble renders around the warrior, and spike hazards, flying raptor birds, and sky mines are harmlessly deflected without damage.
   - **Idle Stance at Launch**:
     - On game initialization and reset, `isGameIdleAtStart = true`, zeroing initial velocity $\mathbf{v} = (0, 0)$ and locking animation to `ANIM_IDLE`. Pressing `A`, `D`, or `Space` dismisses the idle state and initiates active traversal.
   - **Sky-Fall Mini-Game Hazard Density Retuning**:
     - Increased obstacle spacing along the descent vector from $5.0\,\text{m}$ to $14.0\,\text{m}$.
     - Reduced bird flock spawn probability to $50\%$ with calibrated horizontal flight speeds ($3.0 - 4.5\,\text{m/s}$).
     - Reduced spike mine density to $33\%$ and tightened collision AABB bounding radii to $0.85\,\text{m}$ for crisp, skill-based dodging.

9. **Mini-Game Scene Direction Relics & Safe Runner Return (`v0.5.7` & `v0.5.8`)**:
   - **Post-15s Immediate Relic Injection (Ladders & Sky-Fall)**:
     - As soon as the 15-second survival trial expires (`scenePlayTime >= SCENE_DIRECTION_COOLDOWN`):
       - **In 3-Ladder Ascent (`UP`)**: The engine immediately injects glowing directional arrow relics directly ahead of the climber on Lane 0 (`DIR_LEFT`), Lane 2 (`DIR_RIGHT`), and Lane 1 (`DIR_LEFT`).
       - **In Sky-Fall Diving (`DOWN`)**: The engine retroactively converts all uncollected items ahead of the falling character into Left and Right direction relics, and spawns three staggered multi-lane gates in the descent route ($y - 5\,\text{m}$, $y - 10\,\text{m}$, $y - 15\,\text{m}$).
     - This ensures the player is never trapped without an immediate, visible exit trigger after surviving the trial.
   - **Procedural Relic Streaming**:
     - Once $t \ge 15.0\,\text{s}$, runtime generation guarantees Left (Cyan) and Right (Gold) arrow relics in every descent tier of Sky-Fall and with 40% probability on ladder tracks.
   - **Expanded Aerial Collision Volume & Scaling**:
     - Aerial relics in Sky-Fall feature an enlarged collision volume ($1.85\,\text{m} \times 2.1\,\text{m}$) and scaled sprite footprint ($1.65\,\text{m}$ with a $1.65 + 0.20\sin(5t)$ pulse) so players diving at $9.2\,\text{m/s}$ easily touch the relic when steering sideways.
   - **Touch & Warp Scene Return**:
     - Touching a direction arrow relic plays `SFX_RELIC`, sets `platformerSavedDir`, `lastHorizontalDir`, and `currentDirection` to that direction, displays floating text (`"DIRECTION TRIGGERED!\nWARP <<< LEFT"` or `"WARP >>> RIGHT"`), and invokes `returnToPlatformerScene(true)`. The warrior immediately resumes running horizontally across the platformer world with ground snapping.

10. **In-Engine Video Playback & Asset Embedding Analysis (`v0.5.9`)**:
    - **In-Engine Video Architecture (`VideoPlayer`)**:
      - Uses native **Windows Media Foundation** (`IMFSourceReader`, `-lmfplat -lmfreadwrite -lmfuuid -lole32`), eliminating external library dependencies or third-party DLLs.
      - Decodes compressed H.264 video streams directly to 32-bit RGB/BGRA pixel frames on CPU and uploads them to an OpenGL 2D texture via `glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pData)`.
      - Concurrently decodes AAC audio into in-memory uncompressed PCM Wave format (`WAVEFORMATEX`), streaming synchronously with video timestamps via asynchronous `PlaySoundA`.
      - Features dynamic letterboxing preserving 16:9 aspect ratios across any resolution, an animated bottom progress bar, and user-friendly skip handling via `Space`, `Esc`, or `Enter`.
    - **Asset Embedding Feasibility & Size Reduction Analysis**:
      - *Is it possible to embed `assets/introVid.mp4` into game files?*
        **Yes.** It can be embedded either as a compiled Windows Resource (`.rc` script compiled via `windres` into a `.res`/`.o` object linked into `main.exe`) or converted into a C++ `const uint8_t introVideoBytes[]` array.
      - *Will embedding reduce file size?*
        **No.** MP4 files encoded with H.264/AAC are already compressed using modern lossy discrete cosine transform (DCT) and Huffman/CABAC entropy coding. Embedding an MP4 file into an executable simply appends its raw byte stream into the PE `.rsrc` or `.rodata` sections. The overall storage footprint remains equal or slightly larger. Real size reduction can only be achieved via video transcoding (e.g., downscaling from 1080p to 720p or reducing bitrate from ~2500 kbps to ~900 kbps, which reduces file size by ~70%).

11. **Standalone Single-File Game Packaging Architecture (`v0.5.10`)**:
    - **Methods to Deliver a Single Standalone Game File**:
      1. **Virtual Container / Single-File SFX**: Uses a self-extracting container or virtual filesystem (e.g., Enigma Virtual Box, 7-Zip SFX) to combine `main.exe`, `glfw3.dll`, and `assets/` into a single double-clickable `TheWenderer.exe`. On execution, it runs seamlessly in memory or a temporary directory without requiring the recipient to manually unzip or configure dependencies.
      2. **In-Binary Resource Embedding**: All sprite sheets, tilesets, and intro video streams are embedded into the PE `.rsrc` section using a Windows resource script (`.rc` compiled with `windres`). Assets are loaded in-memory (`stbi_load_from_memory` for images and `MFCreateSourceReaderFromByteStream` for video).
      3. **Portable Distribution Archive (`TheWenderer_Standalone.zip`)**: The industry-standard indie release model (Steam, itch.io). Created via `mingw32-make package`, it bundles the executable, dynamic libraries, and assets into a lightweight ~5.8 MB compressed zip file that runs portably on any Windows 10/11 system.

12. **Video Frame Alpha Testing, Media Foundation $XRGB$ Formats & Runtime Static Linking (`v0.5.11`)**:
    - **The $XRGB32$ Alpha Discard Pitfall**:
      - Windows Media Foundation's `MFVideoFormat_RGB32` decoder unpacks frames into 32-bit quad-bytes $[B, G, R, X]$, where the 4th byte $X$ is reserved and set to `0x00`.
      - When sampled by a 2D fragment shader containing transparency testing (`if (texColor.a < 0.12) discard;`), the GPU reads $a = 0.0$ and discards $100\%$ of the video pixels, resulting in audio playing over an entirely blank screen.
      - *Fix*: Decoupled alpha testing with a `uniform bool uDiscardAlpha;` flag. Sprites discard transparent pixels, while full-frame video quads bypass the discard branch. Furthermore, configuring `glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE)` and `GL_RGB` internal format forces the texture sampler to treat alpha as $1.0$ unconditionally.
    - **Eliminating Missing DLL Errors via Static Runtime Linking**:
      - Compiling C++ on Windows using MinGW/GCC without flags defaults to dynamic linking of `libstdc++-6.dll` and `libgcc_s_seh-1.dll`.
      - Adding `-static-libgcc -static-libstdc++` bakes these core language runtimes directly into the `.exe`, ensuring portable distribution without requiring compiler DLLs on target machines.

13. **Free Exploration Mode vs. Autonomous Endless Runner State Machine (`v0.5.12`)**:
    - **Velocity Resolution Decoupling**:
      - In an autonomous endless runner, the game loop forces a constant forward velocity vector ($v_x = \text{BASE\_RUN\_SPEED} \times \text{direction}$) every frame, overriding idle states unless jumping or climbing.
      - In **Free Exploration Mode** (toggled via `F`), this continuous forward injection is disabled:
        ```cpp
        if (freeExplorationMode) {
            bool leftPressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ||
                               (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
            bool rightPressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ||
                                (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
            if (leftPressed && !rightPressed) {
                player.vel.x = -moveSpeed;
                player.facingRight = false;
            } else if (rightPressed && !leftPressed) {
                player.vel.x = moveSpeed;
                player.facingRight = true;
            } else {
                player.vel.x = 0.0f; // Complete arrest of horizontal momentum
            }
        }
        ```
    - **Dynamic Idle Stance Transitions**:
      - When $v_x = 0$ on solid ground, the player state smoothly switches to `ANIM_IDLE` breathing rather than running in place.
      - On vertical ladder shafts, vertical velocity $v_y$ is halted when neither `W` nor `S` is pressed, allowing players to pause mid-shaft and inspect obstacles.

14. **Vertical Shaft Pass-Through & Mini-Game Scene Enclosure (`v0.5.13`)**:
    - **Underground Ladder Collision Deadlock**:
      - In tile-based platformers, a vertical ladder descending from the surface into subterranean caverns presents a geometric dilemma: the surface ground must support the player during horizontal walking, yet allow the player to pass downward when pressing `S` or `Down Arrow`.
      - If surface ground tiles have `isOneWay = false`, downward velocity $v_y < 0$ triggers ground collision, arresting vertical velocity to zero and locking the player in place.
      - *Resolution*:
        1. Underground ladder shafts are pre-registered during procedural chunk generation. Surface ground tiles covering a shaft are marked with `isOneWay = true` (acting as a passable hatch/grate), and subterranean dirt blocks (`sDirt`) directly inside the shaft are omitted.
        2. In the physics loop, any tile positioned inside the active ladder shaft (`isOnLadder && activeTunnel`) is excluded from collision checks.
        3. Proactive detection range is extended to `tunnel.maxY + 1.4f` so a player standing flush on top of the surface is within range to latch on.
    - **Deterministic Mini-Game Scene Exits**:
      - Mini-game scenes (`SCENE_LADDER_CLIMB` and `SCENE_SKY_FALL`) represent enclosed survival trials with explicit win/lose exit conditions.
      - Arbitrary manual scene switching via keys (`Up`/`Down`/`Space`) or jumping off outer boundaries breaks ludonarrative pacing.
      - Exits are strictly constrained to:
        1. **Collectibles**: Direction Arrow Relics (which unlock and spawn post-15s).
        2. **Hazard Penalties**: Collision with traps or birds, initiating cinematic hit/death reactions followed by safe resurrection on the primary running platform.

15. **Analytical Circular Shader Rendering & Single-Hit Shield Absorption (`v0.5.14`)**:
    - **Shader-Based Mathematical Circle vs Polygonal Disks**:
      - Traditionally, drawing a circle in OpenGL requires tessellating a triangle fan with 32 to 64 vertices. At close camera distances, the polygonal edges become visibly segmented.
      - In **The Wenderer**, circular force fields are rendered analytically directly in the fragment shader using a single reusable quad:
        ```glsl
        if (uIsCircle) {
            vec2 centeredCoord = TexCoord - vec2(0.5);
            float dist = length(centeredCoord);
            if (dist > 0.5) discard; // Discard all fragments outside unit radius

            float edgeAlpha = 1.0 - smoothstep(0.482, 0.50, dist); // C1 smooth anti-aliased edge
            float rim = pow(dist * 2.0, uRimPower); // Intense glowing outer rim
            float alpha = uColor.a * edgeAlpha * (0.35 + 0.65 * rim);
            vec3 glowColor = mix(uColor.rgb, vec3(1.0), rim * 0.7);
            FragColor = vec4(glowColor, alpha);
        }
        ```
      - *Key Benefits*: Zero additional vertex memory, scale-independent sub-pixel sharpness, and dynamic parameterized rim glow.
    - **Single-Hit Hazard Absorption & Post-Hit Grace Window**:
      - In a $60\,\text{FPS}$ physics loop, colliding with a hazard occurs over multiple consecutive frames as the player's bounding box traverses the hazard.
      - If an equipped shield does not decrement or break upon impact, the game enters a continuous deflection loop, spamming sound effects and floating HUD messages for up to 10 seconds.
      - Conversely, simply setting `shieldTimer = 0.0f` on frame 1 causes instant player death on frame 2 (16ms later), because the player is still intersecting the spike tile.
      - *Resolution*:
        1. On first contact with any hazard (platform spikes, ladder traps, diving mines, raptor birds), zero `shieldTimer = 0.0f`.
        2. Immediately activate a post-hit grace invulnerability window (`postHitInvulnTimer = 0.75f`).
        3. Emit a single definitive notification: `"SHIELD BROKE! [TRAP DEFLECTED]"` with a single shield shatter audio cue.

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
