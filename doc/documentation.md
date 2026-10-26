# The Wenderer: Technical Progress & System Documentation

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Institution**: Department of Computer Science & Engineering, UITS  
> **Project Title**: The Wenderer — Multi-Directional Endless Runner & Combat Platformer  
> **Engine / Stack**: C++17, Modern OpenGL (3.3 Core Profile), GLFW, GLAD, GLM, WinMM, Microsoft Media Foundation, `stb_image`

---

## 1. Project Overview & Scope

**The Wenderer** is a 2D side-view platformer and endless runner that subverts static runner mechanics. Rather than moving forward along a fixed axis, the player encounters autonomous dynamic direction shifts (running right, running left, climbing vertical shafts, and sliding downhill) at randomized intervals and via collectible mystery relics.

The gameplay is designed around multi-elevation world traversal:
- **Sky Canopy Tier** ($Y \approx +5.5\,\text{m}$): Floating sky platforms with coin ribbons and high-altitude sliding ramps.
- **Surface Platforms** ($Y \approx +1.0$ to $+2.8\,\text{m}$): Elevated wooden and stone ledges with drop-through descent (`S` / Down).
- **Surface Ground Level** ($Y = -0.5\,\text{m}$): Baseline terrain featuring open cavern dive chasms and roaming enemies.
- **Underground Cavern Zone** ($Y = -4.5\,\text{m}$): Subterranean corridors reached via steep ramps, connected back up by vertical ladder shafts.
- **Bedrock Safety Foundation** ($Y = -7.5\,\text{m}$): Safety net floor guaranteeing zero out-of-bounds abyss fall deaths.

---

## 2. Modular Architecture & Code Organization

The codebase is partitioned into 8 modular subsystems with strict separation of concerns:

```text
CGMLabProject_2DGame/
│
├── include/
│   ├── Common.h         # Shared definitions, physics constants, data models & UV helpers
│   ├── Audio.h          # Procedural WinMM audio synthesizer & MCI streaming interface
│   ├── Renderer.h       # Modern OpenGL shader compilation, quad VAO/VBO & draw pipeline
│   ├── World.h          # Procedural generation, streaming & collision detection interface
│   ├── UIManager.h      # Singleton HUD, floating text, enemy health bars, modals & menus
│   ├── LadderScene.h    # 3-track vertical ladder jumping mini-game subsystem
│   ├── SkyFallScene.h   # Aerial free-fall diving mini-game subsystem
│   ├── VideoPlayer.h    # Microsoft Media Foundation video playback & texture streaming
│   ├── font8x8_basic.h  # 8x8 monochrome bitmap font for immediate-mode text rendering
│   ├── glad/glad.h      # OpenGL 3.3 Core function pointers loader
│   ├── GLFW/glfw3.h     # Windowing, OpenGL context & user input handling
│   ├── glm/             # OpenGL Mathematics (vectors, matrices, transformations)
│   └── stb_image.h      # Single-header image decoder (PNG loading)
│
├── src/
│   ├── Audio.cpp        # Procedural PCM synthesis & Windows MCI streaming implementation
│   ├── Renderer.cpp     # GLSL shader program, reusable quad VAO/VBO & draw pipeline
│   ├── World.cpp        # Procedural world chunk generation, spike clusters & enemy spawning
│   ├── UIManager.cpp    # Telemetry HUD, floating combat text, modals & menu rendering
│   ├── LadderScene.cpp  # Ladder physics, rung generation, sawblade traps & exit timers
│   ├── SkyFallScene.cpp # Free-fall physics, raptor birds, floating mines & exit gates
│   ├── VideoPlayer.cpp  # Media Foundation decoder & OpenGL texture synchronization
│   ├── main.cpp         # Master game loop, player state machine & input handling
│   └── glad.c           # GLAD implementation
│
├── assets/
│   ├── samurai/         # Samurai Mack spritesheets (Run, Idle, Attack1, Defend_nobg)
│   ├── enemies/         # Bringer of Death (walk, attack, cast, spell) & Slimes (idle, move, transform, demon)
│   ├── world_tileset.png# Brackeys 256x256 tileset (grass, dirt, ladders, spikes)
│   ├── spikes.png       # Procedural 64x64 metallic spikes texture
│   ├── heart_pickup.png # 3-Lives ruby heart pickup icon
│   ├── shield_pickup.png# Heraldic golden-cyan defensive energy shield icon
│   ├── coin.png         # Brackeys 192x16 gold coin spritesheet (12 frames)
│   ├── intro.mp4        # Introductory cinematic video
│   ├── Quarter_in_the_Slot.mp3 # Background music track
│   └── thunder-after-crackle.mp3 # Bringer of Death thunder spell audio
│
├── Makefile             # Unified build script (WinMM, OpenGL32, GLFW3, GDI32, MFPlat)
└── README.md            # Quick start, controls & version history
```

---

## 3. Subsystem Breakdown & Technical Implementation

### 3.1 Core Models & Physics (`/include/Common.h`)
Serves as the shared data contract across all modules:
- **Calibrated Platformer Physics**:
  - `GRAVITY = -22.0f`: Fluid, readable vertical drop arc ($g = -22.0\,\text{m/s}^2$).
  - `JUMP_BURST = 11.2f`: Responsive jump launch impulse ($v_y = 11.2\,\text{m/s}$).
  - `BASE_RUN_SPEED = 4.8f`: Balanced horizontal traversal pace.
  - `CLIMB_SPEED = 3.8f`: Fluid vertical ladder ascent/descent rate.
  - `SLIDE_SPEED = 7.8f`: Controlled downhill slope velocity.
  - `DASH_BURST_SPEED = 14.5f`: High-speed dash burst with afterimage ghost trails.
- **Data Structures**:
  - `PlatformTile`: World bounds ($x, y, w, h$), tileset index, ladder flag, one-way drop flag, and biome theme.
  - `SpikeTrap`: Bounding box for procedural grouped metallic spikes (clusters of 2 to 4 spikes).
  - `DirectionRelic`: Mystery collectible type (0..3: Right, Left, Up, Down), floating oscillation phase.
  - `Enemy`: Finite state machine (`ENEMY_STATE_IDLE`, `PATROL`, `ATTACK`, `TRANSFORM`, `DEATH`, `JUMP`, `CAST`), position, velocity, dimensions, facing direction, attack wind-up timer, jump trajectory, spell cooldown.
  - `SpellHazard`: Multi-elevation telegraphed thunder spell with warning, active lightning impact, and dissipation phases.
  - `CharacterAnimSet`: OpenGL texture handles for Idle, Run, Jump, Fall, Attack, and Defend (`texDefend`).
- **Normalized UV Helpers**:
  - Maps pixel coordinates into normalized $[0, 1]$ GPU texture space for tiles, animated coins, shields, and hearts.

---

### 3.2 Hybrid Audio Engine (`/src/Audio.cpp` & `/include/Audio.h`)
Combines zero-dependency procedural synthesis with hardware-accelerated media streaming:
- **In-Memory Procedural PCM Synthesis**:
  - Dynamically builds 44-byte RIFF/WAVE headers followed by 16-bit mono PCM samples at $22,050\,\text{Hz}$ directly in RAM.
  - Dispatches non-blocking playback via `PlaySoundA` (`SND_MEMORY | SND_ASYNC`), eliminating missing asset errors:
    1. `SFX_SLASH`: Aerodynamic whoosh ($2400 \rightarrow 320\,\text{Hz}$) with steel ring harmonics ($3250\,\text{Hz}, 4950\,\text{Hz}$).
    2. `SFX_BLOCK`: Sharp metallic parry clink plus dual resonant ringing bells ($2850\,\text{Hz}, 4150\,\text{Hz}$) with $280\,\text{ms}$ decay.
    3. `SFX_ENEMY_HIT`, `SFX_ENEMY_ATTACK`, `SFX_ENEMY_DEATH`: Impact crunches and creature combat feedback.
    4. `SFX_HEART_PICKUP`: Dual-tone melodic restoration chime ($587\,\text{Hz} \rightarrow 880\,\text{Hz}$).
    5. `SFX_STEP`, `SFX_JUMP`, `SFX_CLIMB`, `SFX_SLIDE`, `SFX_COIN`, `SFX_RELIC`, `SFX_SHIFT`, `SFX_DEATH`.
- **Windows MCI Hardware-Accelerated Streaming**:
  - Streams looping background music [`assets/Quarter_in_the_Slot.mp3`](/assets/Quarter_in_the_Slot.mp3) using `mciSendStringA` at 50% default volume.
  - Features real-time `[M]` key mute/unmute toggle with HUD badges and floating text notifications.
  - Handles the 3-phase sequenced Demon Slime transformation audio ($5.5\text{s}$: wet splat $\rightarrow$ bubbling acid $\rightarrow$ monster roar), thunder spell strikes, and the Boss Demon Slime dying roar vocalization [`assets/dyingRoar.mp3`](/assets/dyingRoar.mp3).

---

### 3.3 Graphics Pipeline & Shader Architecture (`/src/Renderer.cpp` & `/include/Renderer.h`)
Implements Modern OpenGL 3.3 Core Profile graphics:
- **GLSL Shaders**:
  - **Vertex Shader**: Multiplies quad vertices by $P \times V \times M$. Applies dynamic UV sub-rect transformations via uniform `uUVRect = (u, v, du, dv)`:
    $$\text{TexCoord} = (u + aTexCoord.x \cdot du, \; v + aTexCoord.y \cdot dv)$$
  - **Fragment Shader**: Samples colors from `sampler2D uTexture` and applies alpha discard threshold (`if (texColor.a < 0.1) discard;`) multiplied by tint `uColor`.
- **Circular Energy Shield Discard Math**:
  - Renders a protective circular shield bubble by evaluating normalized quad distance in the fragment shader:
    $$r = \sqrt{(x - 0.5)^2 + (y - 0.5)^2}$$
    Pixels with $r > 0.5$ are discarded, creating a perfect glowing circular energy bubble without tessellating curved 3D geometry.
- **Single Reusable Quad VAO/VBO**:
  - A single $1 \times 1$ unit square (2 triangles, 6 vertices) is permanently resident in GPU VRAM. Every entity, tile, character sprite, and HUD element is rendered using this quad scaled, translated, and rotated by the Model Matrix ($M$).

---

### 3.4 Combat Subsystem & Katana Defend / Parry Mechanics
- **Melee Attack Hitbox Sweeps (`LMB` / `J` / `X`)**:
  - Animation: 6 frames at $12.0\,\text{FPS}$ ($0.5\text{s}$ total).
  - Active damage window restricted to frames 1–3 ($t \in [0.10\text{s}, 0.32\text{s}]$).
  - Projects an oriented forward bounding box ($2.4\text{m} \times 2.0\text{m}$ reaching $1.35\text{m}$ forward):
    $$\mathbf{p}_{\text{hit}} = \mathbf{p}_{\text{player}} + (\text{dirSign} \times 1.35\,\hat{\mathbf{i}})$$
  - Damage scaling: Bringer of Death (1 hit), Initial Slime (2 hits), Boss Demon Slime (5 hits).
- **Katana Defend / Block Stance (`RMB` / `K`)**:
  - Animation: 8 frames at $8.0\,\text{FPS}$ using [`assets/samurai/Defend_nobg.png`](/assets/samurai/Defend_nobg.png) ($1600\times 200$, $200\times 200\text{px}$ per frame).
  - **Calibrated Coordinate Frame & Proportions**: Downscaled by $0.3377\times$ via 64-sample supersampling to match [`assets/samurai/Idle.png`](/assets/samurai/Idle.png) pixel-for-pixel ($51$–$53\,\text{px}$ height, $Y=121$ foot grounding, $X=94$ horizontal centering). Renders uniformly under `playerRenderSize = (5.5f, 5.5f)` with zero size distortion or platform sinking.
  - Guard duration holds for up to $1.0\text{s}$ (`blockTimer <= 1.0f`) while horizontal movement is anchored in place.
  - Complete damage absorption: Deflects Slime contact, Bringer of Death scythe cleaves, telegraphed thundercloud strikes, and Demon Slime Boss charges/smashes with zero life loss.
  - Parry impact triggers `SFX_BLOCK`, spawns `"[PARRY]"` floating text, grants $0.6\text{s}$ grace invulnerability, and applies a **2.0-second cooldown** (`blockCooldown = 2.0f`).

---

### 3.5 Enemy Finite State Machines & AI
- **Bringer of Death**:
  - Patrols surface and caverns at $1.8\,\text{m/s}$.
  - Calibrated sprite offset ($1.077\text{m}$) ensures exact visual centering on platform hitboxes.
  - **Multi-Elevation Telegraphed Thunder Spell**: Detects players across vertical platform elevations up to $14.0\text{m}$ and distances $3.0\text{m}$ to $14.0\text{m}$. Spawns an animated 16-frame thunder cloud with a 3-phase lifecycle:
    1. *Cloud Gathering Telegraph (Frames 0–6, $t \in [0.0\text{s}, 0.58\text{s}]$)*: Cloud forms overhead with `"THUNDER CLOUD GATHERING! MOVE!"` warning; zero damage dealt, allowing player to dodge or raise guard.
    2. *Mid-Animation Thunder Strike (Frames 7–10, $t \in [0.58\text{s}, 0.85\text{s}]$)*: Lightning connects with the ground with explosive acoustic impact; evaluates damage only if player remains in the strike zone.
    3. *Dissipation (Frames 11–15, $t \in [0.85\text{s}, 1.35\text{s}]$)*: Residual sparks fade out harmlessly.
- **Initial Slime & Metamorphosis**:
  - Hops along platforms at $0.75$–$0.9\,\text{m/s}$. Calibrated vertical placement (`spawnY = footY + 1.2f`) rests quad base directly on platform tops ($Y = 0.0\,\text{m}$ surface, $Y = -4.0\,\text{m}$ cavern). Ground raycasting aligns with `hasSolidGroundAt`, enabling smooth autonomous roaming and ledge turnarounds. Defeating it in 2 hits triggers `ENEMY_STATE_TRANSFORM` (33 frames at $6.0\,\text{FPS}$, $5.5\text{s}$ duration) with sequenced audio before the Boss Demon Slime emerges.
- **Boss Demon Slime**:
  - Giant $8.5\text{m}$ boss with 5 HP and an overhead health bar.
  - **Platform Jumping (`demon_smash.png`)**: 18-frame sequence with crouch charge (frames 0–4), ballistic leap ($v_y = 11.0$–$14.5\,\text{m/s}$), gravity flight, dynamic tile touchdown, and ground smash shockwave.
  - **Frontal Damage Sector**: Cleave attacks require the player to be physically in front. Dashing behind the boss cleanly evades the strike.
  - **Death Sequence & Dying Roar**: When reduced to 0 HP, enters a 22-frame dissolution sequence (`05_d_death.gif` / `demon_death.png`) playing at $6.0\,\text{FPS}$ ($3.67\,\text{s}$ duration) accompanied by the asynchronous playback of [`assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) with a $4.0\,\text{s}$ lingering smoke despawn threshold.
  - **Boss Evasion**: Running $>28\text{m}$ away or dashing through the boss ($>8\text{m}$) despawns the boss safely.

---

### 3.6 Procedural World Generation & Streaming (`/src/World.cpp` & `/include/World.h`)
- **Endless Chunk Streaming**:
  - Generates chunks $35\,\text{m}$ ahead in both horizontal directions ($+X$ and $-X$).
  - Automatically prunes chunks farther than $85\,\text{m}$ behind the player, keeping memory usage constant and sustaining 60+ FPS indefinitely.
- **High-Density Slime Spawning & Starter Encounters**:
  - **Guaranteed Starter Slimes**: Spawns 2 initial slimes at $X = +17.5\,\text{m}$ and $X = -18.0\,\text{m}$ on the initial spawn chunk, ensuring immediate combat gameplay without long empty treks.
  - **100% Chunk Spawn Rate & Increased Density**: Replaced probabilistic spawning with guaranteed generation of 2 to 4 slimes (`numSlimes = 2 + (std::rand() % 3)`) per $35\,\text{m}$ chunk.
  - **Multi-Elevation Cavern Spawning**: Implemented a 33% probability for slimes to spawn in subterranean caverns (`CAVERN_FLOOR_Y + 0.5\,\text{m}`) as well as surface ground, populating underground explorations.
- **Grouped Spike Clusters & Vertical-Aware Enemy Separation**:
  - Spawns platform spikes in grouped clusters of 2 to 4 spikes (`SpikeTrap`).
  - **Vertical-Aware Separation Buffers**: Bringer of Death and Slimes maintain a $9.0\,\text{m}$ horizontal buffer only when sharing the same elevation tier ($\Delta y < 2.5\,\text{m}$), allowing surface Bringers of Death and cavern slimes to coexist within the same horizontal sector without blocking spawns. Slime-to-slime spacing is calibrated to $4.0\,\text{m}$.
- **Unblocked Cavern Chasm Carving**:
  - Downhill dive slopes are reserved first, and surface ground generation explicitly skips spans above dive slopes ($[startX - 0.6\,\text{m}, endX + 0.6\,\text{m}]$), ensuring open cavern entrances.

---

### 3.7 UI & Mini-Game Subsystems
- **`UIManager`**:
  - Thread-safe singleton managing the score counter, coin meter, distance tracking, 3 animated heart life pips, Guard status badge, overhead segmented enemy health bars, floating combat text, and glassmorphic modals.
- **`LadderScene` & `SkyFallScene`**:
  - Vertical 3-track climbing trial and aerial free-fall diving trial. Both include real-time survival countdown badges (`SURVIVE UNTIL EXIT: XX.Xs`), exit announcement banners, and direction arrow relic gates.
- **`VideoPlayer`**:
  - Decodes `intro.mp4` via Microsoft Media Foundation directly into an OpenGL texture with presentation timestamp synchronization.

---

## 4. Compilation & Build Specification

The project compiles with GCC/MinGW-w64 targeting C++17:
```bash
# Build binary via Makefile (make or mingw32-make)
make
# (or mingw32-make)

# Or compile and run directly:
make win

# Or compile directly with g++:
powershell -Command "if (!(Test-Path 'build')) { New-Item -ItemType Directory -Path 'build' | Out-Null }; Copy-Item lib/*.dll build/ -Force"
g++.exe -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ \
    -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp \
    ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp \
    ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c \
    -o ./build/main.exe -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm \
    -lmfplat -lmfreadwrite -lmfuuid -lole32

# Package standalone portable distribution zip:
make package
# (or mingw32-make package)
```
Linked libraries:
- `glfw3`: Window creation, input handling, and OpenGL context management.
- `opengl32`: Hardware-accelerated graphics API.
- `gdi32`: Windows graphics device interface support.
- `winmm`: Windows Multimedia API for non-blocking procedural audio synthesis and MCI MP3 streaming.
- `mfplat`, `mfreadwrite`, `mfuuid`, `ole32`: Microsoft Media Foundation APIs for hardware video decoding.
