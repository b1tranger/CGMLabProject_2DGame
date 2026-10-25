# The Wenderer: Complete Project Changelog & Version History 📜

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Repository**: [CGMLabProject_2DGame](/)  
> **Archive Source Directory**: [`/doc/prompts/`](/doc/prompts/)  

All entries in this changelog map directly to session archives recorded in [`/doc/prompts/`](/doc/prompts/), documenting the evolutionary milestones of **The Wenderer** from initial prototype to a modular, multi-elevation endless platformer.

---

## Table of Versions

| Release Version | Session Version | Date | Core Milestone / Focus |
| :--- | :--- | :--- | :--- |
| **v0.5.15** | [v4.17](#v417---ladder-hero-scale-fix-post-intro-tutorial-modal-shield-pickup-icon-and-scene-exit-timers) | 2026-10-25 | Ladder Hero Sprite Scale Restoration, Post-Intro Tutorial Modal Dialog, Dedicated Shield Pickup Icon & Mini-Game Exit Timers |
| **v0.5.14** | [v4.16](#v416---circular-energy-shield-bubble-and-single-hit-trap-absorption) | 2026-10-25 | Circular Energy Shield Bubble Rendering via Fragment Discard & Single-Hit Trap Absorption with Grace Invulnerability |
| **v0.5.13** | [v4.13](#v413---mini-game-scene-exit-restrictions-and-underground-ladder-descent-fix) | 2026-10-25 | Mini-Game Scene Exit Restrictions (Relics/Hit Only) & Underground Ladder Descent Pass-Through Fix |
| **v0.5.12** | [v4.12](#v412---free-exploration-mode-and-manual-movement-toggle) | 2026-10-25 | Free Exploration Mode (`F` Key) — Manual A/D Movement, Dynamic Idle Stance, Zero Auto-Run Drift & Ladder Hold |
| **v0.5.11** | [v4.11](#v411---fix-standalone-missing-dll-and-video-rendering-alpha-discard) | 2026-10-25 | Fixed Standalone `msys-stdc++-6.dll` Dependency via Static Runtime Linkage & Restored Intro Video Visuals (Alpha Discard Fix) |
| **v0.5.10** | [v4.10](#v410---standalone-single-file-game-packaging-analysis) | 2026-10-25 | Standalone Single-File Packaging Architecture, In-Binary Resource Embedding vs. Virtual Container SFX & Automated `package` Target |
| **v0.5.9** | [v4.9](#v49---in-engine-intro-video-playback-and-asset-embedding-analysis) | 2026-10-25 | In-Engine Hardware-Accelerated Intro Video Playback (Media Foundation), Audio Sync, Cinematic HUD & Size Optimization Analysis |
| **v0.5.8** | [v4.8](#v48---sky-fall-down-scene-left-and-right-direction-relics) | 2026-10-25 | Sky-Fall Down Scene Left & Right Direction Relic Corridors, Multi-Gate Spawns & Platform Runner Resumption |
| **v0.5.7** | [v4.7](#v47---ladder-scene-direction-trigger-relics--exit-spawning) | 2026-10-25 | Ladder Climb Scene Left/Right Direction Relic Spawns, Post-15s Immediate Triggers & Safe Runner Return |
| **v0.5.6** | [v4.6](#v46---left-chunk-persistence-direction-arrows-shield-fruits-idle-start-single-hero-and-down-scene-pacing) | 2026-10-25 | Left Chunk Persistence, 4-Direction Arrowheads, 10s Shield Fruits, Idle Start, Single Hero (Samurai Mack) & Sky-Fall Deconfliction |
| **v0.5.5** | [v4.5](#v45---dynamic-day-night-cycle-thematic-background-sceneries-and-scene-cooldown) | 2026-10-25 | 4 Thematic Background Sceneries, 1.5m Day/Night Cycle (Starts at Night) & 15s Scene Trial Cooldown |
| **v0.5.4** | [v4.4](#v44---dash-mechanic-left-starting-platforms-and-dynamic-up-down-mini-game-scenes) | 2026-10-25 | Dash on SHIFT, Left Starting Platforms, 3-Parallel Ladder Climb Scene & Sky-Fall Diving Scene |
| **v0.5.3** | [v4.3](#v43---downward-ladder-entry--procedural-world-generation-analysis) | 2026-10-25 | Downward Ladder Descent via S/Down Key & Bidirectional World Generation Analysis |
| **v0.5.2** | [v4.2](#v42---floating-text-for-fruit-pickups-and-auto-shift-disabling) | 2026-10-25 | Floating Text for Fruit Pickups & Disabling Invisible Auto-Shifts |
| **v0.5.1** | [v4.1](#v41---slow-down-running-and-climbing-animation-framerate) | 2026-10-25 | Running & Ladder Climbing Framerate Calibration (8.0 & 3.5 FPS) |
| **v0.5.0** | [v4.0](#v40---samurai-ninja-player-animations-pause-menu-and-score-counter) | 2026-10-24 | Samurai & Ninja Sprites, ESC Pause Menu & On-Screen Score HUD |
| **v0.4.3** | [v3.6](#v36---technical-documentation--academic-code-guide) | 2026-10-24 | Technical Progress Documentation & Academic Guide |
| **v0.4.2** | [v3.5](#v35---modular-code-architecture--cavern-chasm-fixes) | 2026-10-24 | Modular Engine Decoupling, Ladder Stopping & Cavern Chasm Fixes |
| **v0.4.1** | [v3.4](#v34---multi-elevation-worlds-caverns-sky-tier--midpoint-interrupts) | 2026-10-24 | Caverns, Sky Canopy, Direction Memory & Midpoint Interrupts |
| **v0.4.0** | [v3.2](#v32---ladder-tunnels-steep-slopes-audio-sfx--dynamic-endless-platforms) | 2026-10-24 | Dynamic Platforms, Ladder Tunnels, Slopes, Audio SFX & Biomes |
| **v0.3.0** | [v3.1](#v31---brackeys-world-assets-relics-ladders--spiky-traps) | 2026-10-24 | Brackeys Asset Integration, Direction Relics, Spikes & Bedrock |
| **v0.2.0** | [v3.0](#v30---side-view-platformer-transition--animated-lpc-sprites) | 2026-10-24 | Side-View Perspective Shift & LPC Animated Character Sprites |
| **v0.1.2** | [v2.0](#v20---project-structure-clarification--version-archiving-guidelines) | 2026-10-21 | Project Structure Clarification & Archiving Guidelines |
| **v0.1.1** | [v1.2](#v12---root-relative-path-standardization) | 2026-10-21 | Root-Relative Path Standardization Across Documentation |
| **v0.1.0-hotfix** | [v1.1](#v11---makefile-msys2-shell-environment-fix) | 2026-10-18 | Makefile MSYS2 `/bin/sh` Syntax Error Fix |
| **v0.1.0** | [v1.0](#v10---initial-project-analysis--phase-1-implementation) | 2026-10-17 | Initial Engine Setup & Phase 1 Prototype |

---

## Detailed Version Logs

### [v0.5.15] - 2026-10-25
#### Session Archive: [`v4.17 - Ladder Hero Scale Fix, Post-Intro Tutorial Modal, Shield Pickup Icon, and Scene Exit Timers.md`](/doc/prompts/v4.17%20-%20Ladder%20Hero%20Scale%20Fix,%20Post-Intro%20Tutorial%20Modal,%20Shield%20Pickup%20Icon,%20and%20Scene%20Exit%20Timers.md)
* **Visual Presentation & Proportions**:
  - **Ladder Climb Hero Scale Restoration**:
    - *Root Cause*: In `SCENE_LADDER_CLIMB`, the climbing Samurai Mack sprite quad was translated without matrix scaling, rendering at a tiny default $1.0\times 1.0$ meter dimension ($5.5\times$ smaller than intended).
    - *Resolution*: Added `modelHero = glm::scale(modelHero, glm::vec3(playerRenderSize.x, playerRenderSize.y, 1.0f));` restoring hero dimensions to $5.5\times 5.5$ meters, perfectly uniform with runner and sky-fall scenes.
* **Onboarding & Tutorial Guidance**:
  - **Post-Intro Tutorial Window Modal**:
    - Automatically displays an elegant glassmorphic modal dialog upon conclusion or skip of the intro video, appearing strictly once per game launch (`hasShownInitialTutorial`).
    - Communicates the narrative essence of "Wandering": *"Endless runner with traps to avoid and shields to pickup. Change movement direction and find the end goal of this venture."*
    - Directs players to inspect the Pause Menu for full keybindings and reminds them that pressing **`[T]`** inside the Pause Menu reopens the tutorial dialog at any time.
    - Updated Pause Menu key list to clearly feature: `[T]             TUTORIAL / HOW TO PLAY`.
* **Collectibles & Visual Clarity**:
  - **Dedicated Shield Icon Pickups**:
    - *Eliminated Ambiguity*: Replaced fruit sprites with a dedicated heraldic Shield icon texture (`assets/shield_pickup.png`) generated with golden and cyan trims, radiant star core emblem, and glowing energy halo.
    - Updated all pickup banners and notifications: `"+10s ENERGY SHIELD!"` and `"ENERGY SHIELD EQUIPPED [Absorbs 1 hazard hit!]"`.
* **Scene Pacing & Exit Predictability**:
  - **Top-Center Countdown & Exit Banner HUD**:
    - *Eliminated Abrupt Transitions*: Replaced sudden direction relic pop-ins with a real-time top-center HUD badge displaying `SURVIVE UNTIL EXIT: XX.Xs` with an animated countdown progress bar.
    - *Definitive Exit Notice*: When the 15-second survival trial completes, triggers an audio cue and displays a prominent glowing banner: `=== EXIT APPEARED! === [TOUCH A DIRECTION ARROW TO RETURN]` alongside directional arrow relics positioned visibly ahead on the tracks/sky lanes.

### [v0.5.14] - 2026-10-25
#### Session Archive: [`v4.16 - Circular Energy Shield Bubble and Single-Hit Trap Absorption.md`](/doc/prompts/v4.16%20-%20Circular%20Energy%20Shield%20Bubble%20and%20Single-Hit%20Trap%20Absorption.md)
* **Mechanics & Combat State**:
  - **Single-Hit Shield Trap Absorption**:
    - *Root Cause of Deflection Loop*: The fruit shield timer (`shieldTimer`) was never reduced or cleared upon colliding with hazards, allowing players to overlap spikes, mines, or raptors for up to 10 seconds while spamming "DEFLECTED" sound and visual notifications on every single frame.
    - *Immediate Shield Break*: A trap impact now immediately destroys the shield (`shieldTimer = 0.0f`), triggering a single distinct announcement: `"SHIELD BROKE! [TRAP DEFLECTED]"`.
  - **Post-Hit Grace Invulnerability Window**:
    - *Immediate Re-Hit Prevention*: When the shield breaks, a brief $0.75\,\text{s}$ grace window (`postHitInvulnTimer = 0.75f`) is applied so the player can safely clear the hazard's physical bounding box without suffering an instantaneous fatal hit on frame 2.
    - Applied universally across all three game scenes: Platformer Spiky Traps, Ladder Climb Traps, and Sky-Fall Diving Raptors/Mines.
* **Rendering & Visual Fidelity**:
  - **Analytical Circular Force-Field Shader**:
    - *Eliminated Box Quad*: Replaced the previous flat square quad with a true analytical circular shield rendered via `gameRenderer.drawCircle()`.
    - *Fragment Discard & Edge Smoothing*: The shader computes normalized distance from quad center $\vec{p} = \text{TexCoord} - (0.5, 0.5)$ and discards fragments where $\|\vec{p}\| > 0.5$. Outer edge pixels are anti-aliased via `smoothstep(0.482, 0.50, dist)`.
    - *Glowing Electric Rim*: Applies a power-curve rim glow formula $\text{rim} = \text{pow}(\text{dist} \times 2.0, \text{uRimPower})$, yielding a translucent protective core and an intense, shimmering electric perimeter wrapped snugly around the hero.

### [v0.5.13] - 2026-10-25
#### Session Archive: [`v4.13 - Mini-Game Scene Exit Restrictions and Underground Ladder Descent Fix.md`](/doc/prompts/v4.13%20-%20Mini-Game%20Scene%20Exit%20Restrictions%20and%20Underground%20Ladder%20Descent%20Fix.md)
* **Mechanics & Level Interactivity**:
  - **Mini-Game Scene Exit Restrictions**:
    - *Disallowed Key Switching*: Removed arbitrary manual key transitions that switched between `SCENE_LADDER_CLIMB` and `SCENE_SKY_FALL` via Up/Down keys, and disabled leaping off outer edge lanes to exit.
    - *Strict Exit Rule*: Players can now strictly only exit mini-game scenes by either:
      1. Collecting Direction Arrow Relics (which unlock and spawn across lanes after the 15-second survival trial completes).
      2. Suffering an obstacle or trap hit (which plays the death/hit sequence, triggers screen darkening, and cleanly resurrects the player back in the main platformer runner).
  - **Underground Ladder Descent Deadlock Fix**:
    - *Root Cause*: Standing on the surface ground over an underground ladder and pressing `S` or `Down Arrow` caused the player to freeze without descending. This occurred due to two issues:
      1. Solid ground and dirt tiles (`sTop` and `sDirt` with `isOneWay = false`) were generated directly across the top of underground shafts, snapping downward vertical velocity to zero on collision.
      2. The proactive ladder vertical threshold checked `playerPos.y <= tunnel.maxY + 0.6f`, which evaluated to `false` for a player standing on top of surface ground (`playerPos.y = 0.6f > tunnel.maxY + 0.6f = 0.1f`).
    - *Resolution*:
      1. Pre-planned underground ladder shaft X coordinates in [`src/World.cpp`](/src/World.cpp). Surface ground tiles directly over shafts are configured as one-way hatches (`isOneWay = true`) and solid dirt blocks inside the shaft are omitted.
      2. Expanded ladder detection vertical threshold to `tunnel.maxY + 1.4f` in [`src/main.cpp`](/src/main.cpp).
      3. Bypassed collision resolution for tiles inside the active ladder shaft when the player is climbing/descending on a ladder.
      4. Downward ladder entry in `processInput` now nudges `playerPos.y -= 0.35f` to cleanly latch the character onto the rungs.
      5. Added support for `GLFW_KEY_DOWN` alongside `GLFW_KEY_S` for dropping through one-way tiles and descending ladders.

### [v0.5.12] - 2026-10-25
#### Session Archive: [`v4.12 - Free Exploration Mode and Manual Movement Toggle.md`](/doc/prompts/v4.12%20-%20Free%20Exploration%20Mode%20and%20Manual%20Movement%20Toggle.md)
* **Mechanics & Player Controls**:
  - **Free Exploration Mode (`F` Key)**:
    - *Motivation*: The default game mode operates as a continuous high-speed endless runner where the Samurai sprints forward autonomously. Players exploring intricate platforms, cavern shafts, or avoiding obstacles needed full, unhurried manual control.
    - *Velocity Decoupling*: When Free Exploration Mode is active (`freeExplorationMode = true`), continuous autonomous velocity injection is suppressed. The horizontal velocity $v_x$ is strictly driven by whether `A` (run left, $-v_{run}$) or `D` (run right, $+v_{run}$) is held. Releasing both keys immediately zeros horizontal velocity ($v_x = 0$), bringing the character to an immediate, stable halt.
    - *Ground Idle State Resolution*: In Free Exploration, whenever $v_x = 0$ on solid ground, the player state machine resolves to `ANIM_IDLE`, playing Samurai Mack's breathing animation. Pressing `A` or `D` flips the sprite orientation and transitions smoothly to `ANIM_RUN`.
    - *Ladder Climb Hold*: On vertical ladder shafts in Free Exploration Mode, vertical climbing velocity $v_y$ is clamped to 0 when neither `W` nor `S` is held, allowing players to pause on ladders and observe moving hazards.
    - *UI & Feedback*: Displays floating popup `FREE EXPLORATION: ON / OFF`, adds on-screen HUD badge in top-left, updates window title, and adds status item `[F] MODE: FREE EXPLORATION` in the ESC Pause Menu.

### [v0.5.11] - 2026-10-25
#### Session Archive: [`v4.11 - Fix Standalone Missing DLL and Video Rendering Alpha Discard.md`](/doc/prompts/v4.11%20-%20Fix%20Standalone%20Missing%20DLL%20and%20Video%20Rendering%20Alpha%20Discard.md)
* **Resolved Issues & Edge Cases**:
  - **`msys-stdc++-6.dll` Missing System Error**:
    - *Root Cause*: The default compiler configuration dynamically linked against MSYS2's `libstdc++` and `libgcc`, causing runtime failures on machines without an MSYS environment in PATH.
    - *Resolution*: Updated [`Makefile`](/Makefile) to compile with `-static-libgcc -static-libstdc++`. This bakes the C++ runtime directly into `build/main.exe`. The `package` target was further updated to automatically bundle `msys-2.0.dll` alongside `glfw3.dll` and `assets/`, guaranteeing 100% portable execution on any Windows machine.
  - **Invisible Intro Video (Audio Only) Alpha Discard Bug**:
    - *Root Cause*: Windows Media Foundation decodes `MFVideoFormat_RGB32` as 32-bit $XRGB$ frames where the reserved byte (Alpha) is strictly zero (`0x00`). The fragment shader in [`src/Renderer.cpp`](/src/Renderer.cpp) previously executed `if (texColor.a < 0.12) discard;` for all textured quads, causing the GPU to discard every single pixel of the video frame.
    - *Resolution*:
      1. Updated the fragment shader to introduce `uniform bool uDiscardAlpha;`. Enabled alpha-discard for sprites and transparent tiles while explicitly disabling it (`discardAlpha = false`) for opaque video playback.
      2. Configured the video texture with `glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE)` and `GL_RGB` internal format in [`src/VideoPlayer.cpp`](/src/VideoPlayer.cpp), forcing texture alpha fetches to `1.0`.
      3. Added proper initial `glViewport` initialization based on active window framebuffer dimensions in [`src/main.cpp`](/src/main.cpp).

### [v0.5.10] - 2026-10-25
#### Session Archive: [`v4.10 - Standalone Single-File Game Packaging Analysis.md`](/doc/prompts/v4.10%20-%20Standalone%20Single-File%20Game%20Packaging%20Analysis.md)
* **Packaging & Distribution Engineering**:
  - **Standalone Feasibility & Architecture**: Comprehensively analyzed all three technical methods for packaging *The Wenderer* into a standalone file:
    1. **Virtual Container / Single-File SFX**: Packaging `main.exe`, `glfw3.dll`, and `assets/` into a single self-contained `.exe` container with zero user extraction required.
    2. **In-Binary Resource Embedding**: Compiling all assets directly into `.rsrc` / `.rodata` PE sections with memory-based loaders (`stbi_load_from_memory`, `IMFByteStream`).
    3. **Self-Contained Portable Archive / Installer**: Industry-standard distribution zip containing all runtime files and assets ready for distribution on platforms like itch.io and Steam.
  - **Automated `package` Build Target**: Added `package` target to [`Makefile`](/Makefile) that assembles a clean, standalone distribution archive (`TheWenderer_Standalone.zip`, ~5.8 MB) containing the executable, dynamic libraries, and complete asset suite with zero local build toolchain requirements for end users.
  - **Runtime Dependency Verification**: Inspected PE import tables via `objdump`, confirming standard Windows UCRT, Media Foundation (`MFPlat`, `MFReadWrite`), WinMM, and GDI32 system linkage across Windows 10 and 11 environments.

### [v0.5.9] - 2026-10-25
#### Session Archive: [`v4.9 - In-Engine Intro Video Playback and Asset Embedding Analysis.md`](/doc/prompts/v4.9%20-%20In-Engine%20Intro%20Video%20Playback%20and%20Asset%20Embedding%20Analysis.md)
* **Architectural Additions**:
  - **In-Engine Video Subsystem (`VideoPlayer`)**: Created [`include/VideoPlayer.h`](/include/VideoPlayer.h) and [`src/VideoPlayer.cpp`](/src/VideoPlayer.cpp) utilizing native Windows Media Foundation (`IMFSourceReader`) with zero external binary dependencies. Streams and decodes H.264 video frames directly to an OpenGL 2D texture (`GL_BGRA` / `glTexSubImage2D`) at native 1080p resolution while simultaneously decoding AAC audio streams into in-memory PCM Wave audio played asynchronously via `PlaySoundA`.
  - **Game State Transition**: Introduced `STATE_INTRO_VIDEO = 0` to `GameState` in [`include/Common.h`](/include/Common.h). The engine initializes into this state before the platformer begins.
  - **Cinematic Presentation & Responsive Letterboxing**: Automatically computes aspect ratios (`16:9`) to letterbox the video inside any window size, rendering an animated bottom progress bar and pulsing skip prompt text (`[SPACE / ESC / ENTER] SKIP INTRO`).
  - **Skip & End-of-Stream Handling**: Spacebar, Enter, or Escape gracefully aborts video and audio playback, closes decoder handles, and places the player safely into the starting idle runner state (`STATE_RUNNING`, `isGameIdleAtStart = true`).
* **Technical Embedding & Size Optimization Analysis**:
  - Evaluated embedding `introVid.mp4` directly into `.exe` via Windows Resource Compiler (`windres` / `.rc`) vs. compiling as a byte array vs. streaming from disk.
  - Explained mathematical reasons why embedding an already-compressed H.264 MP4 file into executable binary files does **not** reduce its file size, and detailed real size-reduction strategies (transcoding to 720p H.265/AV1 or lowered bitrates).

### [v0.5.8] - 2026-10-25
#### Session Archive: [`v4.8 - Sky-Fall Down Scene Left and Right Direction Relics.md`](/doc/prompts/v4.8%20-%20Sky-Fall%20Down%20Scene%20Left%20and%20Right%20Direction%20Relics.md)
* **Resolved Issues & Edge Cases**:
  - **Sky-Fall Direction Relic Scarcity & Missed Pickups**: In [`src/main.cpp`](/src/main.cpp), diving at $9.2\,\text{m/s}$ caused single small direction relics at random coordinates to be easily bypassed in $< 0.5\,\text{s}$. Pre-generated items below the player were previously only coins and fruit shields, leaving a dead zone upon 15s trial completion.
* **Added & Improved**:
  - **Retroactive Item Conversion Ahead of Descent**: Upon reaching 15 seconds (`scenePlayTime >= SCENE_DIRECTION_COOLDOWN`), all uncollected items within 35m below the player are immediately transformed into Left (`DIR_LEFT`, Cyan) and Right (`DIR_RIGHT`, Gold) Direction Arrow Relics.
  - **Staggered Multi-Gate Descent Corridor**: Injected three consecutive multi-lane exit gates directly in the descent corridor at $y - 5.0\,\text{m}$, $y - 10.0\,\text{m}$, and $y - 15.0\,\text{m}$ featuring Left, Center, and Right direction arrowheads.
  - **Guaranteed Continuous Procedural Relics**: Procedural skyfall generation now guarantees pairs of Left and Right Direction Arrow Relics on every single descent tier once 15s has passed.
  - **Expanded Aerial Pickup Radius**: Increased collision volume for direction relics in Sky-Fall to $1.85\,\text{m}$ horizontal and $2.1\,\text{m}$ vertical, ensuring natural, effortless collection while steering.
  - **Enlarged Pulsing Relic Sprites**: Scaled direction relics up to $1.65\,\text{m}$ ($1.65 + 0.20\sin(5t)$) with vibrant glowing cyan/gold arrowhead tints for maximum readability against the falling sky backdrop.
  - **Seamless Platform Runner Resumption**: Touching any Left or Right arrow relic immediately calls `returnToPlatformerScene(true)`, snapping the player flush to solid ground via vertical raycast and resuming the horizontal runner in the indicated direction.

### [v0.5.7] - 2026-10-25
#### Session Archive: [`v4.7 - Ladder Scene Direction Trigger Relics & Exit Spawning.md`](/doc/prompts/v4.7%20-%20Ladder%20Scene%20Direction%20Trigger%20Relics%20&%20Exit%20Spawning.md)
* **Resolved Issues & Edge Cases**:
  - **Missing Left/Right Direction Objects on Ladders**: In [`src/main.cpp`](/src/main.cpp), `SCENE_LADDER_CLIMB` previously only spawned Gold Coins (`type = 0`) and Fruit Shields (`type = 1`), leaving players with no direction relic trigger objects to exit back to the platformer runner without dying to traps.
  - **Ladder Generation Boundary Sync**: Fixed `enterLadderScene()` which pre-generated up to 85m while leaving `ladderGenTopY = 8.0f`. Initial tier generation is now tightly bounded ($y \le 26.0\text{m}$) and synchronized with runtime generation.
* **Added & Improved**:
  - **Immediate Post-15s Direction Relic Injection**: Upon completing the 15-second survival trial (`scenePlayTime >= SCENE_DIRECTION_COOLDOWN`), the engine immediately spawns `DIR_LEFT` and `DIR_RIGHT` glowing arrowhead relics directly ahead of the climber on Lanes 0, 2, and 1.
  - **Procedural Ladder Relic Streaming**: Runtime ladder generation regularly spawns Direction Arrow Relics (40% probability) once the 15s trial completes, alternating between Left and Right arrowheads.
  - **Touch & Warp Scene Return**: Touching a Direction Arrow Relic on the ladder plays `SFX_RELIC`, triggers floating warp text (`"DIRECTION TRIGGERED!\nWARP <<< LEFT"` or `"WARP >>> RIGHT"`), and calls `returnToPlatformerScene(true)` with the selected horizontal direction.
  - **Pulsing Neon Relic Rendering**: Rendered ladder direction relics with `directionArrowsTexture`, applying pulsating scale ($1.35 + 0.15\sin(5t)$) and vibrant neon cyan/gold tints.
  - **Sky-Fall Parity & Leap Direction Alignment**: Extended identical post-15s direction relic spawning to `SCENE_SKY_FALL` and ensured outer-lane leaps off Lane 0 and Lane 2 explicitly orient `platformerSavedDir` to Left and Right.

### [v0.5.6] - 2026-10-25
#### Session Archive: [`v4.6 - Left Chunk Persistence, Direction Arrows, Shield Fruits, Idle Start, Single Hero, and Down Scene Pacing.md`](/doc/prompts/v4.6%20-%20Left%20Chunk%20Persistence%2C%20Direction%20Arrows%2C%20Shield%20Fruits%2C%20Idle%20Start%2C%20Single%20Hero%2C%20and%20Down%20Scene%20Pacing.md)
* **Resolved Issues & Edge Cases**:
  - **Left Chunk Persistence & Unbounded Deletion Fix**: Fixed chunk eviction in [`src/World.cpp`](/src/World.cpp) where `leftGenEdge` was previously unbounded and failed to track culled chunks. Clamped `leftGenEdge = std::max(leftGenEdge, playerX - 80.0f)` and `rightGenEdge = std::min(rightGenEdge, playerX + 80.0f)`.
  - **Zero Airborne Floating on Scene Return**: Implemented `World::ensureGroundAround(targetX, theme)` and dynamic ground surface scanning upon returning from Up (Ladder) or Down (Sky-Fall) mini-games. If returning over a solid platform, player snaps flush to ground; if over an open chasm, player retains downward velocity with active gravity, preventing any mid-air floating.
* **Added & Improved**:
  - **4-Direction Arrowhead Objects** ([`assets/direction_arrows.png`](/assets/direction_arrows.png)): Generated a dedicated $256 \times 64$ sprite sheet containing glowing neon arrowheads for Left, Right, Up, and Down on translucent backing discs via [`build/gen_arrows.py`](/build/gen_arrows.py). Direction relics explicitly indicate and trigger that exact direction.
  - **10-Second Temporary Shield from Fruit Pickups**: Fruit pickups (`apple`, `orange`, `banana`, `cherry`) now grant a 10s invulnerability shield (`shieldTimer = 10.0f`) with a glowing cyan energy bubble aura around Samurai Mack. Hazards (spikes, sawblades, raptor birds, aerial mines) are safely absorbed and deflected across all scenes.
  - **Idle Animation Start**: Character begins standing still in idle animation at game launch or restart (`playerVel = (0, 0)`, `currentAnim = ANIM_IDLE`, `isGameIdleAtStart = true`). A pulsing on-screen hint banner guides players: `"PRESS [A] OR [D] TO RUN | [SPACE] TO JUMP"`.
  - **Single Hero Focus (Ninja Code Removal)**: Completely removed Ninja animation assets, textures, switching routines, and Pause Menu entries, streamlining the codebase and asset footprint around Samurai Mack.
  - **Sky-Fall Scene Obstacle Deconfliction**: Increased vertical tier spacing from 5.0m to 14.0m, reduced bird spawn rate to 50% on alternate tiers, lowered spike mine frequency to 33%, and calibrated collision boxes from 1.15m to 0.85m for fair, readable evasive diving.

### [v0.5.5] - 2026-10-25
#### Session Archive: [`v4.5 - Dynamic Day-Night Cycle, Thematic Background Sceneries, and Scene Cooldown.md`](/doc/prompts/v4.5%20-%20Dynamic%20Day-Night%20Cycle%2C%20Thematic%20Background%20Sceneries%2C%20and%20Scene%20Cooldown.md)
* **Added & Improved**:
  - **Four Thematic Background Scenery Assets**:
    - **Silvergrass Field** ([`assets/scenery_silvergrass.png`](/assets/scenery_silvergrass.png)): Shimmering Japanese Susuki grass / pampas plumes swaying along rolling hills.
    - **Autumn Foliage** ([`assets/scenery_autumn.png`](/assets/scenery_autumn.png)): Blazing crimson, amber, and golden maple/ginkgo crowns with mountain silhouettes and swirling leaves.
    - **Green Forest Foliage** ([`assets/scenery_forest.png`](/assets/scenery_forest.png)): Dense evergreen pine firs and lush woodland oaks with layered viridian depth.
    - **Urban Area** ([`assets/scenery_urban.png`](/assets/scenery_urban.png)): Metropolitan high-rise skyline with multi-tier towers, spires, and lit windows that glow intensely at night and blend sleekly into glass at day.
    - Generated procedurally using a standalone script ([`build/gen_scenery.py`](/build/gen_scenery.py)) as seamless $512 \times 256$ looping parallax layers.
    - Integrated instant scenery theme cycling via **`[B]`** key and Pause Menu.
  - **Dynamic Day / Night Lighting Cycle (1.5 Min Cycle, Starts at Night)**:
    - Automatically alternates between **Night** and **Day** every 1.5 minutes (90.0 seconds).
    - Initial world state begins in **Night time** (`dayNightTimer = 0.0s`, `isNight = true`).
    - Smooth 15-second twilight blending curve ($75.0\text{s} \to 90.0\text{s}$) modeling sunrise dawn and sunset dusk transitions.
    - Dynamic sky gradient shifting between deep midnight indigo (`0.035, 0.05, 0.12`), twilight rose-gold (`0.38, 0.16, 0.20`), and azure day (`0.24, 0.52, 0.85`).
    - 64 twinkling celestial stars and a glowing silver moon with crater halo rendered at night ($L < 0.75$).
    - Radiant golden sun with solar corona rendered during the day ($L > 0.25$).
    - Added **`[N]`** key to fast-forward Day/Night phases for instant previewing.
  - **15-Second Mini-Game Scene Trial Cooldown**:
    - When entering the 3-Ladder Ascent (`UP`) or Sky-Fall Dive (`DOWN`) scenes, a 15-second survival trial activates.
    - During the first 15 seconds (`scenePlayTime < 15.0s`):
      - Autonomous direction timer shifts are paused.
      - Mystery Fruit Relics award +100 bonus score and display remaining trial time (`"RELIC +100! (Warp in Xs)"`).
      - Scene exit key triggers (`S`/Down or outer-ladder leap in ladder scene; `W`/Up or Space in sky fall) are guarded, prompting `"SURVIVE 15s FIRST! (Xs left)"`.
    - Upon completing the 15 seconds, a floating alert `"=== DIRECTION SHIFTS UNLOCKED! ==="` and corner HUD notification announce that directional transitions and warps are now fully active.

---

### [v0.5.4] - 2026-10-25
#### Session Archive: [`v4.4 - Dash Mechanic, Left Starting Platforms, and Dynamic Up-Down Mini-Game Scenes.md`](/doc/prompts/v4.4%20-%20Dash%20Mechanic%2C%20Left%20Starting%20Platforms%2C%20and%20Dynamic%20Up-Down%20Mini-Game%20Scenes.md)
* **Added & Improved**:
  - **High-Velocity Dash Mechanic (`SHIFT` Key)**:
    - Implemented a burst dash ($15.0\,\text{m/s}$ for $0.22\,\text{s}$ with a $0.65\,\text{s}$ cooldown).
    - Features motion ghosting/afterimage visual trail rendering with cyan/gold ethereal alpha fade.
    - Provides spike hazard invulnerability / phasing during active dash burst frames.
  - **Curated Left-Side Starting Platforms**:
    - Replaced empty initial spawn left area with curated multi-elevation starting structures across $x \in [-24, -2]$:
      - Low platform ($y = 1.8$), Mid platform ($y = 3.0$) with ladder & Mystery Relic, and Sky Canopy platform ($y = 5.2$) with high ladder & Mystery Relic.
      - Underground cavern drop shaft at $x = -12.0$ with ladder descent, plus golden coin collections.
  - **3-Parallel Endless Ladder Climbing Scene (`SCENE_LADDER_CLIMB`)**:
    - Triggered by UP direction change (from Mystery Fruit Relic warp or autonomous switch).
    - Player climbs continuously upward across 3 parallel ladder tracks (Left, Center, Right).
    - `A`/`Left` and `D`/`Right` keys perform horizontal leaps between adjacent ladders.
    - Spawns ascending moving blade/spike traps across random lanes.
    - Trap collision triggers realistic falling physics, screen darkening, and a reverse `Death.png` resurrection animation (frames 5 down to 0) before returning to the regular running platform.
    - Outer edge left/right triggers return the player to the regular platform with a jump arc.
    - `S`/`Down` key initiates a dynamic transition directly into the Sky-Fall diving scene.
  - **Sky-Fall Aerial Diving Scene (`SCENE_SKY_FALL`)**:
    - Triggered by DOWN direction change.
    - Player enters free-fall descent with aerial left/right steering and dynamic camera framing.
    - Features newly generated pixel-art animated bird obstacles (`assets/bird.png`, 4-frame animated raptor sprite sheet) and floating sky spikes.
    - Collision plays `Take Hit.png` rotated $90^\circ$ counter-clockwise (`glm::radians(90.0f)`), followed by screen fade and return to the regular running platform.
    - `W`/`Up` key transitions directly into the Ladder Climbing scene, and `SPACE` allows recovery back to the platform runner.

---

### [v0.5.3] - 2026-10-25
#### Session Archive: [`v4.3 - Downward Ladder Entry and Procedural World Generation Analysis.md`](/doc/prompts/v4.3%20-%20Downward%20Ladder%20Entry%20and%20Procedural%20World%20Generation%20Analysis.md)
* **Added & Improved**:
  - **Downward Ladder Shaft Entry via `S`/`Down` Key**:
    - Players can now press `S` or `Down Arrow` while standing near the top of a ladder shaft to latch onto the ladder and descend into underground caverns.
    - Added `GLFW_KEY_S` and `GLFW_KEY_DOWN` to the proactive ladder latch detection condition in the physics update loop.
    - Added a dedicated "Downward Ladder Entry" input handler in `processInput()` that detects grounded players pressing `S`/`Down` near shaft tops and initiates downward climbing at `CLIMB_SPEED`.
  - **Bidirectional Procedural World Generation Analysis**:
    - Confirmed that the world generates procedurally in **both** directions (leftward and rightward) using the identical `generateChunk()` function.
    - Leftward movement IS truly endless; new 35-unit chunks are generated on-demand as the player approaches the left edge.
    - Explained why the initial area left of spawn appears sparse (the `isInitial` flag skips elevated features).
    - Provided technical comparison: procedural generation (~170 units in memory at any time) is far more memory-efficient than static pre-generation for infinite worlds.

---

### [v0.5.2] - 2026-10-25
#### Session Archive: [`v4.2 - Floating Text for Fruit Pickups and Auto-Shift Disabling.md`](/doc/prompts/v4.2%20-%20Floating%20Text%20for%20Fruit%20Pickups%20and%20Auto-Shift%20Disabling.md)
* **Added & Improved**:
  - **World-Space Floating Text Engine**:
    - Extended `Renderer::drawText` to support dynamic camera transformations via an optional `const glm::mat4& view` parameter.
    - Integrated floating popup text that spawns at world coordinates $(X, Y)$ and follows camera scrolling smoothly.
  - **Explicit Direction Warp Floating Text**:
    - Mystery Fruit Relic pickups now spawn vibrant floating text above the fruit: `RELIC COLLECTED!\nWARP >>> RIGHT` (or `<<< LEFT`, `^^^ ASCEND`, `vvv DESCEND`).
    - Gold Coin pickups spawn floating golden `+50` text with smooth rise and alpha fade.
  - **Resolved Perceived "Invisible Pickups" Bug**:
    - Discovered and addressed autonomous background timer (`directionSwitchTimer`) that triggered direction shifts and fruit corner notifications every $6.5\,\text{s}$ even when no fruit was on screen.
    - Disabled autonomous timer shifts by default (`autoShiftEnabled = false`), ensuring direction shifts happen **exclusively upon collecting visible Mystery Fruit Relics**.
    - Added an optional autonomous shift toggle (`T` key) and a status indicator in the `ESC` Pause Menu.

---

### [v0.5.1] - 2026-10-25
#### Session Archive: [`v4.1 - Slow Down Running and Climbing Animation Framerate.md`](/doc/prompts/v4.1%20-%20Slow%20Down%20Running%20and%20Climbing%20Animation%20Framerate.md)
* **Bug Fixes & Animation Calibrations**:
  - **Eliminated Double-Multiplication Framerate Glitch**: Fixed an issue where `animTimer` was multiplied by $10.0$ in the update loop and then multiplied by $12.0$ in the frame index selector, causing the running animation to play at a glitchy $120\,\text{FPS}$ ($15$ full run cycles per second).
  - **Normalized Timer Accumulation**: `animTimer` now accumulates pure elapsed real-world seconds (`animTimer += deltaTime`).
  - **Calibrated Running Speed**: Running animation now cycles at $8.0\,\text{FPS}$ ($125\,\text{ms}$ per frame), making leg strides, flowing clothes, and katana movement clear and readable.
  - **Calibrated Ladder Climbing Speed**: Climbing animation now cycles at $3.5\,\text{FPS}$ ($285\,\text{ms}$ per step), producing deliberate, tactile vertical hand-over-hand climbing.
  - **Calibrated Idle Breathing Speed**: Combat breathing stance cycles at $4.0\,\text{FPS}$.

---

### [v0.5.0] - 2026-10-24
#### Session Archive: [`v4.0 - Samurai Ninja Player Animations Pause Menu and Score Counter.md`](/doc/prompts/v4.0%20-%20Samurai%20Ninja%20Player%20Animations%20Pause%20Menu%20and%20Score%20Counter.md)
* **Added**:
  - **Samurai Mack & Shinobi Kenji Character Integration**:
    - Hand-crafted 2D pixel-art character animation sheets with high-resolution frames for **Samurai Mack** and **Shinobi Kenji**.
    - Full movement animation states:
      - Running forward (8 frames at 12 FPS)
      - Horizontally mirrored running backward (UV flipped with leading blade)
      - Jump rise (Frame 0/1 of Jump)
      - Downward fall (Frame 0/1 of Fall)
      - Slide dash (thrusting blade posture aligned along steep slopes)
      - Ladder climbing (alternating vertical strides)
      - Breathing stance idle (6 FPS)
      - Dramatic ground defeat death animation (cycling to final defeated pose)
    - Live hero toggle (`C` key) to switch between Samurai and Ninja in real time with instant HUD notifications.
  - **Interactive Pause Menu System (`ESC` Key)**:
    - Pressing `ESC` halts game physics ($dt = 0$) and opens a glassmorphism modal dialog.
    - Live statistics readout: current total score, traveled meters, coins collected, relics triggered, and active character.
    - Full menu interactions: `[ESC/SPACE]` Resume, `[C]` Toggle Hero, `[R]` Restart Run, `[Q]` Quit to Desktop, and controls reference guide.
  - **On-Screen Real-Time Score Counter HUD**:
    - Translucent glass card located at top-right corner with glowing golden accent border and spinning gold coin icon.
    - Dynamic tally formula: $\text{Total Score} = \text{Distance (m)} \times 10 + \text{Coins} \times 50 + \text{Relics} \times 100$.
    - Real-time updates displaying total points, coin count, and distance traveled.
  - **In-Memory 8x8 Pixel Bitmap Font Renderer**:
    - Embedded zero-dependency ASCII bitmap font (`font8x8_basic.h`) uploaded into an OpenGL texture during `Renderer::init()`.
    - Implemented `Renderer::drawText()` supporting arbitrary text string rendering, variable glyph scaling, and RGBA color tinting.

---

### [v0.4.3] - 2026-10-24
#### Session Archive: [`v3.6 - Technical Documentation and Academic Guide Creation.md`](/doc/prompts/v3.6%20-%20Technical%20Documentation%20and%20Academic%20Guide%20Creation.md)
* **Added**:
  - Created [`/doc/documentation.md`](/doc/documentation.md): Comprehensive technical progress report covering architecture, subsystem design, graphics pipeline, physics tuning, and build specifications.
  - Created [`/doc/explain.md`](/doc/explain.md): Accessible academic guide stripping away technical jargon, using everyday analogies (*Rulebook, Musician, Artist, Architect, Director*) to explain each code module for project presentations and lab viva exams.
  - Updated [`/README.md`](/README.md) with quick links to both newly prepared documents.

---

### [v0.4.2] - 2026-10-24
#### Session Archive: [`v3.5 - Modular Code Refactoring Ladder Stopping and Cavern Chasm Fixes.md`](/doc/prompts/v3.5%20-%20Modular%20Code%20Refactoring%20Ladder%20Stopping%20and%20Cavern%20Chasm%20Fixes.md)
* **Architectural Refactoring**:
  - Decoupled the ~1,900-line monolithic `src/main.cpp` into 5 maintainable, focused modules:
    - [`/include/Common.h`](/include/Common.h): Central shared constants, calibrated physics parameters, and data models.
    - [`/include/Audio.h`](/include/Audio.h) & [`/src/Audio.cpp`](/src/Audio.cpp): In-memory WinMM synthesized procedural sound engine.
    - [`/include/Renderer.h`](/include/Renderer.h) & [`/src/Renderer.cpp`](/src/Renderer.cpp): Modern OpenGL 3.3 Core shaders, quad VAO/VBO buffers, and texture rendering pipeline.
    - [`/include/World.h`](/include/World.h) & [`/src/World.cpp`](/src/World.cpp): Multi-elevation generation, chasm carving, and AABB collision resolution.
    - [`/src/main.cpp`](/src/main.cpp): Streamlined game loop, player physics state machine, and HUD rendering (~450 lines).
* **Resolved Issues & Gameplay Enhancements**:
  - **Ladder Stopping & Auto-Latching**: Fixed bug where the player ran past ladders during upward movement; approaching a shaft in `DIR_UP` state now instantly halts horizontal velocity ($v_x = 0$) and latches onto the ladder shaft to ascend cleanly.
  - **Unblocked Cavern Chasms**: Fixed bug where surface ground and platforms blocked steep slopes into the underground cavern; planned slope spans are now carved as open chasms.
  - **Balanced Gameplay Speed**: Re-calibrated movement speeds to comfortable platformer pacing: run speed $7.2 \rightarrow 4.8\,\text{m/s}$, gravity $-32.0 \rightarrow -22.0\,\text{m/s}^2$, jump burst $13.8 \rightarrow 11.2\,\text{m/s}$, climb speed $5.6 \rightarrow 3.8\,\text{m/s}$, and slide speed $12.0 \rightarrow 7.8\,\text{m/s}$.

---

### [v0.4.1] - 2026-10-24
#### Session Archive: [`v3.4 - Multi-Elevation Worlds Caverns Sky Tier and Midpoint Interrupts.md`](/doc/prompts/v3.4%20-%20Multi-Elevation%20Worlds%20Caverns%20Sky%20Tier%20and%20Midpoint%20Interrupts.md)
* **Added**:
  - **Horizontal Direction Memory (`lastHorizontalDir`)**: Ascending or descending ladder shafts now remembers the prior horizontal run direction (`DIR_RIGHT` or `DIR_LEFT`), resuming that direction upon summiting instead of forcing an arbitrary rightward turn.
  - **Mid-Slope Jumping & Traversal Interrupts**: Mid-slope sliding can now be interrupted at any point by jumping (`Space`/`W`/Up) or steering (`A`/`D`), giving full control over downward momentum.
  - **Underground Cavern Zone ($Y = -4.5\,\text{m}$)**: Added subterranean cavern corridors entered via downward dive slopes and surface chasms, featuring multiple upward ladder shafts leading back to intermediate ledges and the surface.
  - **Super-Elevated Sky Canopy Tier ($Y = +5.5\,\text{m}$)**: High-altitude sky platforms accessed via extended ladder shafts, concluding with grand downward return slopes leading back to the surface.
  - **Locked Vertical Ladder Climbing**: Sidewise drift is strictly locked to zero while climbing ladders unless the player explicitly triggers a mid-ladder jump or dismount.
  - **Bedrock Foundation Lowering**: Bedrock lowered to $Y = -7.5\,\text{m}$ to catch players beneath all cavern pits, guaranteeing zero out-of-bounds fall deaths.

---

### [v0.4.0] - 2026-10-24
#### Session Archive: [`v3.2 - Tunnels Slopes Audio SFX Dynamic Endless Platforms and Corner HUD.md`](/doc/prompts/v3.2%20-%20Tunnels%20Slopes%20Audio%20SFX%20Dynamic%20Endless%20Platforms%20and%20Corner%20HUD.md)
* **Added**:
  - **Truly Endless Procedural Platforms**: Replaced fixed layer structures with a randomized multi-tier platform generator producing randomized elevations, lengths, and gaps.
  - **Vertical Ladder Tunnel Shafts**: Enclosed ladders within masonry-walled vertical shafts featuring shadowed interior backdrops and illuminated guide lines.
  - **Steep Downhill Slide Slopes**: Implemented angled ramp physics connecting higher tiers to lower levels; stepping onto a slope triggers downhill sliding, tilted sprite animation, and rushing wind SFX.
  - **In-Memory Procedural Audio Engine (`winmm`)**: Added zero-dependency synthesized PCM WAV audio for footsteps, jumps, ladder climb, slope slides, coin pickups, relic pickups, direction warp transitions, and spike death impacts.
  - **Comprehensible Corner HUD Notifications**: Added glassmorphism top-left notification cards with rotating item icons, category tags, titles, and explanations on every pickup.
  - **Dynamic Biomes & Transition Phase**: Added 4 distinct themes (Emerald Forest, Frozen Glacier, Sunset Canyon, Mystic Cavern) with cinematic slow-mo transition flashes and smooth environmental color interpolation.

---

### [v0.3.0] - 2026-10-24
#### Session Archive: [`v3.1 - Brackeys Assets Ladders Pitfalls Spikes and Direction Relics.md`](/doc/prompts/v3.1%20-%20Brackeys%20Assets%20Ladders%20Pitfalls%20Spikes%20and%20Direction%20Relics.md)
* **Added**:
  - **Brackeys Platformer Asset Integration**: Integrated pixel-art textures from the Brackeys platformer bundle: `world_tileset.png`, `platforms.png`, `coin.png`, and `fruit.png`.
  - **Mystery Direction Relics**: Floating fruit pickups that immediately trigger autonomous direction shifts (`DIR_RIGHT`, `DIR_LEFT`, `DIR_UP`, `DIR_DOWN`) with an on-screen alert banner.
  - **Vertical Traversal (Ladders & Pitfalls)**: Added climbable wooden ladders and intentional pitfall gaps with one-way drop-through platforms (`S` / Down).
  - **Abyss Immunity**: Replaced bottomless abyss deaths with an endless bedrock safety foundation with textured grass and dirt tiles.
  - **Lethal Spiky Traps**: Hazardous spikes on platforms and pit floors; walking into or landing on spikes triggers instant impalement and Game Over.
  - **Animated Gold Coins**: 12-frame rotating gold coins placed along platform runways (+50 pts).

---

### [v0.2.0] - 2026-10-24
#### Session Archive: [`v3.0 - Side-View Endless Runner Transition with Dynamic Direction and Sprites.md`](/doc/prompts/v3.0%20-%20Side-View%20Endless%20Runner%20Transition%20with%20Dynamic%20Direction%20and%20Sprites.md)
* **Major Perspective Shift**:
  - Converted the game from a top-down ribbon runner to a classic 2D side-view endless platformer.
  - Re-mapped physics coordinates: horizontal axis ($X$) represents travel progression, vertical axis ($Y$) represents elevation, jump physics, and downward gravity.
* **Added**:
  - Integrated open-source Liberated Pixel Cup (LPC) composite character spritesheet (`assets/player_sheet.png`) and `stb_image.h` PNG decoder.
  - Dynamic character animation states: Run Right (Row 37), Run Left (Row 35), Climb Up/Down (Row 30), Fall (Row 26 col 3), and Jump (Row 26 col 1).
  - Autonomous dynamic direction flips triggered by random countdown intervals (3.0s–6.0s) with visual alert banners.

---

### [v0.1.2] - 2026-10-21
#### Session Archive: [`v2.0 - Project Structure and Version Archiving Advice.md`](/doc/prompts/v2.0%20-%20Project%20Structure%20and%20Version%20Archiving%20Advice.md)
* **Documentation & Repository Structure**:
  - Evaluated repository structure and documented the distinction between monolithic gameplay source files and external project dependencies (`glad.c`, `include/`, `lib/`).
  - Formulated version snapshot strategies and best practices for archiving game builds before major milestone refactoring.

---

### [v0.1.1] - 2026-10-21
#### Session Archive: [`v1.2 - Convert File Paths to Root-Relative.md`](/doc/prompts/v1.2%20-%20Convert%20File%20Paths%20to%20Root-Relative.md)
* **Standards & Compliance**:
  - Converted all internal documentation file paths and markdown links across `README.md`, `AGENTS.md`, and prompt archives to repository root-relative paths (`/doc/...`).
  - Eliminated machine-specific absolute file URIs (`file:///...`) for portability across platforms and remote Git hosting.

---

### [v0.1.0-hotfix] - 2026-10-18
#### Session Archive: [`v1.1 - Fix Makefile for MSYS2 Sh Environment.md`](/doc/prompts/v1.1%20-%20Fix%20Makefile%20for%20MSYS2%20Sh%20Environment.md)
* **Build System Fix**:
  - Diagnosed and resolved MSYS2 `/bin/sh` syntax error when executing `make win` on Windows.
  - Removed shell-dependent batch conditions from [`/Makefile`](/Makefile).
  - Updated [`.gitignore`](/.gitignore) to track [`build/.gitkeep`](/build/.gitkeep) and [`build/glfw3.dll`](/build/glfw3.dll) while ignoring compiled binaries.

---

### [v0.1.0] - 2026-10-17
#### Session Archive: [`v1.0 - Project Analysis and Phase 1 Implementation.md`](/doc/prompts/v1.0%20-%20Project%20Analysis%20and%20Phase%201%20Implementation.md)
* **Initial Release (Phase 1 Complete)**:
  - Implemented the core engine prototype using Modern OpenGL 3.3 Core Profile in C++.
  - Created a single reusable $1 \times 1$ Quad VAO/VBO and MVP GLSL shader replacing hardcoded geometry.
  - Added delta-time game loop using GLFW time.
  - Implemented vector-adaptive movement with lateral lane shifting and jumping relative to the active movement axis.
  - Created procedural obstacle tracks with AABB collision detection and game reset states.
  - Implemented the prototype "Wending" mechanic: 90-degree orthogonal track pivot points with smooth rotational camera follow using `glm::mix`.
  - Created initial documentation [`/doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md), [`/Makefile`](/Makefile), [`/CMakeLists.txt`](/CMakeLists.txt), and [`/AGENTS.md`](/AGENTS.md).
