# The Wenderer: Technical Progress & System Documentation

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Institution**: Department of Computer Science & Engineering, UITS  
> **Project Title**: The Wenderer — Multi-Directional Endless Runner & Platformer  
> **Engine / Stack**: C++17, Modern OpenGL (3.3 Core Profile), GLFW, GLAD, GLM, WinMM, `stb_image`

---

## 1. Project Overview & Scope

**The Wenderer** is a 2D side-view platformer and endless runner that subverts static runner mechanics. Rather than moving forward along a fixed axis, the player encounters autonomous dynamic direction shifts (running right, running left, climbing vertical shafts, and sliding downhill) at randomized intervals and via collectible mystery relics. 

The gameplay is designed around multi-elevation world traversal:
- **Sky Canopy Tier** ($Y \approx +5.5\,\text{m}$)
- **Surface Platforms** ($Y \approx +1.0$ to $+2.8\,\text{m}$)
- **Surface Ground Level** ($Y = -0.5\,\text{m}$)
- **Underground Cavern Zone** ($Y = -4.5\,\text{m}$)
- **Bedrock Safety Foundation** ($Y = -7.5\,\text{m}$) — guaranteeing zero out-of-bounds abyss fall deaths.

---

## 2. Modular Architecture & Code Organization

The codebase has been refactored from a monolithic file into a modular structure:

```text
CGMLabProject_2DGame/
│
├── include/
│   ├── Common.h         # Constants, enumerations, data models, UV helpers
│   ├── Audio.h          # Procedural WinMM audio synthesis interface
│   ├── Renderer.h       # Modern OpenGL shader compilation & quad drawing interface
│   ├── World.h          # Procedural generation, streaming & collision interface
│   ├── glad/glad.h      # OpenGL 3.3 Core function pointers loader
│   ├── GLFW/glfw3.h     # Windowing, context & user input handling
│   ├── glm/             # OpenGL Mathematics (vectors, matrices, transformations)
│   └── stb_image.h      # Single-header image decoder (PNG loading)
│
├── src/
│   ├── Audio.cpp        # In-memory WAV assembly & real-time audio playback
│   ├── Renderer.cpp     # GLSL shaders, reusable quad VAO/VBO & draw pipeline
│   ├── World.cpp        # Multi-elevation generation, chasm carving & AABB collision
│   ├── main.cpp         # Main game loop, player state machine & HUD overlay
│   └── glad.c           # GLAD implementation
│
├── assets/
│   ├── world_tileset.png # Brackeys 256x256 tileset (grass, dirt, ladders, spikes)
│   ├── platforms.png    # Brackeys 64x64 platform variations (grass, snow, desert, dungeon)
│   ├── coin.png         # Brackeys 192x16 gold coin spritesheet (12 frames)
│   ├── fruit.png        # Brackeys 64x64 collectible mystery relics (4 fruits)
│   └── player_sheet.png # LPC animated character spritesheet (run, climb, jump, slide)
│
├── Makefile             # Unified build script (WinMM, OpenGL32, GLFW3, GDI32)
└── README.md            # Quick start, controls & version history
```

---

## 3. Module Breakdown & Technical Implementation

### 3.1 [`/include/Common.h`](/include/Common.h)
Serves as the central contract for the entire engine:
- **Calibrated Platformer Physics**:
  - `GRAVITY = -22.0f`: Fluid, readable vertical drop arc.
  - `JUMP_BURST = 11.2f`: Responsive jump launch velocity.
  - `BASE_RUN_SPEED = 4.8f`: Balanced, readable horizontal traversal rate.
  - `CLIMB_SPEED = 3.8f`: Fluid vertical ladder ascent/descent.
  - `SLIDE_SPEED = 7.8f`: Exciting yet controllable downhill slope acceleration.
  - `MIN_SWITCH_TIME = 6.0f`, `MAX_SWITCH_TIME = 10.0f`: Pacing intervals for autonomous direction flips.
- **Data Models**:
  - `PlatformTile`: World coordinates, dimensions, tileset index, ladder flag, one-way drop flag, and active biome theme.
  - `LadderTunnel`: Centerline $X$, vertical bounds ($minY, maxY$), full shaft width, and presence indicators.
  - `SteepSlope`: Start and end endpoints, slope incline angle $\theta = \arctan(\Delta y / \Delta x)$, and theme.
  - `SpikeTrap`: Lethal AABB hazard bounding box.
  - `DirectionRelic`: Mystery fruit type (0..3), floating oscillation phase, collection state.
  - `AnimatedCoin`: Animation frame timer, collection state.
  - `CornerNotification`: HUD glassmorphism card parameters (icon, category tag, title, description, color, countdown timer).
- **UV Sub-Rect Helpers**:
  - Maps pixel coordinates into normalized $[0, 1]$ GPU texture space for the player spritesheet, Brackeys world tileset ($16 \times 16$ grid), animated coins ($12$ frames), and fruit relics.

---

### 3.2 [`/src/Audio.cpp`](/src/Audio.cpp) & [`/include/Audio.h`](/include/Audio.h)
Provides zero-dependency, real-time procedural sound synthesis:
- **In-Memory RIFF/WAVE Assembly**:
  - Dynamically builds a standard 44-byte RIFF/WAVE header followed by 16-bit mono PCM samples at 22,050 Hz directly in RAM.
  - Eliminates external sound file dependencies, missing asset errors, and external decoder libraries.
- **Asynchronous Audio Dispatch**:
  - Uses the Windows Multimedia API (`PlaySoundA` with `SND_MEMORY | SND_ASYNC`) for non-blocking playback that never stalls the rendering loop.
- **Procedural Sound Palette**:
  1. `SFX_STEP`: Short low-frequency footstep click ($130 \rightarrow 45\,\text{Hz}$).
  2. `SFX_JUMP`: Upward springy pitch glide ($190 \rightarrow 620\,\text{Hz}$).
  3. `SFX_CLIMB`: Dual-harmonic wooden ladder rung tap ($460\,\text{Hz} + 880\,\text{Hz}$).
  4. `SFX_SLIDE`: Rushing wind and friction noise during downhill slope sliding.
  5. `SFX_COIN`: Bright two-tone crystal chime ($987\,\text{Hz} \rightarrow 1318\,\text{Hz}$).
  6. `SFX_RELIC`: Ascending 4-note chord arpeggio ($C_5, E_5, G_5, C_6$).
  7. `SFX_SHIFT`: Portal warp transition whoosh during direction and biome changes.
  8. `SFX_DEATH`: Low distorted impact crunch upon hitting spiky traps.

---

### 3.3 [`/src/Renderer.cpp`](/src/Renderer.cpp) & [`/include/Renderer.h`](/include/Renderer.h)
Implements a Modern OpenGL 3.3 Core Profile graphics pipeline:
- **GLSL Shaders**:
  - **Vertex Shader**: Transforms vertices via $P \times V \times M$ matrix multiplication. Dynamically calculates texture coordinates on the GPU using uniform `uUVRect = (u, v, du, dv)`:
    $$\text{TexCoord} = (u + aTexCoord.x \cdot du, \; v + aTexCoord.y \cdot dv)$$
  - **Fragment Shader**: Samples pixel color from `sampler2D uTexture` and applies pixel-art transparency threshold (`if (texColor.a < 0.12) discard;`) multiplied by `uColor`.
- **Reusable Quad VAO/VBO**:
  - A single $1 \times 1$ unit square (2 triangles, 6 vertices) is allocated once in GPU VRAM. All game entities (tiles, character sprites, background pillars, HUD cards) are drawn using this single quad scaled and translated via the Model Matrix ($M$).
- **Texture Management**:
  - Loads PNG files straight into GPU VRAM using `stb_image` with `GL_NEAREST` filtering for crisp, distortion-free pixel art.

---

### 3.4 [`/src/World.cpp`](/src/World.cpp) & [`/include/World.h`](/include/World.h)
Handles multi-elevation world generation, collision resolution, and memory streaming:
- **Multi-Elevation Strata**:
  - **Sky Canopy Tier ($Y \approx +5.5\,\text{m}$)**: Suspended high-altitude platforms accessed via tall ladder shafts, concluding with a grand downhill slope returning to the surface.
  - **Surface Platforms ($Y \approx +1.0$ to $+2.8\,\text{m}$)**: One-way platforms allowing drop-through descent (`S` / Down).
  - **Surface Ground ($Y = -0.5\,\text{m}$)**: Baseline terrain featuring open chasms.
  - **Underground Cavern Zone ($Y = -4.5\,\text{m}$)**: Subterranean corridor entered via downward dive slopes; contains multiple upward ladder shafts connecting back to surface ledges.
  - **Bedrock Foundation ($Y = -7.5\,\text{m}$)**: Bottom safety floor guaranteeing zero out-of-bounds abyss fall deaths.
- **Unblocked Cavern Chasm Carving**:
  - Downhill dive slopes are planned first; their horizontal spans ($[startX - 0.6\,\text{m}, endX + 0.6\,\text{m}]$) are registered as open chasms.
  - The surface ground generator explicitly skips placing surface tiles across these spans, keeping the path down into the cavern completely unobstructed.
- **Dynamic Procedural Streaming**:
  - Generates chunks 35 units ahead in both horizontal directions ($+X$ and $-X$) as the player travels.
  - Prunes distant entities farther than 85 units behind the player, ensuring bounded memory usage and sustained 60 FPS performance indefinitely.
- **AABB Collision Detection**:
  - Axis-Aligned Bounding Box checks for landing resolution, collectible pickups, ladder triggers, and lethal spike collisions.

---

### 3.5 [`/src/main.cpp`](/src/main.cpp)
Controls the game loop, player physics state machine, and player input:
- **Ladder Auto-Latching & Stopping**:
  - When passing or approaching a ladder shaft while `currentDirection == DIR_UP` (or pressing `W`/Up near a shaft), horizontal velocity is instantly zeroed ($v_x = 0.0\,\text{m/s}$).
  - Player $X$ smoothly centers onto the shaft centerline (`tunnel.x`), eliminating lateral drift and ensuring fluid vertical climb.
- **Trajectory Direction Memory (`lastHorizontalDir`)**:
  - Remembers prior horizontal running trajectory (`DIR_RIGHT` or `DIR_LEFT`).
  - When the player reaches the summit of a ladder shaft (`playerPos.y >= tunnel.maxY`), the upward state cleanly completes and the player automatically resumes their prior horizontal course.
- **Downhill Slope Sliding & Midpoint Interrupts**:
  - Stepping onto a steep slope locks the player to the surface contour $Y_{\text{surf}}(x)$ and accelerates downward at $7.8\,\text{m/s}$.
  - The sprite tilts along the incline angle $\theta$.
  - **Midpoint Jump**: Pressing `Space`, `W`, or Up during a slide breaks slope attachment, launches into a vertical jump ($v_y = 11.2\,\text{m/s}$), and plays the jump SFX.
  - **Midpoint Steer**: Pressing `A` or `D` interrupts the slide and switches direction immediately.
- **Dynamic Biomes & Transition Phase**:
  - 4 Biomes: Emerald Forest, Frozen Glacier, Sunset Canyon, Mystic Cavern.
  - Direction changes trigger a transition phase with time dilation ($0.5\times$ speed for $0.25\,\text{s}$), a screen energy flash, and smooth environmental color interpolation (`glm::mix`).
- **Corner HUD Notification Card**:
  - Top-left glassmorphism card displaying rotating item sprites, category tags, titles, and explanations on every pickup for 2.8 seconds.

---

## 4. Compilation & Build Specification

The project compiles with GCC/MinGW-w64 targeting C++17:
```bash
# Build via Makefile
mingw32-make

# Or compile directly with g++:
g++.exe -std=c++17 -Wall -Wextra -O2 -I./include \
    ./src/main.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/glad.c \
    -o ./build/main.exe -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm
```
Libraries linked:
- `glfw3`: Cross-platform window creation and OpenGL context initialization.
- `opengl32`: Core hardware-accelerated graphics API.
- `gdi32`: Windows graphics device interface support.
- `winmm`: Windows Multimedia API for non-blocking asynchronous audio playback.
