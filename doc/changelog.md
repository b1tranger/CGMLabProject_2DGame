# The Wenderer: Complete Project Changelog & Version History 📜

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Repository**: [CGMLabProject_2DGame](/)  
> **Archive Source Directory**: [`/doc/prompts/`](/doc/prompts/)  

All entries in this changelog map directly to session archives recorded in [`/doc/prompts/`](/doc/prompts/), documenting the evolutionary milestones of **The Wenderer** from initial prototype to a modular, multi-elevation endless platformer.

---

## Table of Versions

| Release Version | Session Version | Date | Core Milestone / Focus |
|---|---|---|---|
| **v0.6.16** | [v6.0](#v60---fix-build--run-commands-makefile-pipeline-and-cmake-specification) | 2026-10-26 | Build System & Quick Start Fix: Decoupled Build/Run Targets in Makefile, Restored CMakeLists.txt Modern Subsystems, Bundled msys-2.0.dll & Provided Robust Shell-Agnostic Commands |
| **v0.6.15** | [v5.15](#v515---boss-demon-slime-death-animation-calibration-to-match-updated-dying-roar-audio) | 2026-10-26 | Boss Demon Slime Death Animation Calibration: Slowed `05_d_death.gif` to 6.0 FPS (3.67s) & Extended Despawn Timer to 4.0s Synchronized with 3.96s `dyingRoar.mp3` |
| **v0.6.14** | [v5.14](#v514---slime-ground-placement--ai-unfreezing-and-boss-demon-slime-dying-roar-audio-integration) | 2026-10-26 | Slime Ground Placement & AI Unfreezing, and Boss Demon Slime Dying Roar Audio Integration (`dyingRoar.mp3` for `05_d_death.gif`) |
| **v0.6.13** | [v5.13](#v513---scale-down-player-defend-animation-to-match-idle-spritesheet-proportions) | 2026-10-25 | Player Defend Animation Proportional Rescaling: Downscaled `Defend_nobg.png` by $0.338\times$, Aligned $Y=121$ Footing & $X=94$ Centering to Match `Idle.png` Pixel-for-Pixel |
| **v0.6.12** | [v5.12](#v512---increase-slime-enemy-spawn-frequency-and-multi-elevation-density) | 2026-10-25 | Slime Enemy Spawn Frequency & Multi-Elevation Density: 2–4 Slimes per Chunk, Initial Starter Slimes, Cavern Spawning & Vertical-Aware Separation Buffers |
| **v0.6.11** | [v5.11](#v511---mid-animation-death-spell-damage-and-visual-telegraph-window) | 2026-10-25 | Mid-Animation Death Spell Damage Timing: 0.58s Visual Cloud Gathering Telegraph & Frames 7–10 Thunder Strike Ground Damage |
| **v0.6.10** | [v5.10](#v510---documentation-and-academic-guide-synchronization-with-modern-subsystems) | 2026-10-25 | Documentation & Academic Guide Synchronization: Modern OpenGL 2D Sprites Guide, 8 Modular Subsystems, Combat/Defend Mechanics & Viva Q&A |
| **v0.6.9** | [v5.9](#v59---synchronized-thunder-spell-sfx-and-right-click-katana-defend-mechanic) | 2026-10-25 | Synchronized Thunder Spell SFX (`thunder-after-crackle.mp3`), Katana Defend / Block Stance (`Defend_nobg.png`), 1.0s Guard Window & 2.0s Parry Cooldown |
| **v0.6.8** | [v5.8](#v58---delayed-multi-layer-thunder-spell-bgm-integration-free-exploration-default-and-enemy-separation) | 2026-10-25 | Delayed Multi-Layer Thunder Spell (`thunder-after-crackle.mp3`), BGM Integration (`Quarter_in_the_Slot.mp3` with Mute `M`), Free Exploration Default & Enemy Separation (12m) |
| **v0.6.7** | [v5.7](#v57---shift-control-remap-bod-centering--spell-attack-spike-clusters-and-boss-transformation-sfx) | 2026-10-25 | Shift Control Remap (`Y`), Tutorial Window Persistence, BOD Sprite Centering & Thunder Spell, Grouped Platform Spikes & Boss Sequenced Audio |
| **v0.6.6** | [v5.6](#v56---updated-slime-animations-and-boss-demon-slime-platform-jump-mechanics) | 2026-10-25 | Slime Animation Refresh & Boss Demon Slime Platform Jump Mechanics (`06_Smash.gif` Integration) |
| **v0.6.5** | [v5.5](#v55---lock-boss-attack-direction-and-directional-damage-sector) | 2026-10-25 | Directional Attack Commitment: Locked Boss Demon Slime & Bringer of Death Facing Direction & Frontal Damage Sector |
| **v0.6.4** | [v5.4](#v54---fix-bringer-of-death-facing-realistic-katana-slash-sfx-guaranteed-tutorial-window-and-idle-stillness-on-restart) | 2026-10-25 | Fixed Enemy Facing Direction (Inverted UV Flip), Metallic Katana Slash SFX, Guaranteed Post-Intro Tutorial Window & Idle Stillness on Restart |
| **v0.6.3** | [v5.3](#v53---calibrate-enemy-proportions-speeds-platform-grounding-and-telegraphed-attack-windups) | 2026-10-25 | Calibrated Enemy Sprite Proportions, Pacing Speeds, Platform Grounding & Telegraphed Attack Wind-Ups |
| **v0.6.2** | [v5.2](#v52---modular-decomposition-of-main-engine-into-uimanager-ladderscene-and-skyfallscene) | 2026-10-25 | Modular Architecture Decomposition: Subsystem Segregation of `UIManager`, `LadderScene`, and `SkyFallScene` |
| **v0.6.1** | [v5.1](#v51---standardize-standalone-executable-to-thewendererexe-and-eliminate-redundant-mainexe) | 2026-10-25 | Standalone Executable Deduplication: Standardized on `TheWenderer.exe` & Purged Redundant `main.exe` |
| **v0.6.0** | [v5.0](#v50---combat-system-3-lives-spikes-bringer-of-death-and-demon-slime-boss-battle) | 2026-10-25 | Real-Time Combat System (J/X/LMB), 3-Lives Health System & Heart Pickups, Procedural Spikes, Bringer of Death & Demon Slime Boss Battle |
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

### [v0.6.16] - 2026-10-26
#### Session Archive: [`v6.0 - Fix Build & Run Commands, Makefile Pipeline, and CMake Specification.md`](/doc/prompts/v6.0%20-%20Fix%20Build%20%26%20Run%20Commands,%20Makefile%20Pipeline,%20and%20CMake%20Specification.md)
* **Build System & Quick Start Command Diagnosis**:
  - Diagnosed command failure in [`README.md`](/README.md) where `mingw32-make` is not recognized on standard MSYS2/Windows environments where GNU make is named `make` (`make.exe`).
  - Resolved implicit execution side-effect where `make win` directly executed `./build/main.exe`, preventing pure compilation steps. Decoupled Makefile targets into dedicated `.PHONY: all build run win linux package clean`.
  - Added automatic directory validation and runtime DLL synchronization (`if (!(Test-Path 'build')) ...; Copy-Item lib/*.dll build/ -Force`) to the build recipe.
  - Synchronized [`CMakeLists.txt`](/CMakeLists.txt) by registering modern decoupled modules (`VideoPlayer.cpp`, `Audio.cpp`, `Renderer.cpp`, `World.cpp`, `UIManager.cpp`, `LadderScene.cpp`, `SkyFallScene.cpp`) and required media foundation linking flags (`-lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32`).
  - Bundled [`lib/msys-2.0.dll`](/lib/msys-2.0.dll) into the repository so stand-alone execution outside MSYS environment does not fail on missing POSIX runtime.
  - Documented explicit commands for PowerShell, Windows Command Prompt (`cmd.exe`), and Git Bash across [`README.md`](/README.md) and [`doc/documentation.md`](/doc/documentation.md).

### [v0.6.15] - 2026-10-26
#### Session Archive: [`v5.15 - Boss Demon Slime Death Animation Calibration to Match Updated Dying Roar Audio.md`](/doc/prompts/v5.15%20-%20Boss%20Demon%20Slime%20Death%20Animation%20Calibration%20to%20Match%20Updated%20Dying%20Roar%20Audio.md)
* **Boss Demon Slime Death Animation Timing Calibration (`05_d_death.gif` / `demon_death.png`)**:
  - **Acoustic Waveform Analysis**: Measured the newly updated [`assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) audio stream, determining an exact total duration of $3.958\,\text{seconds}$ ($44,100\,\text{Hz}$, 16-bit mono) with sustained acoustic reverberations persisting up to $4.0\,\text{seconds}$.
  - **Framerate Deceleration ($6.0\,\text{FPS}$)**: Decelerated the 22-frame death animation playback rate from $10.0\,\text{FPS}$ down to $6.0\,\text{FPS}$ (`eFrame = std::min(static_cast<int>(enemy.animTimer * 6.0f), 21)`). This expands the visual death sequence across $3.67\,\text{seconds}$, giving the boss's agony collapse and dissolution deliberate, dramatic weight.
  - **Despawn Timer Alignment**: Extended the active death state threshold from $2.4\,\text{seconds}$ to $4.0\,\text{seconds}$ (`enemy.animTimer >= 4.0f`), allowing the final dissipating smoke frames to linger smoothly and dissolve in unison with the closing tail of the audio roar.

### [v0.6.14] - 2026-10-26
#### Session Archive: [`v5.14 - Slime Ground Placement & AI Unfreezing, and Boss Demon Slime Dying Roar Audio Integration.md`](/doc/prompts/v5.14%20-%20Slime%20Ground%20Placement%20%26%20AI%20Unfreezing,%20and%20Boss%20Demon%20Slime%20Dying%20Roar%20Audio%20Integration.md)
* **Slime Ground Placement & AI Unfreezing**:
  - **Ground Alignment Fix**: Diagnosed and resolved issue where initial slimes were spawned with `spawnY = footY + 0.7f` instead of `spawnY = footY + 1.2f`. Because the slime render quad is $2.4\,\text{m}$ tall ($1.2\,\text{m}$ half-height) with pixels anchored at the bottom edge, a $0.7\,\text{m}$ offset placed the bottom of the slime at $Y = -0.5\,\text{m}$—sinking $0.5\,\text{m}$ (1 full tile layer) underground into the dirt.
  - **AI Movement Unfreezing**: Fixed collision raycasting deadlock where `footY` was evaluated at $0.7\,\text{m} - 1.2\,\text{m} = -0.5\,\text{m}$. Because surface platform tiles have tops at $Y = 0.0\,\text{m}$, the difference ($0.5\,\text{m}$) exceeded the $0.4\,\text{m}$ ground tolerance check (`hasSolidGroundAt`), causing the slime AI to believe it was perpetually falling into an abyss and freezing its movement in place every frame. With `spawnY = footY + 1.2f`, `footY` evaluates precisely to $0.0\,\text{m}$ (surface) and $-4.0\,\text{m}$ (caverns), restoring active hopping patrols.
  - **Free Roaming Ledge Turnaround**: Removed unconditional facing updates during patrol mode outside 8m, allowing patrolling slimes to turn around at platform edges without having their orientation immediately overridden towards the player on the subsequent frame.
* **Boss Demon Slime Dying Roar Audio Integration (`dyingRoar.mp3`)**:
  - Integrated authentic monster dying vocalization [`assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) into `AudioManager::playBossDyingRoar()`.
  - Triggered asynchronous playback via Windows MCI when the Boss Demon Slime reaches 0 HP, accompanying the full 22-frame death animation sequence (`05_d_death.gif` / `demon_death.png`) and smoke dissipation.

### [v0.6.13] - 2026-10-25
#### Session Archive: [`v5.13 - Scale Down Player Defend Animation to Match Idle Spritesheet Proportions.md`](/doc/prompts/v5.13%20-%20Scale%20Down%20Player%20Defend%20Animation%20to%20Match%20Idle%20Spritesheet%20Proportions.md)
* **Player Defend Animation Proportional Rescaling (`Defend_nobg.png`)**:
  - **Empirical Pixel Analysis**: Evaluated sprite coordinate bounding boxes across all character animations. In [`assets/samurai/Idle.png`](/assets/samurai/Idle.png), [`Run.png`](/assets/samurai/Run.png), and [`Attack1.png`](/assets/samurai/Attack1.png), Samurai Mack is drawn inside $200\times 200$ frames with an average body height of $51$–$52\,\text{px}$, centered horizontally at $X = 94\,\text{px}$ with feet grounded at $Y = 121\,\text{px}$. In contrast, [`Defend_nobg.png`](/assets/samurai/Defend_nobg.png) had the character drawn at $154$–$190\,\text{px}$ height with feet at $Y = 194\,\text{px}$, causing the player to balloon to $3.0\times$ normal size and sink below platforms when guarding.
  - **Super-Sampled Resampling ($0.3377\times$)**: Executed a precision 64-sample box area resampler across all 8 frames of [`Defend_nobg.png`](/assets/samurai/Defend_nobg.png), scaling down the character by scale factor $S = 52.0 / 154.0 \approx 0.33766$.
  - **Grounding & Centering Alignment**: Mapped source pivot $(X=98, Y=194)$ directly onto destination pivot $(X=94, Y=121)$. The resulting guard stance frames have an exact height of $51$–$53\,\text{px}$ (overhead raised blade reaches $65\,\text{px}$, matching [`Attack1.png`](/assets/samurai/Attack1.png) at $69\,\text{px}$), $X=93$–$96$ horizontal centering, and $Y=121$ foot alignment.
  - **Seamless In-Engine Compatibility**: Because the texture itself now adheres to the exact coordinate reference frame of all other Samurai Mack sprites, the uniform `playerRenderSize = (5.5f, 5.5f)` and vertical offset `playerPos.y - 0.10f` in [`src/main.cpp`](/src/main.cpp) render the defensive stance with perfect visual continuity and zero size jump.

### [v0.6.12] - 2026-10-25
#### Session Archive: [`v5.12 - Increase Slime Enemy Spawn Frequency and Multi-Elevation Density.md`](/doc/prompts/v5.12%20-%20Increase%20Slime%20Enemy%20Spawn%20Frequency%20and%20Multi-Elevation%20Density.md)
* **Increased Slime Spawn Frequency & Encounter Density**:
  - **Guaranteed Starter Slimes in Spawn Chunk**: Added two guaranteed initial slime enemies on the surface at $X = +17.5\,\text{m}$ (moving left) and $X = -18.0\,\text{m}$ (moving right) upon game initialization, providing immediate, engaging combat encounters without requiring players to trek into distant chunks.
  - **100% Chunk Spawning Rate**: Removed the prior $60\%$ probabilistic roll, guaranteeing that every newly generated $35\,\text{m}$ procedural world chunk spawns slimes.
  - **Increased Slime Pack Multiplicity**: Boosted procedural slime count per chunk from 1–2 to 2–4 slimes (`numSlimes = 2 + (std::rand() % 3)`), creating lively, dynamic combat encounters.
  - **Multi-Elevation Cavern Spawning**: Implemented a 33% probability for slimes to spawn in subterranean caverns (`CAVERN_FLOOR_Y + 0.5\,\text{m}`), transforming underground exploration into perilous, combat-rich zones alongside surface runs.
  - **Vertical-Aware Separation Buffers**: Refined the enemy distance deconfliction check: Bringer of Death and Slimes now require a $9.0\,\text{m}$ horizontal separation buffer only when sharing the same elevation tier ($\Delta y < 2.5\,\text{m}$). This allows surface Bringers of Death and cavern slimes to coexist within the same horizontal coordinates without mutually blocking spawns. Slime-to-slime spacing is tightened to $4.0\,\text{m}$, and candidate placement search attempts were doubled from 12 to 24 per slime.

### [v0.6.11] - 2026-10-25
#### Session Archive: [`v5.11 - Mid-Animation Death Spell Damage and Visual Telegraph Window.md`](/doc/prompts/v5.11%20-%20Mid-Animation%20Death%20Spell%20Damage%20and%20Visual%20Telegraph%20Window.md)
* **Mid-Animation Death Spell Damage Calibration**:
  - Diagnosed and resolved issue where Bringer of Death thundercloud spells dealt damage immediately upon initial frame manifestation ($t = 0.0\text{s}$), making hits unavoidable if the player was at the target location.
  - Aligned damage collision evaluation strictly to the cloud's mid-animation strike phase (frames 7–10 at 12 FPS, $t \in [0.58\text{s}, 0.85\text{s}]$), where the lightning bolt visually extends and crashes into the ground.
  - Granted a generous $0.58\text{s}$ visual telegraph window (frames 0–6) accompanied by a `"THUNDER CLOUD GATHERING! MOVE!"` floating text alert, providing ample reaction time for players to sprint, dash, jump away, or raise their Katana Defend guard (`RMB`/`K`) to parry the strike.

### [v0.6.10] - 2026-10-25
#### Session Archive: [`v5.10 - Documentation and Academic Guide Synchronization with Modern Subsystems.md`](/doc/prompts/v5.10%20-%20Documentation%20and%20Academic%20Guide%20Synchronization%20with%20Modern%20Subsystems.md)
* **Academic Guide Enhancement (`/doc/explain.md`)**:
  - Authored a dedicated technical chapter on **"How 2D Sprites Work in Modern OpenGL"**:
    - Explains single-quad GPU VBO/VAO instancing, normalized UV coordinate space ($U \in [0, 1], V \in [0, 1]$), spritesheet frame slicing math ($u_0 = i/N, u_1 = (i+1)/N$), horizontal UV mirroring (`flipHorizontal`), fragment shader alpha discard (`if (texColor.a < 0.1) discard;`), and orthographic MVP matrix projection.
  - Synchronized architectural documentation to present all 8 modular engine subsystems (`main.cpp`, `Common.h`, `Renderer.cpp`, `World.cpp`, `Audio.cpp`, `UIManager.cpp`, `LadderScene.cpp`, `SkyFallScene.cpp`, `VideoPlayer.cpp`).
  - Added full explanations of combat mechanics: Katana slash active frames, Right-Click Defend stance ($1.0\text{s}$ duration, metallic parry SFX, $2.0\text{s}$ cooldown), enemy AI state machines, and expanded Viva Q&A with 6 core evaluation questions.
* **System Documentation Overhaul (`/doc/documentation.md`)**:
  - Completely updated technical documentation to capture the latest engine state, calibrated physics, hybrid audio architecture (procedural synthesis + Windows MCI hardware-accelerated streaming), circular shield fragment discard mathematics, and Media Foundation video player linking.
* **Quick Start Build Sync (`/README.md`)**:
  - Updated direct `g++` compilation command to link all modular sources (`UIManager.cpp`, `LadderScene.cpp`, `SkyFallScene.cpp`).

### [v0.6.9] - 2026-10-25
#### Session Archive: [`v5.9 - Synchronized Thunder Spell SFX and Right-Click Katana Defend Mechanic.md`](/doc/prompts/v5.9%20-%20Synchronized%20Thunder%20Spell%20SFX%20and%20Right-Click%20Katana%20Defend%20Mechanic.md)
* **Synchronized Bringer of Death Spellcast SFX**:
  - Re-anchored `audioMgr.playThunderSpell()` (`assets/thunder-after-crackle.mp3`) to trigger immediately upon the Bringer of Death initiating its spellcast attack animation (`ENEMY_STATE_CAST`).
  - By matching the wraith's cast wind-up duration ($9\text{ frames} / 7.5\text{ fps} \approx 1.2\text{s}$) with the audio's 1.0s initial electrical crackle build-up, the explosive lightning thunderclap impacts precisely as the ground thunder cloud animation spawns at the player's ground coordinate.
* **Player Defend / Parry Stance (`assets/samurai/Defend_nobg.png`)**:
  - Loaded full 8-frame defensive guard animation using `assets/samurai/Defend_nobg.png` ($1600\times 200$, $200\times 200$ px/frame) triggered via **Right Mouse Click** (`GLFW_MOUSE_BUTTON_RIGHT`) or `K` key.
  - While defending, horizontal movement is anchored in place, and the samurai holds a guarded stance for up to **1.0 second**.
  - Deflecting any enemy hazard (Slime collision, Bringer of Death scythe slash, Thunder cloud strike, Boss Demon Slime charge, cleave, or ground slam) completely absorbs the damage, plays a newly synthesized metallic katana deflection clang (`SFX_BLOCK`), spawns `"[PARRY]"` floating text, grants 0.6s grace invulnerability, and applies a **2.0-second cooldown** (`blockCooldown = 2.0f`).
* **HUD and UI Tutorials**:
  - Added real-time HUD status badge for Guard state: displays cyan `[GUARD: ACTIVE]` when blocking, or red `[DEF: X.Xs]` during active cooldown.
  - Updated Tutorial modal window and Pause Menu with Defend controls and combat instructions.

### [v0.6.8] - 2026-10-25
#### Session Archive: [`v5.8 - Delayed Multi-Layer Thunder Spell, BGM Integration, Free Exploration Default, and Enemy Separation.md`](/doc/prompts/v5.8%20-%20Delayed%20Multi-Layer%20Thunder%20Spell,%20BGM%20Integration,%20Free%20Exploration%20Default,%20and%20Enemy%20Separation.md)
* **Telegraphed Multi-Layer Thunder Spells & Audio Synchronization**:
  - Broadened Bringer of Death vertical spell targeting threshold from `absY < 2.8f` to `absY < 14.0f` and horizontal range to `[3.0m, 14.0m]`, enabling underground cavern enemies to detect and cast thunder spells at players exploring ground-level or high sky-canopy platforms.
  - Implemented a 3-phase `SpellHazard` lifecycle:
    1. **Telegraph Warning Phase ($t \in [0.0\text{s}, 1.0\text{s}]$, frames 0–5)**: Dark storm cloud slowly gathers overhead at 5.0 FPS with an on-screen floating text alert (`"THUNDER CLOUD GATHERING! MOVE!"`), giving the player 1.0 full second to dash or jump away.
    2. **Impact & Strike Phase ($t \in [1.0\text{s}, 1.5\text{s}]$, frames 6–11)**: Directly on $t = 1.0\text{s}$, `audioMgr.playThunderSpell()` triggers the crackling thunder audio [`assets/thunder-after-crackle.mp3`](/assets/thunder-after-crackle.mp3). Deals damage only if the player remains within the $1.4\text{m}$ blast radius during this active impact window.
    3. **Dissipation Phase ($t \in [1.5\text{s}, 1.9\text{s}]$, frames 12–15)**: Lingering electric sparks and cloud remnants fade out smoothly before the hazard despawns at $t = 1.9\text{s}$.
* **Background Music System & Dynamic Mute**:
  - Integrated [`assets/Quarter_in_the_Slot.mp3`](/assets/Quarter_in_the_Slot.mp3) as looping in-game BGM using Windows MCI (`mciSendStringA`) streaming.
  - Initialized playback volume to 50% by default (`setaudio bgm volume to 500`).
  - Added dedicated `[M]` key toggle (`GLFW_KEY_M`) enabling players to mute/unmute music at will, featuring dynamic HUD status cards and floating notification popups (`"MUSIC: MUTED"` / `"MUSIC: UNMUTED (50%)"`).
  - Updated pause menu with interactive BGM audio status indicator.
* **Free Exploration Mode as Default**:
  - Re-anchored game design so Free Exploration Mode (`freeExplorationMode = true`) is active by default upon game launch and restart.
  - Player character retains idle breathing stillness when stationary and moves solely via manual `A`/`D` navigation.
  - Autonomous continuous sprint runner mode is preserved as an optional mechanic toggled via `[F]`.
  - Enriched Tutorial Window (`renderTutorialModal`) and Pause Menu (`renderPauseMenu`) with exhaustive descriptions of the new default exploration controls, katana attacks, spell dodging, and BGM mute key.
* **Enemy Separation Buffer**:
  - Added a strict 12.0m horizontal distance buffer in [`src/World.cpp`](/src/World.cpp) between Bringer of Death and Slime spawns, preventing enemy crowding and ensuring distinct, readable combat encounters.
* **Redundant Asset Cleanup**:
  - Removed obsolete `assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/No Effect Sprites` directory and registered it under [`.gitignore`](/.gitignore).

### [v0.6.7] - 2026-10-25
#### Session Archive: [`v5.7 - Shift Control Remap, BOD Centering & Spell Attack, Spike Clusters, and Boss Transformation SFX.md`](/doc/prompts/v5.7%20-%20Shift%20Control%20Remap,%20BOD%20Centering%20&%20Spell%20Attack,%20Spike%20Clusters,%20and%20Boss%20Transformation%20SFX.md)
* **Shift Mode Control Decoupled & Tutorial Window Flash Bug Resolved**:
  - Remapped Shift Mode toggle from `[T]` to `[Y]` (`GLFW_KEY_Y`), eliminating the conflict where pressing `T` simultaneously triggered autonomous direction shifts and opened the tutorial window.
  - Diagnosed and fixed the "split-second flash and disappear" bug of the Tutorial Window: when opening the modal via `[T]` in the pause menu, the keypress was being captured on the very next frame by the modal's dismissal check, immediately closing it. Added `tutDismissLast = true` upon modal trigger to enforce a fresh key-release cycle before dismissal can occur.
  - Updated both `renderPauseMenu` and `renderTutorialModal` to display `[Y] Shift Mode` status (`Auto Timer` vs `Manual Relics`) and comprehensive control instructions.
* **Bringer of Death Exact Pixel Centering**:
  - Analyzed the raw $140\times 93$ sprite canvas: the character's body sits at horizontal pixel range $X \in [81, 127]$ with its center at $X = 103.5$ (displaced $+33.5\text{ px}$ from canvas center $70.0$, or $+23.9\%$ of canvas width).
  - Calculated exact world-space quad offset: $0.2393 \times 4.5\text{m} = 1.077\text{m}$.
  - Applied offset translation ($-1.077\text{m}$ when facing left, $+1.077\text{m}$ when facing right), ensuring the visible Bringer of Death model is centered on its world entity position `enemy.pos.x` and platform hitbox.
* **Bringer of Death Ranged Thunder Spell Attack**:
  - Stitched `Individual Sprite/Cast` (9 frames) into `assets/enemies/bod_cast.png` ($1260\times 93$) and `Individual Sprite/Spell` (16 frames) into `assets/enemies/bod_spell.png` ($2240\times 93$).
  - Implemented distance-based spell casting AI: when player is at a distance ($3.8\text{m}$ to $12.0\text{m}$), Bringer of Death stops, locks facing direction, and casts a thunder spell (9 frames at 9 FPS).
  - Spawns an animated `SpellHazard` at the player's ground location: frames 0–5 depict a swirling thundercloud forming overhead (telegraph warning); frames 6–11 strike down as a lightning pillar inflicting damage/breaking shields; frames 12–15 dissipate into sparks.
* **Grouped Platform Trap Spikes & Increased Enemy Density**:
  - Elevated platforms now spawn grouped clusters of 2 to 4 spikes (`SpikeTrap`), creating distinct hazardous obstacles requiring calculated jumps, slides, or dashes.
  - Substantially increased non-initial chunk enemy spawning: allows 1 to 3 Bringers of Death per chunk with staggered spell cooldowns and increases Initial Slime spawn probability to 60% with multi-slime support.
* **3-Phase Sequenced Slime Transformation Audio & Slower Animation**:
  - Integrated native Windows MCI audio playback in `AudioManager` to stream 3 sequenced MP3 tracks during slime metamorphosis:
    1. `universfield-wet-splat-impact-567197.mp3` immediately upon slime defeat ($t = 0.0\text{s}$).
    2. `freesound_community-acid-bubbling-105397.mp3` during acid puddle dissolution ($t = 0.85\text{s}$).
    3. `audiopapkin-monster-roar-ps-o24-333057.mp3` as the giant Demon Slime awakens and rises ($t = 2.8\text{s}$).
  - Slowed down transformation animation to 6.0 FPS over 33 frames ($5.5\text{s}$ duration), synchronizing sprite visual phases with the audio progression.
* **Boss Demon Slime Smash Impact Audio**:
  - Replaced the placeholder bounce thump on `06_Smash.gif` platform touchdown with `dragon-studio-distant-bang-472364.mp3`, delivering a resonant impact on ground slam.

### [v0.6.6] - 2026-10-25
#### Session Archive: [`v5.6 - Updated Slime Animations and Boss Demon Slime Platform Jump Mechanics.md`](/doc/prompts/v5.6%20-%20Updated%20Slime%20Animations%20and%20Boss%20Demon%20Slime%20Platform%20Jump%20Mechanics.md)
* **Initial Slime Zoom & Crisp Proportions**:
  - Cropped and centered the active slime bounding box ($160\times 160$) from the lower-middle region of the raw $576\times 320$ GIF assets, eliminating vast empty transparent margins.
  - Generates compact, crystal-clear horizontal spritesheets: `slime_idle.png` ($960\times 160$), `slime_move.png` ($1280\times 160$), and `slime_take_hit.png` ($960\times 160$).
  - Rendered at `eRenderSize = glm::vec2(2.4f, 2.4f)` with grounded foot alignment, scaling the slime by 2.5x to be visibly detailed, clear, and perfectly proportioned on the platform.
* **Slime Transformation Fixed (Eliminated Black Screen & Texture Overflow)**:
  - Discovered that generating horizontal sheets for the 33-frame transformation at full $576\times 320$ produced an exorbitant width ($19,008$ pixels) that exceeded hardware `GL_MAX_TEXTURE_SIZE` limits (typically 8,192 or 16,384), causing `glTexImage2D` to fail and render as an incomplete, pure black object.
  - Cropped to the full morphing bounding box ($256\times 256$) and scaled to $160\times 160$ per cell, yielding `slime_transform.png` at $5280\times 160$ pixels ($5280 < 8192$), guaranteed to load and render with 100% reliability on all GPU architectures.
  - Positioned the transformation quad (`6.0f, 6.0f`) at `footY + 3.0f` so the slime morphs into the demon grounded seamlessly on the platform surface.
* **Boss Demon Slime Platform Jumping & Landing Physics Fixed**:
  - Fixed an issue where the boss became trapped mid-air and jumped repeatedly into the sky due to airborne physics prematurely halting when animation frame reached 10 prior to platform contact.
  - Re-engineered `ENEMY_STATE_JUMP` using a dedicated 3-phase physics state machine (`stateTimer`):
    - **Phase 0 (Crouch Wind-up)**: Boss prepares on the ground (frames 0 to 4).
    - **Phase 1 (Ballistic Airborne Flight)**: Launches with $v_y = 9.5$ to $12.5\text{ m/s}$ and $v_x = 4.0$ to $7.5\text{ m/s}$, maintaining gravity ($g = -22.0\text{ m/s}^2$) and cycling mid-air poses (frames 6–8) until feet make physical contact with a platform tile or ground surface.
    - **Phase 2 (Touchdown Slam & Shockwave)**: Upon touchdown ($v_y \le 0$), snaps feet to the platform top $+ 3.0\text{f}$, zeroes velocity, and executes frames 10–17 (slam impact, shockwave damage window frames 11–14, and recovery to patrol).
  - Enforced continuous ground gravity and platform surface snapping during patrol and idle states, preventing the boss from floating or drifting off platforms.
  - Scaled `demon_smash.png` to $288\times 160$ per cell ($5184\times 160$ total), matching all other demon sprite sheets and fitting safely within GPU limits.
* **Packaging & Standalone Distribution**:
  - Updated `TheWenderer_Standalone.zip` via `mingw32-make package`.

### [v0.6.5] - 2026-10-25
#### Session Archive: [`v5.5 - Lock Boss Attack Direction and Directional Damage Sector.md`](/doc/prompts/v5.5%20-%20Lock%20Boss%20Attack%20Direction%20and%20Directional%20Damage%20Sector.md)
* **Boss Attack Directional Commitment (Zero Mid-Attack Spinning)**:
  - Fixed an issue where the Demon Slime Boss (and Bringer of Death) continually updated their facing direction based on `distX < 0.0f` on every frame, even mid-swing, causing them to unnaturally flip $180^\circ$ mid-animation if the player jumped or dashed behind them.
  - Locked `enemy.facingLeft` at the start of the attack animation: while `enemy.state == ENEMY_STATE_ATTACK`, facing direction updates are suppressed.
* **Frontal Impact Damage Validation**:
  - Cleave and shockwave damage now strictly requires the player to be physically situated in front of the attacking enemy: `playerInFront = (enemy.facingLeft && distX <= 0.4f) || (!enemy.facingLeft && distX >= -0.4f)`.
  - Dashing or jumping behind the boss during its telegraph wind-up now cleanly evades the attack.

### [v0.6.4] - 2026-10-25
#### Session Archive: [`v5.4 - Fix Bringer of Death Facing, Realistic Katana Slash SFX, Guaranteed Tutorial Window, and Idle Stillness on Restart.md`](/doc/prompts/v5.4%20-%20Fix%20Bringer%20of%20Death%20Facing,%20Realistic%20Katana%20Slash%20SFX,%20Guaranteed%20Tutorial%20Window,%20and%20Idle%20Stillness%20on%20Restart.md)
* **Enemy Facing Direction Correction (Eliminated Moonwalking)**:
  - Discovered that enemy sprite sheets (`bod_walk.png`, `slime_move.png`, `demon_walk.png`) natively face **LEFT** in raw texture space, opposite to player sprites which natively face right.
  - Corrected horizontal UV flip calculation in [`src/main.cpp`](/src/main.cpp): `bool flipHorizontal = !enemy.facingLeft;`. When moving left (`facingLeft = true`), textures are rendered unflipped (`edu`), and when moving right (`facingLeft = false`), textures are flipped horizontally (`-edu`). Bringer of Death now walks forward in both directions.
* **Realistic Procedural Katana Sword Slash SFX**:
  - Overhauled `SFX_SLASH` in [`src/Audio.cpp`](/src/Audio.cpp) to synthesize a sharp, resonant Japanese katana sword slash.
  - Combined a swift aerodynamic whoosh (descending 2400 Hz to 320 Hz exponential sweep + band-passed white noise whip) with multi-frequency metallic steel resonance (3250 Hz, 4950 Hz, and 1920 Hz harmonic rings) and a sharp cutting transient bite.
* **Guaranteed Post-Intro Tutorial Window Modal**:
  - Fixed race condition where holding or pressing `Space`, `Enter`, or `Esc` to finish or skip the intro video immediately bleed through and dismissed the tutorial window on the subsequent frame.
  - Initialized `tutDismissLast = true` upon video skip/completion, strictly requiring the player to release and freshly press a key to dismiss the tutorial guide.
  - Froze gameplay simulation (`deltaTime = 0.0f`) while `showTutorialModal` is active so no enemies or hazards can advance while reading the tutorial.
* **Grounded Idle Stillness on Tutorial Dismiss & Game Over Restart**:
  - Ensured player remains completely motionless (`playerVel = (0, 0)`, `isGameIdleAtStart = true`) upon tutorial dismissal and upon Game Over restart.
  - Guarded Game Over input with `gameOverRestartLast` and prevented `Space` from registering as jump on the restart frame.
  - Preserved idle stance until directional navigation keys (`A`, `D`, `Left`, `Right`) are deliberately pressed, allowing the player full control over when to start running.

### [v0.6.3] - 2026-10-25
#### Session Archive: [`v5.3 - Calibrate Enemy Proportions, Speeds, Platform Grounding, and Telegraphed Attack Windups.md`](/doc/prompts/v5.3%20-%20Calibrate%20Enemy%20Proportions,%20Speeds,%20Platform%20Grounding,%20and%20Telegraphed%20Attack%20Windups.md)
* **Enemy Quad Aspect Ratios & Proportional Rendering**:
  - **Bringer of Death**: Corrected render quad from squished 1:1 square `(4.8, 4.8)` to true 1.5:1 aspect ratio `glm::vec2(4.5f, 3.0f)`, matching its $140\times 93$ sprite dimensions and completely eliminating vertical stretching.
  - **Initial Slime**: Scaled render quad from `(2.4, 2.4)` to `glm::vec2(4.5f, 2.5f)` (1.8:1 aspect ratio matching its $576\times 320$ canvas), making it fully visible and correctly proportioned rather than sunken and tiny.
  - **Boss Demon Slime**: Updated render quad from 1:1 square `(8.5, 8.5)` to true 1.8:1 aspect ratio `glm::vec2(10.8f, 6.0f)`, matching its $288\times 160$ sprite dimensions.
* **Platform Grounding & Ledge Boundary Clamping**:
  - Added [`World::hasSolidGroundAt`](/src/World.cpp) to verify the presence of solid tiles underneath entity feet.
  - Fixed procedural spawning in [`World::generateChunk()`](/src/World.cpp): verified candidate positions are not inside chasms or underground shafts with safety margins, and verified solid tiles underneath prior to enemy instantiation.
  - Raised enemy vertical center offsets so their feet rest flush upon ground level ($Y = 0.0\text{m}$ on surface, $Y = -4.0\text{m}$ in caverns): BOD at `y = 1.45f`, Slime at `y = 1.25f`, Boss at `y = 3.0f`.
  - Added continuous forward ledge/chasm boundary detection in the AI movement loop in [`src/main.cpp`](/src/main.cpp): patrolling enemies reverse direction at edges, and chasing enemies halt at ledges rather than walking into open chasms and floating between platforms in mid-air.
* **Calibrated Enemy Speeds & Movement Pacing**:
  - Bringer of Death: reduced patrol speed to $1.0\text{m/s}$ and alert chase speed to $1.3\text{m/s}$ (down from excessive $2.2\text{m/s}$).
  - Initial Slime: reduced wander speed to $0.75\text{m/s}$ and approach to $0.9\text{m/s}$ (down from $1.5\text{m/s}$).
  - Boss Demon Slime: reduced approach speed to $0.9\text{m/s}$ (down from $1.8\text{m/s}$), providing heavy, menacing footsteps.
* **Telegraphed Attack Wind-Up Animations**:
  - **Bringer of Death**:
    - Initiates attack state when player is within $2.4\text{m}$, immediately halting horizontal movement.
    - Added $0.75\text{s}$ wind-up telegraph window (frames 0-5 at 8 fps) with glowing overhead red alert `"[ ! ]"`, raising scythe in preparation.
    - Cleave impact window restricted to frames 6-7 ($0.75\text{s}$ to $1.0\text{s}$), allowing player ample time to jump, dash away, or strike first.
  - **Boss Demon Slime**:
    - Removed unavoidable instantaneous walking contact damage.
    - Initiates telegraphed cleave attack when player is within $4.2\text{m}$, halting forward walking.
    - Added $1.125\text{s}$ wind-up telegraph window (frames 0-8 at 8 fps) with overhead alert `"[ ! ] DODGE!"` as the demon draws back its flame-charged arm.
    - Ground slam shockwave damage window restricted to frames 9-11 ($1.125\text{s}$ to $1.5\text{s}$), giving players over a full second to dodge or dash through.
  - Fixed animation timer framerate calculation: standardized `enemy.animTimer` to real-world seconds (`+= deltaTime`), eliminating compounded double-multiplication framerate glitch that caused instant attacks.

### [v0.6.2] - 2026-10-25
#### Session Archive: [`v5.2 - Modular Decomposition of Main Engine into UIManager, LadderScene, and SkyFallScene.md`](/doc/prompts/v5.2%20-%20Modular%20Decomposition%20of%20Main%20Engine%20into%20UIManager,%20LadderScene,%20and%20SkyFallScene.md)
* **Code Size Optimization & Subsystem Modularization**:
  - Refactored monolithic [`src/main.cpp`](/src/main.cpp) by extracting three major functional domains into dedicated header and source pairs matching the project's existing modular design:
    1. **`UIManager` Subsystem** ([`include/UIManager.h`](/include/UIManager.h), [`src/UIManager.cpp`](/src/UIManager.cpp)):
       - Encapsulated floating text management, corner HUD alerts, top-right score counter, survival trial countdown badges, overhead enemy health bars, pause menu dialog, game over modal, and tutorial window into a thread-safe singleton.
    2. **`LadderScene` Subsystem** ([`include/LadderScene.h`](/include/LadderScene.h), [`src/LadderScene.cpp`](/src/LadderScene.cpp)):
       - Encapsulated 3-track vertical ladder jumping physics, procedural rungs, sawblades, spike traps, shield pickups, reverse resurrection animation spotlight, and ladder scene rendering.
    3. **`SkyFallScene` Subsystem** ([`include/SkyFallScene.h`](/include/SkyFallScene.h), [`src/SkyFallScene.cpp`](/src/SkyFallScene.cpp)):
       - Encapsulated high-velocity aerial descent physics, horizontal steering, procedural raptor bird obstacles, floating spike mines, exit gate generation, and 90-degree CCW impact rotation animation.
* **Build Configuration & Zero-Regression Verification**:
  - Updated [`Makefile`](/Makefile) compilation targets to compile and link all new modules (`UIManager.cpp`, `LadderScene.cpp`, and `SkyFallScene.cpp`).
  - Verified clean compilation with zero warnings and zero errors, and re-generated `TheWenderer_Standalone.zip`.

### [v0.6.1] - 2026-10-25
#### Session Archive: [`v5.1 - Standardize Standalone Executable to TheWenderer.exe and Eliminate Redundant main.exe.md`](/doc/prompts/v5.1%20-%20Standardize%20Standalone%20Executable%20to%20TheWenderer.exe%20and%20Eliminate%20Redundant%20main.exe.md)
* **Packaging & Distribution Cleanup**:
  - Eliminated duplicate executable issue where both `main.exe` and `TheWenderer.exe` were packaged into `TheWenderer_Standalone.zip`.
  - Updated [`makefile`](/makefile) `package` target to explicitly copy `build/main.exe` directly as `TheWenderer.exe` in both `TheWenderer/` and `dist/TheWenderer/`.
  - Purged leftover `TheWenderer/main.exe` and regenerated `TheWenderer_Standalone.zip` with a single, branded executable: `TheWenderer/TheWenderer.exe`.

### [v0.6.0] - 2026-10-25
#### Session Archive: [`v5.0 - Combat System, 3 Lives, Spikes, Bringer of Death, and Demon Slime Boss Battle.md`](/doc/prompts/v5.0%20-%20Combat%20System,%203%20Lives,%20Spikes,%20Bringer%20of%20Death,%20and%20Demon%20Slime%20Boss%20Battle.md)
* **Real-Time Combat & Player Attack System**:
  - Implemented melee sword slash attack triggered by **`[J]`**, **`[X]`**, or **Left Mouse Button (LMB)**.
  - Dynamically activates `samuraiAnim.texAttack` with an oriented front-facing hitbox sweep arc ($2.4\times 2.0$ meters at $1.35\text{m}$ reach).
  - Exact damage scaling per specification:
    - **Bringer of Death**: Slain in **1 hit** (+250 points, +5 coins).
    - **Initial Slime**: Slain in **2 hits**, triggering a dramatic delayed demonic transformation into the Boss!
    - **Boss Demon Slime**: Slain in **5 hits** (+1000 boss defeat bonus, +20 coins).
* **Player 3-Lives System & Heart Relics**:
  - Replaced instant sudden-death with a forgiving **3-Lives Health System** (`playerLives = 3`).
  - Added animated ruby heart pickups (`assets/heart_pickup.png`) spawning directly alongside shield relics.
  - Picking up a heart restores **+1 Life pip** (up to 3 max); at full health, grants **+150 Overheal Bonus Score**.
  - Taking damage (spike trap impalement, enemy blade, or slime contact) reduces 1 life pip and grants **1.25s grace invulnerability** with flash feedback.
  - Score HUD updated to render 3 animated heart life pips beside the distance and coin meters.
* **Procedural Metallic Spikes Hazard Texture**:
  - Generated procedural $64\times 64$ metallic spike trap texture `assets/spikes.png` with steel teeth and red hazard warning base.
  - Replaced Brackeys pumpkin tile UVs with `spikesTexture` across the platform runner, `LadderScene`, and `SkyFallScene`.
* **Enemy AI, Boss Battle & Avoidance**:
  - **Bringer of Death** ([Clembod](https://clembod.itch.io/bringer-of-death-free)): Roams surface and caverns; alerts and swings death scythe when close.
  - **Initial Slime** ([Chierit](https://chierit.itch.io/boss-demon-slime)): Spawns at random on surface; hops towards player. Defeating it triggers a 3.2s transformation with audio roar into the Boss Demon Slime.
  - **Boss Demon Slime**: Giant $8.5\times 8.5\text{m}$ boss entity with 5 HP, overhead crimson health bar, heavy cleave attacks, and direct contact damage.
  - **Boss Avoidance & Despawn**: Player can avoid the fight by sprinting in the opposite direction ($>28\text{m}$) or dashing through it ($>8\text{m}$ away), causing the boss to retreat and despawn cleanly.
* **Procedural Audio Waveforms for Combat & Enemies**:
  - Added 8 procedural synthetic waveforms in `Audio.cpp`: `SFX_SLASH`, `SFX_ENEMY_HIT`, `SFX_ENEMY_ATTACK`, `SFX_ENEMY_DEATH`, `SFX_SLIME_BOUNCE`, `SFX_BOSS_ROAR`, `SFX_BOSS_ATTACK`, `SFX_HEART_PICKUP`.
* **Overhead Health Bars & Relic Balancing**:
  - Rendered segmented floating health bars with custom color codings above roaming enemies and wide crimson health bar above the Demon Slime.
  - Shield relics rebalanced to spawn less frequently (1 in 5 platform roll) and positioned strictly on separate platforms away from direction-change relics.

### [v0.5.15] - 2026-10-25
#### Session Archive: [`Ladder Hero Scale Fix, Post-Intro Tutorial Modal, Shield Pickup Icon, and Scene Exit Timers - v4.17`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#ladder-hero-scale-fix-post-intro-tutorial-modal-shield-pickup-icon-and-scene-exit-timers---v417)
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
#### Session Archive: [`Circular Energy Shield Bubble and Single-Hit Trap Absorption - v4.16`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#circular-energy-shield-bubble-and-single-hit-trap-absorption---v416)
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
#### Session Archive: [`Mini-Game Scene Exit Restrictions and Underground Ladder Descent Fix - v4.13`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#mini-game-scene-exit-restrictions-and-underground-ladder-descent-fix---v413)
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
#### Session Archive: [`Free Exploration Mode and Manual Movement Toggle - v4.12`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#free-exploration-mode-and-manual-movement-toggle---v412)
* **Mechanics & Player Controls**:
  - **Free Exploration Mode (`F` Key)**:
    - *Motivation*: The default game mode operates as a continuous high-speed endless runner where the Samurai sprints forward autonomously. Players exploring intricate platforms, cavern shafts, or avoiding obstacles needed full, unhurried manual control.
    - *Velocity Decoupling*: When Free Exploration Mode is active (`freeExplorationMode = true`), continuous autonomous velocity injection is suppressed. The horizontal velocity $v_x$ is strictly driven by whether `A` (run left, $-v_{run}$) or `D` (run right, $+v_{run}$) is held. Releasing both keys immediately zeros horizontal velocity ($v_x = 0$), bringing the character to an immediate, stable halt.
    - *Ground Idle State Resolution*: In Free Exploration, whenever $v_x = 0$ on solid ground, the player state machine resolves to `ANIM_IDLE`, playing Samurai Mack's breathing animation. Pressing `A` or `D` flips the sprite orientation and transitions smoothly to `ANIM_RUN`.
    - *Ladder Climb Hold*: On vertical ladder shafts in Free Exploration Mode, vertical climbing velocity $v_y$ is clamped to 0 when neither `W` nor `S` is held, allowing players to pause on ladders and observe moving hazards.
    - *UI & Feedback*: Displays floating popup `FREE EXPLORATION: ON / OFF`, adds on-screen HUD badge in top-left, updates window title, and adds status item `[F] MODE: FREE EXPLORATION` in the ESC Pause Menu.

### [v0.5.11] - 2026-10-25
#### Session Archive: [`Fix Standalone Missing DLL and Video Rendering Alpha Discard - v4.11`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#fix-standalone-missing-dll-and-video-rendering-alpha-discard---v411)
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
#### Session Archive: [`Standalone Single-File Game Packaging Analysis - v4.10`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#standalone-single-file-game-packaging-analysis---v410)
* **Packaging & Distribution Engineering**:
  - **Standalone Feasibility & Architecture**: Comprehensively analyzed all three technical methods for packaging *The Wenderer* into a standalone file:
    1. **Virtual Container / Single-File SFX**: Packaging `main.exe`, `glfw3.dll`, and `assets/` into a single self-contained `.exe` container with zero user extraction required.
    2. **In-Binary Resource Embedding**: Compiling all assets directly into `.rsrc` / `.rodata` PE sections with memory-based loaders (`stbi_load_from_memory`, `IMFByteStream`).
    3. **Self-Contained Portable Archive / Installer**: Industry-standard distribution zip containing all runtime files and assets ready for distribution on platforms like itch.io and Steam.
  - **Automated `package` Build Target**: Added `package` target to [`Makefile`](/Makefile) that assembles a clean, standalone distribution archive (`TheWenderer_Standalone.zip`, ~5.8 MB) containing the executable, dynamic libraries, and complete asset suite with zero local build toolchain requirements for end users.
  - **Runtime Dependency Verification**: Inspected PE import tables via `objdump`, confirming standard Windows UCRT, Media Foundation (`MFPlat`, `MFReadWrite`), WinMM, and GDI32 system linkage across Windows 10 and 11 environments.

### [v0.5.9] - 2026-10-25
#### Session Archive: [`In-Engine Intro Video Playback and Asset Embedding Analysis - v4.9`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#in-engine-intro-video-playback-and-asset-embedding-analysis---v49)
* **Architectural Additions**:
  - **In-Engine Video Subsystem (`VideoPlayer`)**: Created [`include/VideoPlayer.h`](/include/VideoPlayer.h) and [`src/VideoPlayer.cpp`](/src/VideoPlayer.cpp) utilizing native Windows Media Foundation (`IMFSourceReader`) with zero external binary dependencies. Streams and decodes H.264 video frames directly to an OpenGL 2D texture (`GL_BGRA` / `glTexSubImage2D`) at native 1080p resolution while simultaneously decoding AAC audio streams into in-memory PCM Wave audio played asynchronously via `PlaySoundA`.
  - **Game State Transition**: Introduced `STATE_INTRO_VIDEO = 0` to `GameState` in [`include/Common.h`](/include/Common.h). The engine initializes into this state before the platformer begins.
  - **Cinematic Presentation & Responsive Letterboxing**: Automatically computes aspect ratios (`16:9`) to letterbox the video inside any window size, rendering an animated bottom progress bar and pulsing skip prompt text (`[SPACE / ESC / ENTER] SKIP INTRO`).
  - **Skip & End-of-Stream Handling**: Spacebar, Enter, or Escape gracefully aborts video and audio playback, closes decoder handles, and places the player safely into the starting idle runner state (`STATE_RUNNING`, `isGameIdleAtStart = true`).
* **Technical Embedding & Size Optimization Analysis**:
  - Evaluated embedding `introVid.mp4` directly into `.exe` via Windows Resource Compiler (`windres` / `.rc`) vs. compiling as a byte array vs. streaming from disk.
  - Explained mathematical reasons why embedding an already-compressed H.264 MP4 file into executable binary files does **not** reduce its file size, and detailed real size-reduction strategies (transcoding to 720p H.265/AV1 or lowered bitrates).

### [v0.5.8] - 2026-10-25
#### Session Archive: [`Sky-Fall Down Scene Left and Right Direction Relics - v4.8`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#sky-fall-down-scene-left-and-right-direction-relics---v48)
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
#### Session Archive: [`Ladder Scene Direction Trigger Relics & Exit Spawning - v4.7`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#ladder-scene-direction-trigger-relics-exit-spawning---v47)
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
#### Session Archive: [`Left Chunk Persistence, Direction Arrows, Shield Fruits, Idle Start, Single Hero, and Down Scene Pacing - v4.6`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#left-chunk-persistence-direction-arrows-shield-fruits-idle-start-single-hero-and-down-scene-pacing---v46)
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
#### Session Archive: [`Dynamic Day-Night Cycle, Thematic Background Sceneries, and Scene Cooldown - v4.5`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#dynamic-day-night-cycle-thematic-background-sceneries-and-scene-cooldown---v45)
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
#### Session Archive: [`Dash Mechanic, Left Starting Platforms, and Dynamic Up-Down Mini-Game Scenes - v4.4`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#dash-mechanic-left-starting-platforms-and-dynamic-up-down-mini-game-scenes---v44)
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
#### Session Archive: [`Downward Ladder Entry and Procedural World Generation Analysis - v4.3`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#downward-ladder-entry-and-procedural-world-generation-analysis---v43)
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
#### Session Archive: [`Floating Text for Fruit Pickups and Auto-Shift Disabling - v4.2`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#floating-text-for-fruit-pickups-and-auto-shift-disabling---v42)
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
#### Session Archive: [`Slow Down Running and Climbing Animation Framerate - v4.1`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#slow-down-running-and-climbing-animation-framerate---v41)
* **Bug Fixes & Animation Calibrations**:
  - **Eliminated Double-Multiplication Framerate Glitch**: Fixed an issue where `animTimer` was multiplied by $10.0$ in the update loop and then multiplied by $12.0$ in the frame index selector, causing the running animation to play at a glitchy $120\,\text{FPS}$ ($15$ full run cycles per second).
  - **Normalized Timer Accumulation**: `animTimer` now accumulates pure elapsed real-world seconds (`animTimer += deltaTime`).
  - **Calibrated Running Speed**: Running animation now cycles at $8.0\,\text{FPS}$ ($125\,\text{ms}$ per frame), making leg strides, flowing clothes, and katana movement clear and readable.
  - **Calibrated Ladder Climbing Speed**: Climbing animation now cycles at $3.5\,\text{FPS}$ ($285\,\text{ms}$ per step), producing deliberate, tactile vertical hand-over-hand climbing.
  - **Calibrated Idle Breathing Speed**: Combat breathing stance cycles at $4.0\,\text{FPS}$.

---

### [v0.5.0] - 2026-10-24
#### Session Archive: [`Samurai Ninja Player Animations Pause Menu and Score Counter - v4.0`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#samurai-ninja-player-animations-pause-menu-and-score-counter---v40)
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
#### Session Archive: [`Technical Documentation and Academic Guide Creation - v3.6`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#technical-documentation-and-academic-guide-creation---v36)
* **Added**:
  - Created [`/doc/documentation.md`](/doc/documentation.md): Comprehensive technical progress report covering architecture, subsystem design, graphics pipeline, physics tuning, and build specifications.
  - Created [`/doc/explain.md`](/doc/explain.md): Accessible academic guide stripping away technical jargon, using everyday analogies (*Rulebook, Musician, Artist, Architect, Director*) to explain each code module for project presentations and lab viva exams.
  - Updated [`/README.md`](/README.md) with quick links to both newly prepared documents.

---

### [v0.4.2] - 2026-10-24
#### Session Archive: [`Modular Code Refactoring Ladder Stopping and Cavern Chasm Fixes - v3.5`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#modular-code-refactoring-ladder-stopping-and-cavern-chasm-fixes---v35)
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
#### Session Archive: [`Multi-Elevation Worlds Caverns Sky Tier and Midpoint Interrupts - v3.4`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#multi-elevation-worlds-caverns-sky-tier-and-midpoint-interrupts---v34)
* **Added**:
  - **Horizontal Direction Memory (`lastHorizontalDir`)**: Ascending or descending ladder shafts now remembers the prior horizontal run direction (`DIR_RIGHT` or `DIR_LEFT`), resuming that direction upon summiting instead of forcing an arbitrary rightward turn.
  - **Mid-Slope Jumping & Traversal Interrupts**: Mid-slope sliding can now be interrupted at any point by jumping (`Space`/`W`/Up) or steering (`A`/`D`), giving full control over downward momentum.
  - **Underground Cavern Zone ($Y = -4.5\,\text{m}$)**: Added subterranean cavern corridors entered via downward dive slopes and surface chasms, featuring multiple upward ladder shafts leading back to intermediate ledges and the surface.
  - **Super-Elevated Sky Canopy Tier ($Y = +5.5\,\text{m}$)**: High-altitude sky platforms accessed via extended ladder shafts, concluding with grand downward return slopes leading back to the surface.
  - **Locked Vertical Ladder Climbing**: Sidewise drift is strictly locked to zero while climbing ladders unless the player explicitly triggers a mid-ladder jump or dismount.
  - **Bedrock Foundation Lowering**: Bedrock lowered to $Y = -7.5\,\text{m}$ to catch players beneath all cavern pits, guaranteeing zero out-of-bounds fall deaths.

---

### [v0.4.0] - 2026-10-24
#### Session Archive: [`Tunnels Slopes Audio SFX Dynamic Endless Platforms and Corner HUD - v3.2`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#tunnels-slopes-audio-sfx-dynamic-endless-platforms-and-corner-hud---v32)
* **Added**:
  - **Truly Endless Procedural Platforms**: Replaced fixed layer structures with a randomized multi-tier platform generator producing randomized elevations, lengths, and gaps.
  - **Vertical Ladder Tunnel Shafts**: Enclosed ladders within masonry-walled vertical shafts featuring shadowed interior backdrops and illuminated guide lines.
  - **Steep Downhill Slide Slopes**: Implemented angled ramp physics connecting higher tiers to lower levels; stepping onto a slope triggers downhill sliding, tilted sprite animation, and rushing wind SFX.
  - **In-Memory Procedural Audio Engine (`winmm`)**: Added zero-dependency synthesized PCM WAV audio for footsteps, jumps, ladder climb, slope slides, coin pickups, relic pickups, direction warp transitions, and spike death impacts.
  - **Comprehensible Corner HUD Notifications**: Added glassmorphism top-left notification cards with rotating item icons, category tags, titles, and explanations on every pickup.
  - **Dynamic Biomes & Transition Phase**: Added 4 distinct themes (Emerald Forest, Frozen Glacier, Sunset Canyon, Mystic Cavern) with cinematic slow-mo transition flashes and smooth environmental color interpolation.

---

### [v0.3.0] - 2026-10-24
#### Session Archive: [`Brackeys Assets Ladders Pitfalls Spikes and Direction Relics - v3.1`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#brackeys-assets-ladders-pitfalls-spikes-and-direction-relics---v31)
* **Added**:
  - **Brackeys Platformer Asset Integration**: Integrated pixel-art textures from the Brackeys platformer bundle: `world_tileset.png`, `platforms.png`, `coin.png`, and `fruit.png`.
  - **Mystery Direction Relics**: Floating fruit pickups that immediately trigger autonomous direction shifts (`DIR_RIGHT`, `DIR_LEFT`, `DIR_UP`, `DIR_DOWN`) with an on-screen alert banner.
  - **Vertical Traversal (Ladders & Pitfalls)**: Added climbable wooden ladders and intentional pitfall gaps with one-way drop-through platforms (`S` / Down).
  - **Abyss Immunity**: Replaced bottomless abyss deaths with an endless bedrock safety foundation with textured grass and dirt tiles.
  - **Lethal Spiky Traps**: Hazardous spikes on platforms and pit floors; walking into or landing on spikes triggers instant impalement and Game Over.
  - **Animated Gold Coins**: 12-frame rotating gold coins placed along platform runways (+50 pts).

---

### [v0.2.0] - 2026-10-24
#### Session Archive: [`Side-View Endless Runner Transition with Dynamic Direction and Sprites - v3.0`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#side-view-endless-runner-transition-with-dynamic-direction-and-sprites---v30)
* **Major Perspective Shift**:
  - Converted the game from a top-down ribbon runner to a classic 2D side-view endless platformer.
  - Re-mapped physics coordinates: horizontal axis ($X$) represents travel progression, vertical axis ($Y$) represents elevation, jump physics, and downward gravity.
* **Added**:
  - Integrated open-source Liberated Pixel Cup (LPC) composite character spritesheet (`assets/player_sheet.png`) and `stb_image.h` PNG decoder.
  - Dynamic character animation states: Run Right (Row 37), Run Left (Row 35), Climb Up/Down (Row 30), Fall (Row 26 col 3), and Jump (Row 26 col 1).
  - Autonomous dynamic direction flips triggered by random countdown intervals (3.0s–6.0s) with visual alert banners.

---

### [v0.1.2] - 2026-10-21
#### Session Archive: [`Project Structure and Version Archiving Advice - v2.0`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#project-structure-and-version-archiving-advice---v20)
* **Documentation & Repository Structure**:
  - Evaluated repository structure and documented the distinction between monolithic gameplay source files and external project dependencies (`glad.c`, `include/`, `lib/`).
  - Formulated version snapshot strategies and best practices for archiving game builds before major milestone refactoring.

---

### [v0.1.1] - 2026-10-21
#### Session Archive: [`Convert File Paths to Root-Relative - v1.2`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#convert-file-paths-to-root-relative---v12)
* **Standards & Compliance**:
  - Converted all internal documentation file paths and markdown links across `README.md`, `AGENTS.md`, and prompt archives to repository root-relative paths (`/doc/...`).
  - Eliminated machine-specific absolute file URIs (`file:///...`) for portability across platforms and remote Git hosting.

---

### [v0.1.0-hotfix] - 2026-10-18
#### Session Archive: [`Fix Makefile for MSYS2 Sh Environment - v1.1`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#fix-makefile-for-msys2-sh-environment---v11)
* **Build System Fix**:
  - Diagnosed and resolved MSYS2 `/bin/sh` syntax error when executing `make win` on Windows.
  - Removed shell-dependent batch conditions from [`/Makefile`](/Makefile).
  - Updated [`.gitignore`](/.gitignore) to track [`build/.gitkeep`](/build/.gitkeep) and [`build/glfw3.dll`](/build/glfw3.dll) while ignoring compiled binaries.

---

### [v0.1.0] - 2026-10-17
#### Session Archive: [`Project Analysis and Phase 1 Implementation - v1.0`](/doc/prompts/v1.0-v4.17%20-%20Merged%20Prompts%20Archive.md#project-analysis-and-phase-1-implementation---v10)
* **Initial Release (Phase 1 Complete)**:
  - Implemented the core engine prototype using Modern OpenGL 3.3 Core Profile in C++.
  - Created a single reusable $1 \times 1$ Quad VAO/VBO and MVP GLSL shader replacing hardcoded geometry.
  - Added delta-time game loop using GLFW time.
  - Implemented vector-adaptive movement with lateral lane shifting and jumping relative to the active movement axis.
  - Created procedural obstacle tracks with AABB collision detection and game reset states.
  - Implemented the prototype "Wending" mechanic: 90-degree orthogonal track pivot points with smooth rotational camera follow using `glm::mix`.
  - Created initial documentation [`/doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md), [`/Makefile`](/Makefile), [`/CMakeLists.txt`](/CMakeLists.txt), and [`/AGENTS.md`](/AGENTS.md).
