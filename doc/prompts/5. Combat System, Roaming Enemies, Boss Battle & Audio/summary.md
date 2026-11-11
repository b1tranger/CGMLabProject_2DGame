# Session 5 Summary: Combat System, Roaming Enemies, Boss Battle & Audio

- **Session Number**: 5
- **Legacy Versions**: `v5.0 - v5.15`
- **Session Folder**: [`/doc/prompts/5. Combat System, Roaming Enemies, Boss Battle & Audio/`](/doc/prompts/5.%20Combat%20System,%20Roaming%20Enemies,%20Boss%20Battle%20&%20Audio/)
- **Date(s)**: 2026-10-25 to 2026-10-26
- **Conversation Log**: [`conversation.md`](/doc/prompts/5.%20Combat%20System,%20Roaming%20Enemies,%20Boss%20Battle%20&%20Audio/conversation.md)
- **Transcript Status**: Migrated from legacy Markdown archives (raw `transcript_full.jsonl` unavailable).

---

## 1. User Goals
- **v5.0** (Combat System, 3 Lives, Spikes, Bringer of Death, and Demon Slime Boss Battle): Implementation of player melee combat (J/X/LMB), 3-lives health system, ruby heart pickups, procedural metallic spike hazards, Bringer of Death roaming enemy, Demon Slime Boss Battle encounter with delayed metamorphosis, avoidance/despawn mechanics, overhead segmented health bars, and 8 procedural synthesized SFX waveforms.
- **v5.1** (Standardize Standalone Executable to TheWenderer.exe and Eliminate Redundant main.exe): Investigation and resolution of duplicate executables (`main.exe` and `TheWenderer.exe`) in `TheWenderer_Standalone.zip`.
- **v5.2** (Modular Decomposition of Main Engine into UIManager, LadderScene, and SkyFallScene): Refactoring `src/main.cpp` by decomposing monolithic code into dedicated subsystems (`UIManager`, `LadderScene`, and `SkyFallScene`) with clean header/source modular boundaries, updated build configuration, and zero regression of gameplay mechanics.
- **v5.3** (Calibrate Enemy Proportions, Speeds, Platform Grounding, and Telegraphed Attack Wind-Ups): Resolving enemy sprite aspect ratio distortion, sizing, platform collision/chasm floating bugs, speed pacing, and telegraphed attack wind-up windows for responsive player dodge mechanics.
- **v5.4** (Fix Bringer of Death Facing, Realistic Katana Slash SFX, Guaranteed Tutorial Window, and Idle Stillness on Restart): Resolving enemy sprite facing inversion (moonwalking), synthesizing realistic procedural katana sword slash audio, fixing post-intro tutorial modal race condition dismissal, and enforcing motionless idle start on tutorial close and game over restart.
- **v5.5** (Lock Boss Attack Direction and Directional Damage Sector): Enforcing directional attack commitment for Boss Demon Slime and Bringer of Death, suppressing mid-animation flips, and validating frontal impact damage sectors.
- **v5.6** (Updated Slime Animations and Boss Demon Slime Platform Jump Mechanics): Integrated updated initial slime animation assets from `assets/enemies/boss_demon_slime/initial_slime/`, resolved texture overflow black box issue, zoomed in initial slime sprites, and implemented robust multi-platform jumping traversal for Boss Demon Slime utilizing `06_Smash.gif`.
- **v5.7** (Shift Control Remap, BOD Centering & Spell Attack, Spike Clusters, and Boss Transformation SFX): Control decoupling (`T` for Tutorial, `Y` for Shift Mode), Tutorial modal persistence fix, Bringer of Death pixel bounding-box centering and ranged thunder spell casting, grouped platform spikes, multi-enemy procedural spawns, 3-phase sequenced audio during slime transformation, and heavy distant bang audio for boss smash landings.
- **v5.8** (Delayed Multi-Layer Thunder Spell, BGM Integration, Free Exploration Default, and Enemy Separation): Implementation of delayed telegraphed thunder spell casting across multi-elevation platform tiers, thunder crackle SFX integration, BGM playback with volume and mute toggles, default Free Exploration mode, enemy separation buffer, and redundant sprite cleanup.
- **v5.9** (Synchronized Thunder Spell SFX and Right-Click Katana Defend Mechanic): Bringer of Death Spellcast SFX Pre-Delay Synchronization with Attack Animation, Katana Defend / Block Stance Integration (`assets/samurai/Defend_nobg.png`), 1.0s Defensive Guard Window, 2.0s Parry Cooldown, and Full Damage Negation Across All Enemy Attacks.
- **v5.10** (Documentation and Academic Guide Synchronization with Modern Subsystems): Comprehensive overhaul and synchronization of `doc/explain.md`, `doc/documentation.md`, and `README.md` to reflect the 8 modular subsystems, Modern OpenGL 2D sprite mechanics, real-time combat & defend systems, enemy AI behaviors, and hybrid audio architecture.
- **v5.11** (Mid-Animation Death Spell Damage and Visual Telegraph Window): Resolving instantaneous Death Spell damage upon cloud manifestation, implementing a 0.58s visual cloud-gathering telegraph window, and strictly anchoring damage evaluation to mid-animation frames 7–10 when lightning strikes the ground.
- **v5.12** (Increase Slime Enemy Spawn Frequency and Multi-Elevation Density): Calibration of procedural world enemy generation in [`/src/World.cpp`](/src/World.cpp) to significantly increase initial slime enemy frequency, add immediate spawn-chunk encounters, enable subterranean cavern encounters, and refine deconfliction buffers between Bringers of Death and slimes.
- **v5.13** (Scale Down Player Defend Animation to Match Idle Spritesheet Proportions): Empirical coordinate inspection, super-sampled downscaling, and grounding alignment of the player's katana defend animation [`/assets/samurai/Defend_nobg.png`](/assets/samurai/Defend_nobg.png) to match the pixel height, width, and footing of [`/assets/samurai/Idle.png`](/assets/samurai/Idle.png).
- **v5.14** (Slime Ground Placement & AI Unfreezing, and Boss Demon Slime Dying Roar Audio Integration): Diagnosis and resolution of slime enemy spawn coordinate offset ($0.5\text{m}$ underground) and collision raycast failure resulting in AI movement freeze, plus integration of authentic dying roar audio ([`/assets/dyingRoar.mp3`](/assets/dyingRoar.mp3)) for Boss Demon Slime's death animation sequence ([`/assets/enemies/demon_death.png`](/assets/enemies/demon_death.png) / `05_d_death.gif`).
- **v5.15** (Boss Demon Slime Death Animation Calibration to Match Updated Dying Roar Audio): Waveform decoding and acoustic analysis of the updated [`/assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) audio stream, and deceleration of the Boss Demon Slime's 22-frame death animation sequence ([`/assets/enemies/demon_death.png`](/assets/enemies/demon_death.png) / `05_d_death.gif`) to $6.0\,\text{FPS}$ with an extended $4.0\,\text{second}$ dissipation window.

---

## 2. Engineering & Implementation Details
### v5.0 - Combat System, 3 Lives, Spikes, Bringer of Death, and Demon Slime Boss Battle
#### 1. Requirement & Constraint Analysis
- **Hazard Visual Fidelity & Misattribution (Spikes vs. Pumpkins)**:
  - Inspection of the Brackeys `world_tileset.png` at tile coordinate $(4, 8)$ revealed an orange autumnal pumpkin ($RGB(149, 98, 24)$) rather than a metallic spike trap. This visual mismatch caused player confusion regarding lethal hazards.
  - *Engineering Strategy*: Procedurally synthesized a dedicated high-resolution $64\times 64$ metallic spike trap texture `assets/spikes.png` displaying three razor-sharp steel spikes with ambient occlusion, metallic specular highlights, and a hazard-red base bar. Configured UV mapping across the platform runner, `LadderScene`, and `SkyFallScene` to render genuine steel spikes.
- **Relic Deconfliction & Spacing Architecture**:
  - Previously, Mystery Fruit Relics (granting 10s shields) were placed in close proximity to directional arrowheads, causing accidental shield pickups or involuntary direction shifts when attempting to grab protective shields.
  - *Engineering Strategy*: Segregated shield pickups to elevated, isolated sky canopy platforms, reducing procedural spawn probability to a calibrated 1-in-5 chance (`!hasDirectionRelic && rand() % 5 == 0`), placing heart pickups directly adjacent to shield pickups for clear defensive utility clustering.
- **Player 3-Lives Pipeline & Grace Recovery Invulnerability**:
  - The previous physics loop immediately triggered `STATE_GAME_OVER` upon touching any unshielded hazard.
  - *Engineering Strategy*: Introduced `playerLives = 3` with visual heart pip rendering in the top-right Score HUD. Taking damage from spike impalement, enemy blade swings, or slime contact decrements 1 life pip and immediately engages a $1.25\text{s}$ grace period (`postHitInvulnTimer = 1.25f`) with red damage screen tinting and floating text feedback (`"LIFE LOST! (X/3)"`). Death occurs strictly when `playerLives <= 0`.
  - Created animated 4-frame ruby heart pickups (`assets/heart_pickup.png`). Picking up a heart restores $+1$ life pip up to the maximum of 3, or confers $+150$ bonus score if already at maximum health.
- **Real-Time Combat & Hitbox Arc Sweeping**:
  - Enabled sword attack triggers via **`[J]`**, **`[X]`**, and **Left Mouse Click (LMB)**.
  - Decoupled full animation length ($6\text{ frames} \approx 0.5\text{s}$) from the active swing contact window ($t \in [0.10, 0.32]\text{s}$). During active swing frames, projected an oriented forward hitbox arc:
    $$\mathbf{p}_{\text{hit}} = \mathbf{p}_{\text{player}} + (\text{dirSign} \times 1.35\,\hat{\mathbf{i}})$$
  - Integrated `attackDamageDealt` latch to ensure a single swing damages enemies exactly once per attack cycle.
- **Enemy Archetypes & Finite State Machine (FSM)**:
  - Downloaded official itch asset packages for [Bringer of Death](https://clembod.itch.io/bringer-of-death-free) and [Boss Demon Slime](https://chierit.itch.io/boss-demon-slime). Converted and assembled horizontal sprite strips in `assets/enemies/`.
  - **Bringer of Death (1 Hit)**: Roams platforms in `ENEMY_STATE_PATROL`. Detects player within $12\text{m}$, transitions to alert chase, and executes scythe cleave in `ENEMY_STATE_ATTACK` within $2.0\text{m}$. Slain in 1 hit (+250 pts).
  - **Initial Slime (2 Hits)**: Spawns randomly along ground platforms. Hops towards player; slain in 2 hits.
  - **Transformation Metamorphosis**: Defeating the initial slime triggers `ENEMY_STATE_TRANSFORM` (3.2-second transformation delay, playing 32-frame slime metamorphosis animation and synthesized demonic roars).
  - **Boss Demon Slime (5 Hits)**: Giant $8.5\times 8.5\text{m}$ boss entity. Deals contact proximity damage ($<2.5\text{m}$) and heavy cleave damage ($4.8\text{m}$). Slain in 5 hits (+1000 pts).
  - **Boss Avoidance & Spatial Despawning**: If player runs in the opposite direction ($>28\text{m}$) or executes a Shadow Dash through the boss ($>8\text{m}$ clear), the boss retreats and despawns cleanly without game-over penalty.
- **Overhead Segmented Health Bars**:
  - Dynamically rendered floating world-space health bars over active enemies via `UIManager::renderEnemyHealthBars`. Calculated integer division tick marks displaying remaining hits (1 for BOD, 2 for Slime, 5 for Boss) with thematic color codings (Violet for BOD, Emerald for Slime, Crimson for Boss Demon).
- **Procedural Audio Synthesis**:
  - Generated 8 synthetic procedural waveforms directly in `Audio.cpp`: `SFX_SLASH`, `SFX_ENEMY_HIT`, `SFX_ENEMY_ATTACK`, `SFX_ENEMY_DEATH`, `SFX_SLIME_BOUNCE`, `SFX_BOSS_ROAR`, `SFX_BOSS_ATTACK`, `SFX_HEART_PICKUP`.

---

### v5.1 - Standardize Standalone Executable to TheWenderer.exe and Eliminate Redundant main.exe
#### 1. Root-Cause Analysis
- **Origin of `main.exe`**:
  - The default compiler target in the [`makefile`](/makefile) outputs directly to `./build/main.exe`.
  - When the automated `package` target was originally created, its PowerShell command executed:
    `Copy-Item build/main.exe dist/TheWenderer/`
    This created an executable named `main.exe` inside the distribution folder.
- **Origin of `TheWenderer.exe`**:
  - To give the game a player-facing branding, the executable in [`TheWenderer/`](/TheWenderer/) was renamed to `TheWenderer.exe`.
  - However, the directory [`TheWenderer/`](/TheWenderer/) also retained an older unversioned copy of `main.exe`.
  - When subsequent manual packaging or copying steps ran, both files remained in the directory and were compressed together into [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
- **Functional Duplication**:
  - Both binaries were identical builds of the same source code (`src/main.cpp`). Having both inflated the zip archive by ~3.3 MB and caused user confusion over which file to launch.

#### 2. Resolution Strategy
1. **Purged Redundant Binary**: Removed `TheWenderer/main.exe`.
2. **Updated Makefile Packaging Pipeline**: Modified the `package` recipe in [`makefile`](/makefile) so `build/main.exe` is explicitly copied as `TheWenderer.exe`:
   ```makefile
   Copy-Item build/main.exe dist/TheWenderer/TheWenderer.exe -Force;
   Copy-Item build/main.exe TheWenderer/TheWenderer.exe -Force;
   ```
3. **Rebuilt Standalone Archive**: Ran `mingw32-make package` to regenerate [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
4. **Verification**: Verified zip archive table of contents via Python script, confirming that only a single executable `TheWenderer/TheWenderer.exe` exists in the standalone archive.

---

### v5.2 - Modular Decomposition of Main Engine into UIManager, LadderScene, and SkyFallScene
#### 1. Problem Decomposition & Architectural Analysis
[`src/main.cpp`](/src/main.cpp) had accumulated over 3,070 lines of code (~162 KB) due to the inlining of multiple distinct subsystems:
1. **User Interface, HUD & Modals Domain**:
   - World-space floating texts (`spawnFloatingText`, updates, text rendering).
   - Dynamic corner notification card (`showCornerNotification`, timer, icon rendering).
   - Glassmorphic top-right score HUD (total score calculation, distance, coins, shields, lives, and pulsating icons).
   - Start hint banner, mini-game 15s survival countdown badge and pulsing "Exit Appeared" announcement.
   - Pause menu modal dialog (`STATE_PAUSED`), game over modal (`STATE_GAME_OVER`), and post-intro tutorial window (`showTutorialModal`).
2. **Endless 3-Track Ladder Climbing Mini-Game Domain**:
   - 3-lane horizontal track hopping physics (`A`/`D` leap interpolation).
   - Procedural generation of ladder rungs, traps (sawblades, spikes), coins, shields, and direction exit gates.
   - Collision detection against traps, single-hit shield deflection, death falling trajectory, reverse resurrection animation spotlight sequence, and comprehensive rendering.
3. **High-Velocity Sky-Fall Diving Mini-Game Domain**:
   - Aerial steering physics, horizontal terminal clamping, and velocity damping.
   - Procedural aerial generation for flapping birds, floating spike mines, coins, and exit arrows.
   - Bird and mine hazard collisions, 90-degree CCW impact rotation animation, dark screen fading, and celestial starfield rendering.

#### 2. Subsystem Modularization Strategy
To decouple `main.cpp` according to existing conventions seen in [`src/Renderer.cpp`](/src/Renderer.cpp), [`src/Audio.cpp`](/src/Audio.cpp), and [`src/World.cpp`](/src/World.cpp):
- **[`include/UIManager.h`](/include/UIManager.h) & [`src/UIManager.cpp`](/src/UIManager.cpp)**:
  - Encapsulated UI state into a thread-safe singleton `UIManager::instance()`.
  - Exported inline convenience wrappers (`spawnFloatingText` and `showCornerNotification`) for seamless backwards compatibility across all modules.
  - Implemented modular methods for floating texts, corner alerts, score HUD, trial countdown badge, start banner, overhead enemy health bars, pause menu, game over screen, and tutorial modal.
- **[`include/LadderScene.h`](/include/LadderScene.h) & [`src/LadderScene.cpp`](/src/LadderScene.cpp)**:
  - Encapsulated ladder climb state, lanes, leaping interpolation, procedural ladder generation, trap/item collisions, death fall, and reverse resurrection in class `LadderScene`.
  - Exposed global instance `gameLadderScene`.
- **[`include/SkyFallScene.h`](/include/SkyFallScene.h) & [`src/SkyFallScene.cpp`](/src/SkyFallScene.cpp)**:
  - Encapsulated freefall physics, steering, bird flight, spike mines, exit gate generation, and 90-degree CCW take-hit rotation in class `SkyFallScene`.
  - Exposed global instance `gameSkyFallScene`.
- **Shared Data Structures in [`include/Common.h`](/include/Common.h)**:
  - Relocated `CharacterAnimSet` and `extern CharacterAnimSet samuraiAnim;` declaration into `Common.h` so all scenes and managers can access character metadata without circular includes.
  - Placed `CelestialStar` struct definition in `SkyFallScene.h` and referenced it in `main.cpp`.

#### 3. Build & Compilation Verification
- Updated [`Makefile`](/Makefile) compilation targets (`win` and `linux`) to include `./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp`.
- Fixed duplicate struct definitions and missing scope references.
- Verified clean compilation with `g++.exe` (`-Wall -Wextra -O2 -static-libgcc -static-libstdc++`) with 0 errors and 0 warnings.
- Executed `mingw32-make package` to produce `TheWenderer_Standalone.zip`.

---

### v5.3 - Calibrate Enemy Proportions, Speeds, Platform Grounding, and Telegraphed Attack Wind-Ups
#### 1. Requirement & Distortion Decomposition
1. **Vertical Stretching of Bringer of Death**:
   - **Root Cause**: The Bringer of Death sprite sheets (`bod_walk.png`, `bod_idle.png`, `bod_attack.png`) are $140\times 93$ pixels per frame, establishing a native aspect ratio of $140 / 93 \approx 1.505$. In [`src/main.cpp`](/src/main.cpp), the enemy render quad was set to `glm::vec2(4.8f, 4.8f)` ($1:1$ square quad). Compressing a $1.505:1$ rectangular sprite onto a square quad compressed its horizontal width by ~33%, yielding extreme vertical elongation.
   - **Resolution**: Updated `eRenderSize` to `glm::vec2(4.5f, 3.0f)` ($4.5 / 3.0 = 1.50$, exact aspect ratio match).

2. **Under-Sized Slime Sprite**:
   - **Root Cause**: The Initial Slime sprite sheets (`slime_idle.png`, `slime_move.png`, `slime_transform.png`) are $576\times 320$ pixels per frame (aspect ratio $1.80$). The visible body occupies $432\times 244$ pixels. Rendering this onto a square quad `(2.4f, 2.4f)` severely squashed its width and, combined with a negative ground offset that sank it into the platform, made the visible creature tiny (~$0.7\text{m}$).
   - **Resolution**: Scaled `eRenderSize` to `glm::vec2(4.5f, 2.5f)` ($4.5 / 2.5 = 1.80$), yielding a natural visible height of $\approx 1.9\text{m}$ and width of $\approx 3.3\text{m}$.

3. **Boss Demon Slime Aspect Ratio**:
   - The Demon Boss sprite sheets (`demon_idle.png`, `demon_walk.png`, `demon_attack.png`) are $288\times 160$ pixels per frame (aspect ratio $1.80$). Setting `eRenderSize` to `glm::vec2(10.8f, 6.0f)` maintains its exact $1.80$ proportion, appearing colossal and menacing without distortion.

4. **Placement Between Platforms & Floating Over Chasms**:
   - **Root Causes**:
     1. In [`src/World.cpp`](/src/World.cpp), chunk enemy generation picked random coordinates without verifying `!isInsideChasm(candX)` or `!isUndergroundShaft(candX)`. Enemies were spawned floating over pits.
     2. Enemy quad centers were placed at `pos.y = SURFACE_GROUND_Y + 0.85f = 0.35f` with quad heights of $4.8\text{m}$, sinking the lower $2.0\text{m}$ into the ground.
     3. During gameplay in [`src/main.cpp`](/src/main.cpp), enemy movement lacked forward ledge/chasm collision checks; enemies walked off platform borders and floated in mid-air.
   - **Resolutions**:
     - Added [`World::hasSolidGroundAt(x, y, toleranceY)`](/src/World.cpp) to verify solid tile presence underneath entity feet.
     - Spawning logic checks candidate positions against chasms, shafts, and solid tiles with safety margins.
     - Adjusted center offsets so feet touch ground levels ($Y = 0.0\text{m}$ surface, $Y = -4.0\text{m}$ cavern): BOD at `y = 1.45f`, Slime at `y = 1.25f`, Boss at `y = 3.0f`.
     - Added ledge lookahead probe in the AI loop (`hasSolidGroundAt(nextX + dir * probeDist, footY, 0.4f)`): patrolling enemies reverse direction, and alert enemies halt at platform edges.

5. **Movement Speed Pacing**:
   - BOD patrol: reduced to $1.0\text{m/s}$, alert chase: $1.3\text{m/s}$ (down from $2.2\text{m/s}$).
   - Slime wander: reduced to $0.75\text{m/s}$, approach: $0.9\text{m/s}$ (down from $1.5\text{m/s}$).
   - Boss Demon Slime: reduced to $0.9\text{m/s}$ (down from $1.8\text{m/s}$).

6. **Telegraphed Attack Wind-Up Windows**:
   - **Framerate Glitch Fix**: In previous code, `enemy.animTimer` was multiplied by $10.0$ in accumulation and then multiplied by $10.0$ again when computing frames (`static_cast<int>(animTimer * 10.0f)`), resulting in 100 FPS (10x too fast), flashing the attack in $0.1\text{s}$. Standardized `enemy.animTimer` to real-world seconds (`+= deltaTime`).
   - **BOD Attack**: Pauses horizontal movement on trigger ($<2.4\text{m}$). Frames 0-5 at 8 fps ($0.75\text{s}$) display a raised scythe wind-up with an overhead alert `"[ ! ]"`. Cleave impact occurs strictly on frames 6-7 ($0.75\text{s}$ to $1.0\text{s}$), giving the player full dodge agency.
   - **Boss Demon Slime Attack**: Removed instantaneous proximity walking damage. Within $4.2\text{m}$, triggers telegraphed Cleave attack ($8\text{ fps}$). Frames 0-8 ($1.125\text{s}$) display a flame-gathering wind-up with overhead alert `"[ ! ] DODGE!"`. Ground slam shockwave damage occurs strictly on frames 9-11 ($1.125\text{s}$ to $1.5\text{s}$).

---

### v5.4 - Fix Bringer of Death Facing, Realistic Katana Slash SFX, Guaranteed Tutorial Window, and Idle Stillness on Restart
#### 1. Bringer of Death Facing Inversion (Moonwalking Elimination)
- **Root Cause**: An empirical pixel-sweep analysis of the sprite sheets (`bod_walk.png`, `bod_idle.png`, `bod_attack.png`) revealed that the character is drawn natively facing **LEFT** in the raw textures. (In frame 0, the body occupies $X \in [75, 128]$ and during the attack swing, the scythe cuts forward into $X \in [3, 75]$ to the left).
- In [`src/main.cpp`](/src/main.cpp), the shader UV mapping logic was modeled assuming a right-facing source texture:
  ```cpp
  glm::vec4 eUV = enemy.facingLeft ? glm::vec4(eu + edu, 0.0f, -edu, 1.0f) : glm::vec4(eu, 0.0f, edu, 1.0f);
  ```
  Consequently, when `facingLeft` was `true`, it applied `-edu` (flipping the sprite to face right), resulting in backward moonwalking. When `facingLeft` was `false`, it remained unflipped (facing left) while translating rightward.
- **Resolution**: Inverted the flip condition for enemy sprite sheets:
  ```cpp
  bool flipHorizontal = !enemy.facingLeft;
  glm::vec4 eUV = flipHorizontal ? glm::vec4(eu + edu, 0.0f, -edu, 1.0f) : glm::vec4(eu, 0.0f, edu, 1.0f);
  ```
  This renders the raw texture unflipped when moving left and horizontally mirrored when moving right, ensuring the enemy always faces its movement vector.

#### 2. Realistic Procedural Katana Sword Slash SFX
- **Synthesis Model**: Overhauled `SFX_SLASH` in [`src/Audio.cpp`](/src/Audio.cpp) to synthesize a sharp, resonant Japanese katana sword slash using zero-dependency procedural PCM WAV generation:
  1. **Aerodynamic Air Whip (Whoosh)**: Rapid frequency-modulated sweep descending exponentially from $2400\,\text{Hz}$ to $320\,\text{Hz}$ over $200\,\text{ms}$, blended with band-passed white noise ($65\%$ noise, $35\%$ tone).
  2. **Metallic Blade "Shing" Resonance**: High-frequency metallic harmonics at $3250\,\text{Hz}$, $4950\,\text{Hz}$, and $1920\,\text{Hz}$ with an exponential ring-down envelope ($e^{-15t}$).
  3. **Cutting Transient Bite**: Mid-frequency attack bite ($1350\,\text{Hz}$) localized between $t \in [30\,\text{ms}, 150\,\text{ms}]$ simulating the sharp slicing edge cutting through the air.

#### 3. Guaranteed Post-Intro Tutorial Modal Display
- **Root Cause**: When a player pressed or held `Space`, `Enter`, or `Esc` to finish or skip the intro video, the video player transitioned into `STATE_RUNNING` and set `showTutorialModal = true`. On the very next frame, `processInput()` detected `Space`/`Enter`/`Esc` was still depressed, and because `tutDismissLast` was uninitialized/false, it immediately triggered the dismissal branch and closed the modal before it could render a single frame.
- **Resolution**:
  - Moved `tutDismissLast` to file scope and set `tutDismissLast = true` upon video skip/completion, strictly preventing the key used during the video from dismissing the modal. The player must release and explicitly press a key to dismiss the tutorial window.
  - Frozen physics simulation (`deltaTime = 0.0f`) while `showTutorialModal` is active so no enemies or hazards advance while reading the tutorial.

#### 4. Grounded Motionless Idle on Tutorial Dismiss & Game Over Restart
- **Root Cause**: When restarting from Game Over or dismissing the tutorial with `Space`, the keypress bled into the subsequent `Jump` check, clearing `isGameIdleAtStart = false` and setting jump velocity, immediately launching the runner into forward sprint.
- **Resolution**:
  - In `resetGame()` and upon tutorial modal dismiss, enforced `isGameIdleAtStart = true` and zeroed velocity: `playerVel = glm::vec2(0.0f, 0.0f)`.
  - Added `gameOverRestartLast` debounce to absorb the restart keypress and returned immediately from `processInput()` on the restart frame.
  - Updated `Jump` input: if `isGameIdleAtStart` is active, pressing `Space` jumps vertically straight up in place without breaking idle stillness.
  - Preserved idle stillness until directional movement keys (`A`, `D`, `Left`, `Right`) are deliberately pressed, giving players full agency over when to commence running.

---

### v5.5 - Lock Boss Attack Direction and Directional Damage Sector
#### 1. Attack Direction Commitment & Mid-Swing Flip Suppression
- **Root Cause**: In [`src/main.cpp`](/src/main.cpp), `enemy.facingLeft` was evaluated unconditionally every frame using `enemy.facingLeft = (distX < 0.0f)`. If a player dashed, jumped, or moved behind the Boss Demon Slime (or Bringer of Death) while the enemy was in `ENEMY_STATE_ATTACK`, `distX` inverted sign, causing the boss to flip $180^\circ$ mid-swing.
- **Resolution**:
  - Gated facing updates behind state checks:
    ```cpp
    if (enemy.state != ENEMY_STATE_ATTACK) {
        enemy.facingLeft = (distX < 0.0f);
    }
    ```
  - Upon initiating an attack, explicitly locked the orientation:
    ```cpp
    enemy.facingLeft = (distX < 0.0f); // Committed attack orientation
    ```
  - While `enemy.state == ENEMY_STATE_ATTACK`, facing direction remains locked to that orientation until the attack concludes and transitions back to `ENEMY_STATE_IDLE`.

#### 2. Frontal Damage Sector Validation
- In addition to directional commitment, impact damage calculation now verifies that the player is actually located in front of the attacking enemy:
  ```cpp
  bool playerInFront = (enemy.facingLeft && distX <= 0.4f) || (!enemy.facingLeft && distX >= -0.4f);
  ```
  If the player dashes or vaults behind the boss during its telegraph wind-up ($1.125\text{s}$ window), the ground slam shockwave strikes harmlessly in the committed direction, rewarding skilled evasion.

---

### v5.6 - Updated Slime Animations and Boss Demon Slime Platform Jump Mechanics
#### 1. Root-Cause Analysis for the Three Issues

1. **Issue 1: Boss Floating in Mid-Air and Continual Upward Jumping**:
   - **Premature Gravity Cutoff**: In `ENEMY_STATE_JUMP`, horizontal and vertical physics were gated under `if (smashFrame >= 5 && smashFrame < 10)`. Frames 5 to 9 at 12 FPS only spanned $0.417\text{s}$. A jump launched with $v_y = 11.0\text{ m/s}$ requires $\approx 1.0\text{s}$ to arc and land under $g = -22.0\text{ m/s}^2$. At $t = 0.417\text{s}$, the boss was still near its apex ($2.67\text{m}$ high) when `smashFrame` hit 10, causing the engine to freeze `enemy.vel = (0, 0)` in mid-air!
   - **Absence of General Grounding**: When the jump animation finished, the boss returned to `ENEMY_STATE_PATROL` while still elevated. Because patrol only updated $x$, the boss stayed floating in the sky. When its foot raycast saw no ground under it, `jumpCooldown <= 0.0f` fired again, launching it from mid-air into an even higher altitude.
   - **Solution**:
     - Decoupled physical landing from raw frame timing: the boss remains in airborne flight phase (`stateTimer == 1.0f`) with active gravity ($g = -22.0\text{ m/s}^2$), cycling airborne poses (frames 6–8) until its feet physically touch a platform tile or ground surface ($v_y \le 0$).
     - On touchdown, feet snap to the platform top $+ 3.0\text{f}$, velocity zeroes, and it transitions to the ground slam impact (frames 10–17).
     - Added continuous platform gravity and ground snapping to patrol and idle states so the boss can never float.

2. **Issue 2: Slime Sprite Zooming (Lower-Middle Centering)**:
   - In the raw $576 \times 320$ GIF assets, the initial slime was only $\approx 49\times 59$ pixels, anchored in the bottom-middle ($X \in [256, 304], Y \in [258, 316]$). Rendered onto a full quad, the slime was tiny ($0.38\text{m}$).
   - **Solution**: Cropped each frame to an active bounding box of $160\times 160$ centered around $X = 280, Y = 160..320$. The slime body occupies $60\%$ of the frame. Rendered at `eRenderSize = glm::vec2(2.4f, 2.4f)` with foot alignment at `enemy.pos.y - 1.2f`, the slime is enlarged by 2.5x, clearly detailed, animated, and grounded.

3. **Issue 3: Slime Transformation Showing Pure Black Object**:
   - Generating 33 frames horizontally at full $576\times 320$ produced an image $19,008 \times 320$ pixels wide.
   - Hardware `GL_MAX_TEXTURE_SIZE` limits (8,192 or 16,384) caused `glTexImage2D` to fail with `GL_INVALID_VALUE`. An incomplete OpenGL texture renders as solid black.
   - **Solution**: Cropped the morphing region ($X \in [160, 416], Y \in [64, 320]$) and scaled to $160\times 160$ per cell. Total sheet width is $33 \times 160 = 5280$ pixels ($5280 < 8192$), safely within all GPU hardware limits. The transformation quad ($6.0\text{m}\times 6.0\text{m}$) is grounded at `footY + 3.0f`, morphing from slime to demon smoothly.

---

### v5.7 - Shift Control Remap, BOD Centering & Spell Attack, Spike Clusters, and Boss Transformation SFX
#### 1. Input Decoupling & Tutorial Modal Key Flash Investigation
- **Root Cause of Tutorial Vanishing**:
  In `src/main.cpp`, `GLFW_KEY_T` was shared between opening the tutorial modal from the Pause Menu and toggling Shift Mode. Crucially, in `processInput`, the modal dismissal check included `glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS` without guarding for an unreleased key from the opening frame. When `T` was pressed in the Pause Menu, `showTutorialModal = true;` was assigned. On the very next frame, `showTutorialModal` evaluated to `true`, and because the physical key `T` remained depressed, `dismissPressed && !tutDismissLast` immediately evaluated to `true`, closing the modal in ~16ms.
- **Resolution**:
  1. Decouple controls: Remap Shift Mode to `GLFW_KEY_Y` (`[Y]`). Dedicate `[T]` exclusively to opening the Tutorial Modal from both running and paused states.
  2. Guard against synchronous bleed-through: When `T` opens the modal, set `tutDismissLast = true`. The dismissal logic requires `!tutDismissLast`, ensuring the key must be completely released before any subsequent keypress can dismiss the window.
  3. Reflect `[Y] Shift Mode` across `renderPauseMenu` and `renderTutorialModal`.

#### 2. Bringer of Death Pixel Centering & Ranged Thunder Spell Attack
- **Pixel Bounding Box Calculation**:
  - Raw canvas: $140\times 93$ pixels ($W = 140, H = 93$). Canvas horizontal center = $70.0$.
  - Bounding box analysis across Idle, Walk, Hurt, and Cast:
    - `Idle`: $X \in [81, 127]$, center = $104.0$ ($\Delta X = +34.0\text{ px}$).
    - `Walk`: $X \in [74, 130]$, center = $102.0$ ($\Delta X = +32.0\text{ px}$).
    - `Cast`: $X \in [72, 125]$, center = $98.5$ ($\Delta X = +28.5\text{ px}$).
    - Average body center: $X \approx 103.5\text{ px}$.
    - Asymmetry ratio: $+33.5 / 140 = +0.2393$ of canvas width to the right.
    - World render quad size: $4.5\text{m} \times 3.0\text{m}$.
    - Displacement offset in world units: $0.2393 \times 4.5\text{m} = 1.077\text{m}$.
  - Corrective Translation:
    - When `enemy.facingLeft` (no UV flip): translate quad by `enemy.pos.x - 1.077f`.
    - When `!enemy.facingLeft` (horizontal UV flip): translate quad by `enemy.pos.x + 1.077f`.
    - Result: The visual torso of Bringer of Death aligns with the entity position and collision hitbox.
- **Ranged Thunder Spell Casting**:
  - Stitched `assets/enemies/bod_cast.png` (9 frames, $1260\times 93$) and `assets/enemies/bod_spell.png` (16 frames, $2240\times 93$).
  - Added `ENEMY_STATE_CAST` and `struct SpellHazard { glm::vec2 pos; float animTimer; bool hasDealtDamage; bool active; };`.
  - When the player is between $3.8\text{m}$ and $12.0\text{m}$ away, Bringer of Death locks facing direction and plays the 9-frame casting animation (1.0s).
  - At frame 4, a `SpellHazard` is summoned on the player's ground coordinate.
  - Spell animation lifecycle (16 frames at 12 FPS, 1.33s):
    - Frames 0–5: Thundercloud gathers above the target location (telegraph indicator).
    - Frames 6–11: Massive lightning bolt strikes down to ground level (active damage window).
    - Frames 12–15: Cloud dissipates into sparks.

#### 3. Grouped Platform Spikes & Spawn Density Amplification
- **Spike Grouping**:
  In `src/World.cpp`, elevated platforms now generate grouped clusters of 2 to 4 adjacent spikes (`SpikeTrap`) with randomized horizontal offsets, creating authentic platformer hazard corridors.
- **Multi-Enemy Density**:
  - Increased Bringer of Death spawns to 1–3 per chunk with randomized initial cooldowns.
  - Increased Initial Slime spawn chance to 60% with multi-slime support (1–2 slimes per chunk).

#### 4. Sequenced Slime Transformation Audio & Slower Animation
- **Windows Multimedia MCI Integration**:
  Leveraged `mciSendStringA` via `winmm.lib` to stream external MP3 files concurrently without external dependencies.
- **3-Phase Sequenced Audio**:
  1. $t = 0.0\text{s}$: `universfield-wet-splat-impact-567197.mp3` on slime defeat.
  2. $t = 0.85\text{s}$: `freesound_community-acid-bubbling-105397.mp3` during puddle bubbling.
  3. $t = 2.8\text{s}$: `audiopapkin-monster-roar-ps-o24-333057.mp3` as Demon Slime rises.
- **Animation Framerate Calibration**:
  Transformation duration extended to $5.5\text{s}$ (33 frames at 6.0 FPS), aligning visual sprite metamorphosis with the 3 audio events.
- **Boss Smash Impact Bang**:
  Replaced light bounce thump with `dragon-studio-distant-bang-472364.mp3` on `06_Smash.gif` touchdown.

---

### v5.8 - Delayed Multi-Layer Thunder Spell, BGM Integration, Free Exploration Default, and Enemy Separation
#### 1. Telegraphed Multi-Layer Spell Casting & Audio Synchronization
- **Multi-Elevation Platform Targeting ($|\Delta Y| < 14.0\text{m}$)**:
  - In previous versions, the Bringer of Death only targeted players when $|\Delta Y| < 2.8\text{m}$, restricting spell casting to entities on the same immediate platform.
  - In our 4-tier vertical world (Subterranean Cavern at $Y = -4.5\text{m}$, Surface Ground at $Y = 0.0\text{m}$, Intermediate Ledges at $Y = 2.0\text{m}$–$4.0\text{m}$, Sky Canopy at $Y = 6.0\text{m}$–$10.0\text{m}$), underground enemies could never target overground players.
  - By expanding vertical awareness to $|\Delta Y| < 14.0\text{m}$ and horizontal distance to $[3.0\text{m}, 14.0\text{m}]$, the Bringer of Death detects the player across any elevation tier and summons a localized thunder hazard at the player's ground level (`glm::vec2(playerPos.x, playerPos.y - 1.2f)`).
- **Three-Phase Spell Telegraph & Dodge Window**:
  - Previously, the spell struck at frame 6 ($t = 0.5\text{s}$), leaving little reaction time.
  - We structured the `SpellHazard` into 3 explicit timing phases:
    1. **Telegraph Warning Phase ($t \in [0.0\text{s}, 1.0\text{s}]$, frames 0 to 5)**: Cloud gathers overhead at 5.0 FPS with an on-screen warning: `"THUNDER CLOUD GATHERING! MOVE!"`. Players have 1.0 second to dash or jump away.
    2. **Impact Strike Phase ($t \in [1.0\text{s}, 1.5\text{s}]$, frames 6 to 11)**: Exactly at $t = 1.0\text{s}$, `audioMgr.playThunderSpell()` triggers [`assets/thunder-after-crackle.mp3`](/assets/thunder-after-crackle.mp3). The lightning strike impacts the ground; players within the $1.4\text{m}$ horizontal blast radius receive damage or shield break.
    3. **Dissipation Phase ($t \in [1.5\text{s}, 1.9\text{s}]$, frames 12 to 15)**: Electric sparks and cloud remnants fade out at 10.0 FPS. At $t \ge 1.9\text{s}$, `spell.active = false`.
- **Rendering UV Frame Mapping**:
  ```cpp
  int sFrame = (spell.animTimer < 1.0f) ? std::min(static_cast<int>(spell.animTimer * 5.0f), 5) :
               (spell.animTimer < 1.5f) ? (6 + std::min(static_cast<int>((spell.animTimer - 1.0f) * 12.0f), 5)) :
               (12 + std::min(static_cast<int>((spell.animTimer - 1.5f) * 10.0f), 3));
  ```

#### 2. Background Music (BGM) & Interactive Volume/Mute Controls
- **Windows MCI Streaming Engine**:
  - Utilized `mciSendStringA` via Windows Multimedia (`winmm.lib`) for zero-dependency streaming playback of [`assets/Quarter_in_the_Slot.mp3`](/assets/Quarter_in_the_Slot.mp3).
  - Open command: `open "assets/Quarter_in_the_Slot.mp3" type mpegvideo alias bgm`.
  - Volume calibration: Initialized at 50% volume using `setaudio bgm volume to 500` (range 0–1000).
  - Looping playback: Initiated on intro video completion, skip, or fallback via `play bgm repeat`.
- **Mute Toggle (`[M]` Key)**:
  - Toggling `audioMgr.toggleBgmMute()` alternates between `setaudio bgm volume to 0` and `setaudio bgm volume to 500`.
  - Dispatches corner HUD card notifications and floating text (`"MUSIC: MUTED"` / `"MUSIC: UNMUTED (50%)"`).
  - Integrated audio status readout into the Pause Menu modal (`renderPauseMenu`).

#### 3. Free Exploration Mode as Default Gameplay Experience
- **Inverted Default Stance**:
  - Initialized `bool freeExplorationMode = true;` at declaration and inside `resetGame()`.
  - Players now begin in manual platformer exploration mode (`A`/`D` movement, dynamic idle breathing stance when stationary, ladder position holding).
  - Continuous autonomous sprint runner mode is preserved as an optional mechanic toggled via `[F]`.
- **Tutorial & Pause Menu Clarification**:
  - Updated `renderTutorialModal` and `renderPauseMenu` in [`src/UIManager.cpp`](/src/UIManager.cpp) to explicitly state that Free Exploration is default and document all active controls (`A/D`, `W/Space`, `S`, `Shift`, `J/X/Click`, `F`, `M`, `Y`, `T`, `R`, `Q`).

#### 4. Enemy Spatial Separation Buffer
- **Distance Constraint Enforcement**:
  - In [`src/World.cpp`](/src/World.cpp), enforced a strict $12.0\text{m}$ horizontal separation buffer between Bringer of Death and Slime spawns:
    ```cpp
    bool tooCloseToSlime = false;
    for (const auto& existing : enemies) {
        if (existing.type == ENEMY_SLIME_INITIAL && std::abs(existing.pos.x - bodX) < 12.0f) {
            tooCloseToSlime = true;
            break;
        }
    }
    ```
  - Prevents overlapping enemy spawns, ensuring clear and readable combat encounters.

#### 5. Redundant Asset Cleanup & Git Tracking
- Removed obsolete directory `assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/No Effect Sprites`.
- Added exclusion rule to [`.gitignore`](/.gitignore):
  ```gitignore
  assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/No Effect Sprites/
  ```

---

### v5.9 - Synchronized Thunder Spell SFX and Right-Click Katana Defend Mechanic
#### 1. Audio Pre-Delay & Animation Synchronization Modeling
- **Waveform Inspection & Audio Profile**:
  - Analyzed `assets/thunder-after-crackle.mp3` through direct decoded buffer analysis.
  - The sample has an overall duration of $\sim 4.0\text{s}$ at $44.1\text{kHz}$ stereo.
  - Critically, the sound features a distinct two-part envelope:
    1. **Preamble / Crackle Window ($t \in [0.0\text{s}, 1.05\text{s}]$)**: Low-to-mid amplitude ambient electrical crackles and static discharge buzz ($A_{\text{norm}} \approx 0.12 - 0.28$).
    2. **Explosive Thunder Clap ($t \ge 1.05\text{s}$)**: Massive sudden bass spike and acoustic shockwave ($A_{\text{norm}} > 0.88$).
- **Visual Attack Timing of Bringer of Death**:
  - The Bringer of Death cast sequence consists of 9 distinct frames (`bod_cast.png`, $1260\times 93$, 9 frames of $140\times 93$ px).
  - At a playback rate of $7.5\text{ FPS}$, the cast animation runs for:
    $$T_{\text{cast}} = \frac{9\text{ frames}}{7.5\text{ fps}} = 1.20\text{ seconds}$$
  - The lethal thundercloud spell is spawned on the ground right at frame 7 ($t = 7 / 7.5 \approx 0.933\text{s}$).
- **Synchronization Solution**:
  - Previously, `audioMgr.playThunderSpell()` was called when the cloud hit its active phase ($1.0\text{s}$ after cloud spawn), causing the loud thunderclap to occur far too late ($\sim 2.0\text{s}$ after cast start).
  - By moving the audio invocation to the exact moment Bringer of Death switches into `ENEMY_STATE_CAST`, the $1.0\text{s}$ crackling channels while the wraith is raising its scythe and gathering energy. Consequently, the explosive thunder strike lands precisely at $t \approx 1.0\text{s}$ when the lethal ground cloud manifests at the player's ground position.

#### 2. Player Defend / Parry Mechanics & Spritesheet Pipeline
- **Spritesheet Format Inspection**:
  - Inspected `assets/samurai/Defend_nobg.png` ($1600\times 200$ pixels).
  - The sheet contains 8 frames, each precisely $200\times 200$ pixels, matching Samurai Mack's standard `Idle.png`, `Run.png`, and `Attack1.png`.
  - Added `unsigned int texDefend = 0;` and `int defendFrames = 8;` to `CharacterAnimSet` in [`include/Common.h`](/include/Common.h).
- **Guard State Machine (`isBlocking`, `blockTimer`, `blockCooldown`)**:
  - **Input Activation**: Triggered by **Right Mouse Click** (`GLFW_MOUSE_BUTTON_RIGHT`) or the fallback `K` key.
  - **Pre-Conditions**: Can only be engaged if `!isBlocking`, `blockCooldown <= 0.0f`, `playerLives > 0`, and not climbing ladders.
  - **Guard Window ($1.0\text{s}$)**:
    - While guarding, `isBlocking = true` and `blockTimer` counts up to $1.0\text{s}$.
    - Horizontal exploration velocity is zeroed (`heroVelocity.x = 0.0f`), anchoring the samurai in place during the guard stance.
    - Animation renders `samuraiAnim.texDefend` at $8.0\text{ FPS}$ ($8\text{ frames} / 8.0\text{ fps} = 1.0\text{s}$), cycling through the deflection stance.
    - If no attack hits within $1.0\text{s}$, the guard terminates cleanly with zero cooldown penalties (`isBlocking = false`).
  - **Combat Interception & Damage Negation**:
    - Guard absorption was integrated across all damage pathways:
      1. Slime melee collision
      2. Bringer of Death scythe cleave
      3. Bringer of Death telegraphed thundercloud spell
      4. Demon Slime Boss contact charge
      5. Demon Slime Boss cleave swing
      6. Demon Slime Boss platform jump ground smash
    - When an attack arrives while `isBlocking == true`:
      - Player lives are not deducted (`playerLives` remains untouched).
      - Triggers custom procedural parry clang SFX: `SFX_BLOCK` (sharp initial steel strike combined with dual resonant harmonic frequencies at 2850 Hz and 4150 Hz).
      - Spawns cyan `"[PARRY]"` floating combat text above the player.
      - Confers a brief $0.6\text{s}$ grace invulnerability window (`postHitInvulnTimer = 0.6f`).
      - Immediately cancels active guard (`isBlocking = false`) and enforces the mandatory $2.0\text{s}$ cooldown (`blockCooldown = 2.0f`).
- **HUD & Visual Telemetry**:
  - Enhanced [`src/UIManager.cpp`](/src/UIManager.cpp) to render dynamic guard status in the HUD:
    - Cyan `[GUARD: ACTIVE]` while blocking.
    - Crimson `[DEF: X.Xs]` while cooling down.
  - Updated Tutorial modal window and Pause Menu with complete explanations of the new Defend / Parry mechanics and controls.

---

### v5.10 - Documentation and Academic Guide Synchronization with Modern Subsystems
#### 1. Academic Guide (`doc/explain.md`) Overhaul
- **Dedicated Section on 2D Sprites in Modern OpenGL**:
  - Addressed the fundamental Computer Graphics question: *"How does a 3D graphics API (OpenGL) render 2D pixel-art characters and animations?"*
  - Detailed the full pipeline:
    1. **Textured Quads**: Decoupling complex geometry into a single $1\times 1$ unit square (2 triangles, 6 vertices) resident in GPU VRAM (VAO/VBO).
    2. **UV Coordinate Space**: Explanation of normalized texture coordinates ($U \in [0, 1], V \in [0, 1]$).
    3. **Spritesheets & Frame Slicing**: Mathematical formula for extracting frame $i$ out of $N$ frames: $u_0 = i/N, u_1 = (i+1)/N$.
    4. **Horizontal Mirroring / Flipping**: Inverting horizontal UV bounds ($U_{\text{left}} = u_1, U_{\text{right}} = u_0$) when facing left versus right to avoid duplicate asset overhead.
    5. **Alpha Discard in Fragment Shaders**: Explanation of `if (texColor.a < 0.1) discard;` to eliminate unsightly square bounding boxes around transparent pixel art.
    6. **Model-View-Projection Matrix Transformation**: Preserving pixel aspect ratios across widescreen monitors using orthographic projections.
- **Architectural Refresh (8 Modular Blocks)**:
  - Updated legacy 5-block model to the complete 8-block modular architecture (`main.cpp`, `Common.h`, `Renderer.cpp`, `World.cpp`, `Audio.cpp`, `UIManager.cpp`, `LadderScene.cpp`, `SkyFallScene.cpp`, `VideoPlayer.cpp`).
- **Gameplay & Mechanics Explanations**:
  - Detailed Melee Katana Attack hitbox sweeps and active frames.
  - Detailed Katana Defend / Block stance ($1.0\text{s}$ duration, complete damage negation, metallic parry SFX, and $2.0\text{s}$ cooldown).
  - Detailed enemy state machines, Bringer of Death telegraphed spells, Slime metamorphosis, and Boss Demon Slime platform jumping.
  - Expanded Viva Examination Q&A with 6 core evaluation questions and answers.

#### 2. System Documentation (`doc/documentation.md`) Synchronization
- **Subsystem Technical Breakdown**:
  - Synchronized with [`/doc/changelog.md`](/doc/changelog.md) and [`/doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md).
  - Documented calibrated platformer physics constants ($g = -22.0\,\text{m/s}^2, v_y = 11.2\,\text{m/s}, v_x = 4.8\,\text{m/s}$).
  - Documented the Hybrid Audio Engine: in-memory procedural WinMM PCM synthesizer (`SFX_SLASH`, `SFX_BLOCK`, `SFX_ENEMY_HIT`, etc.) and Windows MCI hardware-accelerated streaming for background music (`Quarter_in_the_Slot.mp3`) with `[M]` mute toggle.
  - Documented combat hitbox math, front-facing sector validation, right-click defend parry window, and enemy finite state machines.
  - Documented endless world procedural streaming, chunk pruning ($85\text{m}$ buffer), grouped spike clusters, and enemy separation ($12\text{m}$ buffer).
  - Updated compilation flags to include Microsoft Media Foundation libraries (`-lmfplat -lmfreadwrite -lmfuuid -lole32`).

#### 3. README Synchronization (`README.md`)
- Updated direct `g++` compilation command to include all modular sources (`UIManager.cpp`, `LadderScene.cpp`, `SkyFallScene.cpp`).
- Updated shield pickup description to highlight the dedicated heraldic shield icon (`assets/shield_pickup.png`).

---

### v5.11 - Mid-Animation Death Spell Damage and Visual Telegraph Window
#### 1. Root Cause Analysis of Instantaneous Spell Damage
- **Inspection of Animation Frames**:
  - Analyzed the 16 individual animation frames of `assets/enemies/bod_spell.png` ($2240\times 93$, 16 frames of $140\times 93\text{px}$):
    - **Frames 0–4 ($Y \in [35, 53]$, height $\approx 18\text{px}$)**: Overhead cloud formation and dark swirling mist. No lightning extends towards the ground.
    - **Frames 5–6 ($Y \in [35, 70]$, height $\approx 35\text{px}$)**: Initial downward electrical leader channel.
    - **Frames 7–10 ($Y \in [35, 91]$, height $\approx 56\text{px}$, pixel count $>1000$)**: Full ground-touching lightning discharge column slamming through the ground plane ($Y = 91$).
    - **Frames 11–15 ($Y \in [35, 53]$, height $\approx 18\text{px}$)**: Dissipation phase, ground blast fades, and residual overhead sparks dissipate.
- **Flawed Damage Trigger in Prior Code**:
  - In `src/main.cpp`, the damage condition was previously:
    ```cpp
    if (spell.animTimer <= 0.50f && !spell.hasDealtDamage && !isDashing && postHitInvulnTimer <= 0.0f)
    ```
  - Because `spell.pos` is anchored to `playerPos` when spawned, on the very first frame ($t = 0.0\text{s}$), `sDistX < 1.4f` and `sDistY < 2.2f` evaluated to `true` instantly!
  - As a result, the player was struck the millisecond the cloud appeared, making evasion impossible without pre-existing sprint momentum.

#### 2. Mid-Animation Strike Synchronization
- **3-Phase Spell Hazard State Lifecycle**:
  1. **Phase 1: Visual Telegraph / Gathering ($t \in [0.0\text{s}, 0.58\text{s}]$, frames 0–6 at 12 FPS)**:
     - The dark storm cloud gathers overhead at the player's ground coordinates.
     - Spawns an amber-gold warning floating text: `"THUNDER CLOUD GATHERING! MOVE!"`.
     - Zero damage is evaluated (`sFrame < 7`), providing the player a full $580\text{ms}$ reaction window to run, jump, dash away, or press `Right Click` to raise their katana in defensive guard stance.
  2. **Phase 2: Mid-Animation Thunder Strike ($t \in [0.58\text{s}, 0.85\text{s}]$, frames 7–10 at 12 FPS)**:
     - The lightning bolt visibly impacts the ground plane.
     - Damage is strictly evaluated if the player is within $sDistX < 1.4\text{m}$ and $sDistY < 2.2\text{m}$:
       - If `isBlocking`: parries attack with `SFX_BLOCK`, spawns `[PARRY]` text, sets $2.0\text{s}$ cooldown, and absorbs all damage.
       - If `shieldTimer > 0.0f`: breaks shield and grants invulnerability.
       - Otherwise: deducts 1 player life pip and grants $1.25\text{s}$ grace invulnerability.
     - Single-strike flag `spell.hasDealtDamage = true;` ensures damage is registered exactly once.
  3. **Phase 3: Dissipation ($t \in [0.85\text{s}, 1.35\text{s}]$, frames 11–15)**:
     - Harmless smoke and sparks fade out before `spell.active = false`.
- **Audio Alignment**:
  - `audioMgr.playThunderSpell()` channels initial crackling during Bringer of Death's $1.2\text{s}$ cast animation, and the loud explosive thunder clap lands right as the lightning hits the ground at mid-animation.

---

### v5.12 - Increase Slime Enemy Spawn Frequency and Multi-Elevation Density
#### 1. Problem Diagnosis & Mathematical Spawning Constraints
- **Root Cause of Low Slime Density**:
  - In [`/src/World.cpp`](/src/World.cpp), initial slime spawning previously used a 60% probabilistic roll per 35-meter chunk (`std::rand() % 10 < 6`), producing only 1 or 2 slimes when successful.
  - Initial spawn chunks ($x \in [-24.0, +24.0]$) explicitly omitted slime spawns, forcing players to sprint $>35\,\text{m}$ into procedurally generated chunks before encountering their first combat engagement.
  - Furthermore, Bringer of Death and Slime separation previously checked horizontal distance unconditionally (`dx < 12.0f`). Because a Bringer of Death occupied a $24.0\,\text{m}$ horizontal exclusion footprint within a $35.0\,\text{m}$ chunk, candidate search attempts for slimes frequently collided with the exclusion zone, exhausting the 12 search attempts and resulting in empty chunks.
  - Slimes were also strictly constrained to surface ground level (`SURFACE_GROUND_Y + 0.5f`), leaving the underground cavern network ($Y = -4.5\,\text{m}$) completely devoid of slime encounters.

#### 2. Engineering Solution & Parameter Tuning
- **Guaranteed Starter Slimes in Spawn Chunk**:
  - In `World::initWorld()` / initial chunk generation, directly spawn two initial slime enemies:
    1. Right flank: $X = +17.5\,\text{m}$ on surface ground level ($Y = \text{SURFACE\_GROUND\_Y} + 0.5\text{m} + 0.7\text{m} = 0.7\text{m}$), facing left with $v_x = -0.75\,\text{m/s}$.
    2. Left flank: $X = -18.0\,\text{m}$ on surface ground level ($Y = 0.7\text{m}$), facing right with $v_x = +0.75\,\text{m/s}$.
  - This provides immediate, accessible combat encounters within 3–4 seconds of running in either direction.

- **100% Guaranteed Procedural Chunk Spawning**:
  - Replaced the `std::rand() % 10 < 6` conditional with guaranteed execution in every generated chunk.

- **Increased Pack Density (2 to 4 Slimes per Chunk)**:
  - Scaled procedural count to:
    $$\text{numSlimes} = 2 + (\text{rand}() \pmod 3) \implies \{2, 3, 4\}$$
    effectively doubling the average enemy presence per 35-meter runway.

- **Multi-Elevation Cavern Spawning**:
  - Added a 33% probability for each slime candidate to spawn within the subterranean cavern zone:
    $$\text{footY} = \begin{cases} \text{CAVERN\_FLOOR\_Y} + 0.5\,\text{m} & (\text{rand}() \pmod 3 == 0) \\ \text{SURFACE\_GROUND\_Y} + 0.5\,\text{m} & \text{otherwise} \end{cases}$$
  - Adjusted chasm avoidance to only apply to surface spawns (`!inCavern`), populating underground corridors with hopping slimes while respecting shaft boundaries.

- **Vertical-Aware Separation Buffers**:
  - Refined the distance check between Bringer of Death and Slimes:
    $$\text{Separation Buffer} = \begin{cases} 9.0\,\text{m} & \text{if } \Delta y < 2.5\,\text{m} \\ 0.0\,\text{m} & \text{if } \Delta y \ge 2.5\,\text{m} \end{cases}$$
  - When a Bringer of Death is on the surface ($Y \approx 0.0\,\text{m}$) and a Slime is in the cavern ($Y \approx -4.0\,\text{m}$, $\Delta y \approx 4.0\,\text{m}$), they no longer exclude one another horizontally, allowing subterranean slimes to spawn directly beneath surface wraiths.
  - Slime-to-slime horizontal spacing was reduced from $6.0\,\text{m}$ to $4.0\,\text{m}$ (`dy < 2.0f`).
  - Candidate placement search attempts were doubled from 12 to 24 per slime to ensure high placement success rates.

---

### v5.13 - Scale Down Player Defend Animation to Match Idle Spritesheet Proportions
#### 1. Empirical Pixel Coordinate Analysis & Root Cause Identification
- **Standard Samurai Mack Reference Frame**:
  - Across [`/assets/samurai/Idle.png`](/assets/samurai/Idle.png), [`Run.png`](/assets/samurai/Run.png), and [`Attack1.png`](/assets/samurai/Attack1.png), each frame is $200 \times 200\,\text{px}$.
  - In [`Idle.png`](/assets/samurai/Idle.png):
    - Non-transparent character bounds: $X \in [76, 112]$ ($\Delta X = 37\,\text{px}$, $\text{CenterX} = 94.0\,\text{px}$).
    - Vertical bounds: $Y \in [70, 121]$ ($\Delta Y = 52\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
    - The character's feet are anchored consistently at $Y = 121\,\text{px}$ across all 8 frames, and the baseline body height is $51$–$52\,\text{px}$.
  - In [`Run.png`](/assets/samurai/Run.png): $X \in [72, 117]$ ($\Delta X = 46\,\text{px}$), $Y \in [74, 121]$ ($\Delta Y = 48\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
  - In [`Attack1.png`](/assets/samurai/Attack1.png): $X \in [69, 111]$, $Y \in [69, 121]$ ($\Delta Y = 53\,\text{px}$, $\text{BottomY} = 121\,\text{px}$ in wind-up; raised sword reaches $Y = 53\,\text{px}$, $\Delta Y = 69\,\text{px}$).

- **Defend Animation Discrepancy in `Defend_nobg.png`**:
  - In the original [`Defend_nobg.png`](/assets/samurai/Defend_nobg.png):
    - Frame 0 bounds: $X \in [54, 155]$ ($\Delta X = 102\,\text{px}$, $\text{CenterX} = 104.5\,\text{px}$), $Y \in [41, 194]$ ($\Delta Y = 154\,\text{px}$, $\text{BottomY} = 194\,\text{px}$).
    - Frame 2 (overhead raised blade): $Y \in [5, 194]$ ($\Delta Y = 190\,\text{px}$, $\text{BottomY} = 194\,\text{px}$).
  - **The Defend character was drawn $3.0\times$ larger** ($154\,\text{px}$ vs $52\,\text{px}$) than the standard Samurai Mack sprite, with feet situated $73\,\text{px}$ lower ($Y = 194\,\text{px}$ instead of $Y = 121\,\text{px}$).
  - When rendered in-engine with the uniform model matrix scaling (`playerRenderSize = (5.5f, 5.5f)`), the player abruptly tripled in scale upon activating guard (`RMB`/`K`) and visually sank below the platform floor.

#### 2. Mathematical Modeling & Resampling Transformation
- **Scale Factor Formulation**:
  $$S = \frac{\text{Height}_{\text{Idle}}}{\text{Height}_{\text{Defend, Frame 0}}} = \frac{52.0\,\text{px}}{154.0\,\text{px}} \approx 0.3376623$$
  Applying this scale factor to the defend stance produces:
  - Base guard height: $154\,\text{px} \times S = 52.0\,\text{px}$ (exact match with [`Idle.png`](/assets/samurai/Idle.png)).
  - Base guard width: $102\,\text{px} \times S = 34.4\,\text{px}$ (compact braced stance, matching idle width $37\,\text{px}$).
  - Raised blade height: $190\,\text{px} \times S = 64.15\,\text{px}$ (matches [`Attack1.png`](/assets/samurai/Attack1.png) sword height of $69\,\text{px}$).

- **Pivot & Ground Alignment Mapping**:
  - Source pivot point: $(X_{\text{src}}, Y_{\text{src}}) = (98.0, 194.0)$ (center of feet).
  - Destination pivot point: $(X_{\text{dst}}, Y_{\text{dst}}) = (94.0, 121.0)$ (Idle feet ground anchor).
  - Inverse coordinate mapping for each destination pixel $(dx, dy)$ within each $200 \times 200$ frame:
    $$sx = X_{\text{src}} + \frac{dx - X_{\text{dst}}}{S} = 98.0 + \frac{dx - 94.0}{S}$$
    $$sy = Y_{\text{src}} + \frac{dy - Y_{\text{dst}}}{S} = 194.0 + \frac{dy - 121.0}{S}$$

- **High-Fidelity 64-Sample Box Supersampling**:
  - To prevent moiré fringing, pixel-art aliasing, or dark edge halos, each output pixel was evaluated with $8 \times 8 = 64$ sub-pixel samples.
  - Colors were premultiplied by source alpha during accumulation:
    $$(R, G, B)_{\text{acc}} = \sum (R_i, G_i, B_i) \cdot A_i, \quad A_{\text{acc}} = \sum A_i$$
  - Normalized colors $(R, G, B)_{\text{out}} = (R, G, B)_{\text{acc}} / A_{\text{acc}}$ and applied an alpha boost ($1.35\times$) with thresholding ($A > 0.08$) to preserve a crisp pixel-art silhouette against OpenGL's fragment shader alpha discard (`texColor.a < 0.12`).

#### 3. Verification & Engine Uniformity
- Post-resampling coordinate verification on the generated [`Defend_nobg.png`](/assets/samurai/Defend_nobg.png):
  - Frame 0: $X \in [79, 113]$ ($\Delta X = 35\,\text{px}$), $Y \in [69, 121]$ ($\Delta Y = 53\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
  - Frame 1: $X \in [74, 114]$ ($\Delta X = 41\,\text{px}$), $Y \in [75, 121]$ ($\Delta Y = 47\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
  - Frame 2: $X \in [75, 115]$ ($\Delta X = 41\,\text{px}$), $Y \in [57, 121]$ ($\Delta Y = 65\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
  - Frame 7: $X \in [74, 114]$ ($\Delta X = 41\,\text{px}$), $Y \in [69, 121]$ ($\Delta Y = 53\,\text{px}$, $\text{BottomY} = 121\,\text{px}$).
  - $\text{BottomY} = 121\,\text{px}$ across all 8 frames, $\text{CenterX} \in [93, 96]$ (mean $94.0\,\text{px}$).
- Because the spritesheet itself now matches the coordinate system of all other Samurai Mack spritesheets, [`src/main.cpp`](/src/main.cpp) uses the standard `playerRenderSize = (5.5f, 5.5f)` and translation `playerPos.y - 0.10f` without requiring custom branch scaling or offsets.

---

### v5.14 - Slime Ground Placement & AI Unfreezing, and Boss Demon Slime Dying Roar Audio Integration
#### 1. Mathematical Diagnosis of Slime Underground Embedding & Frozen Movement
- **Quad Coordinate & Vertex Layout**:
  - In [`/src/Renderer.cpp`](/src/Renderer.cpp), the unified quad geometry has vertical vertices spanning $Y \in [-0.5, +0.5]$ with texture coordinates $V \in [1.0, 0.0]$.
  - In [`/src/main.cpp`](/src/main.cpp), initial slime enemies are rendered with quad dimensions `eRenderSize = (2.4f, 2.4f)`.
  - In [`/assets/enemies/slime_move.png`](/assets/enemies/slime_move.png), the actual sprite artwork sits in the bottom portion of each $160\,\text{px}$ frame ($Y \in [104, 157]$), meaning the slime's base rests essentially at $V \approx 0.98 \approx 1.0$, which maps to the bottom edge of the render quad:
    $$\text{Slime Base } Y = \text{enemy.pos.y} - 0.5 \times 2.4\,\text{m} = \text{enemy.pos.y} - 1.2\,\text{m}$$
- **The Spawning Coordinate Discrepancy**:
  - In [`/src/World.cpp`](/src/World.cpp), starter and procedural slime spawns were previously defined as:
    $$\text{spawnY} = \text{footY} + 0.7\,\text{m}$$
  - For surface ground tiles ($Y_{\text{tile}} = -0.5\,\text{m}$, top edge at $Y = 0.0\,\text{m}$), $\text{footY} = 0.0\,\text{m}$, so $\text{spawnY} = 0.7\,\text{m}$.
  - The slime's base was rendered at:
    $$Y = 0.7\,\text{m} - 1.2\,\text{m} = \mathbf{-0.5\,\text{m}}$$
  - Because the surface ground is at $Y = 0.0\,\text{m}$, the slime was embedded exactly $0.5\,\text{m}$ (one full $1.0\,\text{m}$ tile tier) below the walking surface into the dirt.
- **The AI Raycast Freeze Deadlock**:
  - In [`/src/main.cpp`](/src/main.cpp), the slime movement update calculated foot elevation as:
    $$\text{footY} = \text{enemy.pos.y} - 1.2\,\text{m} = 0.7\,\text{m} - 1.2\,\text{m} = -0.5\,\text{m}$$
  - The ledge detection function `hasSolidGroundAt(nextX, footY, 0.4f)` compares `footY` against `tileTop = 0.0f`:
    $$|\text{tileTop} - \text{footY}| = |0.0\,\text{m} - (-0.5\,\text{m})| = 0.5\,\text{m} > 0.4\,\text{m} \implies \text{FALSE}$$
  - Because `hasSolidGroundAt` failed on every single frame, the slime's AI believed it was suspended over an abyss with no ground, immediately forcing `enemy.state = ENEMY_STATE_IDLE` or endlessly flipping facing direction back and forth every frame, leaving it completely stuck.
- **Patrol Orientation Overwrite Fix**:
  - In [`/src/main.cpp`](/src/main.cpp), `enemy.facingLeft = (distX < 0.0f)` was placed outside the distance check, overwriting `facingLeft` every frame even during passive patrol ($>8\,\text{m}$ away). Moving it inside `if (absDist < 8.0f && absY < 2.5f)` enables free autonomous patrolling where slimes reverse at ledges without orientation snapping.

#### 2. Spawning & Movement Resolution
- Updated initial chunk starter slimes in [`/src/World.cpp`](/src/World.cpp):
  $$\text{slime.pos} = (candX, \; \text{SURFACE\_GROUND\_Y} + 0.5\,\text{m} + 1.2\,\text{m}) = (candX, \; 1.2\,\text{m})$$
- Updated procedural chunk generation in [`/src/World.cpp`](/src/World.cpp):
  $$\text{spawnY} = \text{footY} + 1.2\,\text{m}$$
  - Surface ground: $\text{footY} = 0.0\,\text{m} \implies \text{spawnY} = 1.2\,\text{m}$ (bottom rests flush at $0.0\,\text{m}$).
  - Cavern floor: $\text{footY} = -4.0\,\text{m} \implies \text{spawnY} = -2.8\,\text{m}$ (bottom rests flush at $-4.0\,\text{m}$).
- Now `footY = enemy.pos.y - 1.2f` equals $0.0\,\text{m}$ (surface) and $-4.0\,\text{m}$ (caverns), perfectly matching `tileTop` ($|\Delta Y| = 0.0\,\text{m} \le 0.4\,\text{m}$). `hasSolidGroundAt` returns `true`, and slimes freely hop and pursue the player.

#### 3. Boss Demon Slime Dying Roar Audio Integration
- **Asset Analysis**:
  - File: [`/assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) (174,612 bytes).
  - Target Animation: [`/assets/enemies/demon_death.png`](/assets/enemies/demon_death.png) (derived from `05_d_death.gif`, 22 frames).
- **MCI Playback Subsystem**:
  - Added `AudioManager::playBossDyingRoar()` in [`/include/Audio.h`](/include/Audio.h) and [`/src/Audio.cpp`](/src/Audio.cpp).
  - Opens and plays `assets/dyingRoar.mp3` with alias `sfx_dying_roar` asynchronously via `mciSendStringA`.
  - Added cleanup logic to `AudioManager::~AudioManager()` and `stopTransformationAudio()`.
  - In [`/src/main.cpp`](/src/main.cpp), when `enemy.type == ENEMY_BOSS_DEMON_SLIME` reaches 0 HP, `playBossDyingRoar()` is triggered.
  - Adjusted the boss death state timer from $2.2\,\text{s}$ to $2.4\,\text{s}$ to allow the full 22-frame animation sequence and smoke dissipation to be appreciated by the player while the roar echoes.

---

### v5.15 - Boss Demon Slime Death Animation Calibration to Match Updated Dying Roar Audio
#### 1. Acoustic Waveform Analysis of `assets/dyingRoar.mp3`
- Using Windows Media Foundation (`IMFSourceReader`) with PCM decoding to $44,100\,\text{Hz}$ 16-bit mono:
  - Exact decoded PCM stream: $174,576\,\text{samples}$.
  - Calculated total audio duration:
    $$T_{\text{audio}} = \frac{174,576}{44,100} \approx \mathbf{3.95864\,\text{seconds}} \approx 3.96\,\text{s}$$
  - RMS volume analysis across 250ms intervals:
    - $t \in [0.00\text{s}, 0.50\text{s}]$: Initial roar onset ($\text{RMS} = 0.15 \rightarrow 0.26$).
    - $t \in [1.00\text{s}, 1.50\text{s}]$: Primary guttural climax ($\text{RMS} = 0.24 \rightarrow 0.30$).
    - $t \in [1.50\text{s}, 3.50\text{s}]$: Sustained monster vocalization and agony reverberations ($\text{RMS} \approx 0.14$).
    - $t \in [3.50\text{s}, 3.96\text{s}]$: Natural acoustic room decay ($\text{RMS} \approx 0.08 \rightarrow 0.06$).

#### 2. Animation Framerate & Timing Calibration
- **Prior Timing Discrepancy**:
  - Previously, the 22-frame animation sequence ([`assets/enemies/demon_death.png`](/assets/enemies/demon_death.png)) was sampled at $10.0\,\text{FPS}$:
    $$T_{\text{anim}} = \frac{22 \text{ frames}}{10.0\,\text{FPS}} = 2.2\,\text{seconds}$$
  - The entity despawn timer was set to $2.4\,\text{seconds}$.
  - As a result, the boss completely vanished while $1.56\,\text{seconds}$ of the dying roar was still actively echoing.
- **Calibrated Deceleration**:
  - To synchronize the 22-frame visual collapse with the $3.96\,\text{s}$ audio:
    $$\text{Target Framerate} = \frac{22 \text{ frames}}{3.67\,\text{s}} = \mathbf{6.0\,\text{FPS}}$$
  - In [`/src/main.cpp`](/src/main.cpp), updated the death frame calculation:
    $$\text{eFrame} = \min\left( \text{static\_cast<int>}(\text{enemy.animTimer} \times 6.0\text{f}), \; 21 \right)$$
  - Timeline alignment:
    - $t \in [0.00\text{s}, 3.67\text{s}]$: The 22 frames cycle smoothly at $6.0\,\text{FPS}$ ($\approx 167\,\text{ms}$ per frame). The boss staggers backwards, cries out, collapses to its knees, and disintegrates into a dark cloud of smoke.
    - $t \in [3.67\text{s}, 4.00\text{s}]$: The final frame (completely dispersed dark particles) lingers as the tail end of the acoustic roar decays into silence.
  - In [`/src/main.cpp`](/src/main.cpp), extended the despawn threshold:
    ```cpp
    } else if (enemy.state == ENEMY_STATE_DEATH) {
        enemy.animTimer += deltaTime;
        if (enemy.animTimer >= 4.0f) { // Synchronized with 3.96s dyingRoar.mp3 duration
            enemy.active = false;
        }
    }
    ```

---

---

## 3. Actions, Files & Verification
### v5.0
- **Asset Acquisition & Processing**:
  - Implemented automated itch token downloader (`build/fetch_assets.py`) to fetch official packages for Bringer of Death and Boss Demon Slime.
  - Extracted slime animation frames (`slime_idle.gif`, `slime_move.gif`, `slime_take_hit.gif`, `slime_transform.gif`) using standard Python libraries into `assets/enemies/`.
  - Procedurally generated metallic spike hazard texture `assets/spikes.png` ($64\times 64$) and ruby heart texture `assets/heart_pickup.png` ($256\times 64$) via `build/generate_spikes_and_heart.py`.
  - Assembled horizontal spritesheets for all Bringer of Death and Boss Demon Slime animations via `build/assemble_enemy_sprites.py`.
- **Code Modifications**:
  - [`include/Common.h`](/include/Common.h): Defined `SFXType` additions, `EnemyType`, `EnemyState`, `EnemyEntity`, `HeartPickup`, and UV mapping helpers.
  - [`include/World.h`](/include/World.h) & [`src/World.cpp`](/src/World.cpp): Separated shield and direction relics, added procedural heart and enemy spawning, updated distance pruning and reset logic.
  - [`src/Audio.cpp`](/src/Audio.cpp): Synthesized 8 procedural audio waveforms for combat, roars, hits, and pickups.
  - [`include/UIManager.h`](/include/UIManager.h) & [`src/UIManager.cpp`](/src/UIManager.cpp): Added 3 heart pips to `renderScoreHUD`, implemented `renderEnemyHealthBars`, updated tutorial modal and controls documentation.
  - [`include/LadderScene.h`](/include/LadderScene.h), [`src/LadderScene.cpp`](/src/LadderScene.cpp), [`include/SkyFallScene.h`](/include/SkyFallScene.h), [`src/SkyFallScene.cpp`](/src/SkyFallScene.cpp): Integrated `spikesTexture` in render pipelines.
  - [`src/main.cpp`](/src/main.cpp): Implemented attack keybindings (`J`/`X`/`LMB`), 3 lives damage handling, heart collection, enemy AI update loop, boss transformation/despawn, and quad rendering.
- **Compilation & Verification**:
  - Compiled project cleanly using MinGW `g++.exe` with C++17 into `./build/main.exe`.
  - Packaged standalone distribution in [`TheWenderer/`](/TheWenderer/) and updated [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
  - Synchronized documentation in [`README.md`](/README.md), [`doc/CHANGELOG.md`](/doc/CHANGELOG.md), and [`doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md).

---

### v5.1
- Inspected [`TheWenderer/`](/TheWenderer/) directory and identified the duplicate files: `TheWenderer.exe` and `main.exe`.
- Removed `TheWenderer/main.exe`.
- Updated [`makefile`](/makefile) `package` target.
- Re-packaged and verified [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
- Updated [`doc/CHANGELOG.md`](/doc/CHANGELOG.md).

---

### v5.2
- Inspected [`src/main.cpp`](/src/main.cpp) lines and structure.
- Created [`include/UIManager.h`](/include/UIManager.h) and [`src/UIManager.cpp`](/src/UIManager.cpp).
- Created [`include/LadderScene.h`](/include/LadderScene.h) and [`src/LadderScene.cpp`](/src/LadderScene.cpp).
- Created [`include/SkyFallScene.h`](/include/SkyFallScene.h) and [`src/SkyFallScene.cpp`](/src/SkyFallScene.cpp).
- Updated [`include/Common.h`](/include/Common.h) and [`Makefile`](/Makefile).
- Refactored [`src/main.cpp`](/src/main.cpp) to delegate UI, Ladder, and Sky-Fall updates and rendering.
- Tested compilation using `g++` and build automation using `mingw32-make`.
- Updated documentation in [`README.md`](/README.md), [`doc/CHANGELOG.md`](/doc/CHANGELOG.md), and [`doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md).

---

### v5.3
- Inspected sprite bounding boxes using PowerShell `System.Drawing.Bitmap` to determine exact alpha bounds and aspect ratios.
- Updated [`include/World.h`](/include/World.h) and [`src/World.cpp`](/src/World.cpp) with `hasSolidGroundAt` and robust chasm/shaft-free spawning.
- Updated [`src/main.cpp`](/src/main.cpp) with aspect ratio-corrected quads, calibrated movement velocities, ledge safety probes, telegraphed attack wind-ups, and normalized `animTimer` seconds.
- Compiled codebase cleanly with `mingw32-make` and generated standalone release with `mingw32-make package`.
- Updated [`doc/CHANGELOG.md`](/doc/CHANGELOG.md) and [`README.md`](/README.md) with root-relative documentation.

---

### v5.4
- Inspected sprite sheets and attack frame offsets using PowerShell script.
- Re-synthesized `SFX_SLASH` in [`src/Audio.cpp`](/src/Audio.cpp).
- Updated [`src/main.cpp`](/src/main.cpp) with inverted enemy UV flip, global `tutDismissLast` protection, simulation pause during tutorial, and decoupled idle start.
- Compiled codebase with `g++.exe` with zero errors and generated standalone release with `mingw32-make package`.
- Updated [`doc/CHANGELOG.md`](/doc/CHANGELOG.md) and [`README.md`](/README.md).

---

### v5.5
- Modified [`src/main.cpp`](/src/main.cpp) to suppress enemy facing flips during `ENEMY_STATE_ATTACK` and validate frontal damage orientation.
- Compiled codebase with `g++.exe` with zero errors and zero warnings.
- Packaged release with `mingw32-make package` to update [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
- Updated [`doc/CHANGELOG.md`](/doc/CHANGELOG.md) and [`README.md`](/README.md) (`v0.6.5`).

---

### v5.6
- Re-generated optimized, cropped, and scaled spritesheets using PowerShell `System.Drawing`:
  - [`assets/enemies/slime_idle.png`](/assets/enemies/slime_idle.png): $960\times 160$ (6 frames of $160\times 160$)
  - [`assets/enemies/slime_move.png`](/assets/enemies/slime_move.png): $1280\times 160$ (8 frames of $160\times 160$)
  - [`assets/enemies/slime_take_hit.png`](/assets/enemies/slime_take_hit.png): $960\times 160$ (6 frames of $160\times 160$)
  - [`assets/enemies/slime_transform.png`](/assets/enemies/slime_transform.png): $5280\times 160$ (33 frames of $160\times 160$, fits within 8192 limit)
  - [`assets/enemies/demon_smash.png`](/assets/enemies/demon_smash.png): $5184\times 160$ (18 frames of $288\times 160$, matching all demon sheets)
- Updated [`src/main.cpp`](/src/main.cpp):
  - Updated initial slime quad render size to `glm::vec2(2.4f, 2.4f)` and transformation quad to `glm::vec2(6.0f, 6.0f)`.
  - Added ground alignment when slime transformation begins (`enemy.pos.y = footY + 3.0f`).
  - Rewrote `ENEMY_STATE_JUMP` using a 3-sub-phase physics state machine (`stateTimer`):
    - Sub-phase 0: Crouch charge (frames 0–4)
    - Sub-phase 1: Airborne leap with gravity until touchdown
    - Sub-phase 2: Ground slam touchdown, shockwave damage window (frames 11–14), and recovery
  - Added universal platform gravity and ground snapping for boss patrol and idle states.
- Updated [`src/World.cpp`](/src/World.cpp) slime spawn position to `spawnY = footY + 1.2f`.
- Successfully compiled engine executable with `g++` (`build/main.exe`).
- Updated standalone distribution package [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
- Updated [`doc/CHANGELOG.md`](/doc/CHANGELOG.md) (`v0.6.6`).

---

### v5.7
- Terminated running game process lock before compilation.
- Stitched 9-frame `bod_cast.png` and 16-frame `bod_spell.png` using PowerShell `System.Drawing`.
- Analyzed exact pixel coordinates and bounding boxes of Bringer of Death sprite canvas.
- Integrated MCI streaming audio methods into [`/include/Audio.h`](/include/Audio.h) and [`/src/Audio.cpp`](/src/Audio.cpp).
- Added `ENEMY_STATE_CAST`, `SpellHazard`, and `spellCooldown` to [`/include/Common.h`](/include/Common.h).
- Added grouped platform spikes and multi-enemy generation to [`/src/World.cpp`](/src/World.cpp).
- Updated Pause Menu and Tutorial Modal layouts in [`/include/UIManager.h`](/include/UIManager.h) and [`/src/UIManager.cpp`](/src/UIManager.cpp).
- Implemented input decoupling, BOD centering offset, spell casting AI, hazard lifecycle, and sequenced audio in [`/src/main.cpp`](/src/main.cpp).
- Compiled game binary (`build/main.exe`) and produced standalone package (`TheWenderer_Standalone.zip`).
- Synchronized version history in [`/doc/CHANGELOG.md`](/doc/CHANGELOG.md) and [`/README.md`](/README.md).

---

### v5.8
- **Asset Removal & Gitignore**:
  - Deleted redundant sprite directory and un-tracked cached files via `git rm -r --cached`.
  - Updated [`.gitignore`](/.gitignore).
- **Audio Subsystem Expansion**:
  - Updated [[`include/Audio.h`](/include/Audio.h)] and [[`src/Audio.cpp`](/src/Audio.cpp)] with `startBGM()`, `stopBGM()`, `toggleBgmMute()`, `isMuted()`, and `playThunderSpell()`.
- **Combat & World Calibration**:
  - Updated [[`include/Common.h`](/include/Common.h)] to add `bool audioPlayed` to `SpellHazard`.
  - Updated [[`src/World.cpp`](/src/World.cpp)] to enforce 12.0m spacing between Bringer of Death and Slime entities.
- **Main Loop & Input Integration**:
  - Updated [[`src/main.cpp`](/src/main.cpp)]:
    - Defaulted `freeExplorationMode = true`.
    - Integrated BGM start on intro finish/skip/failure and BGM stop on cleanup.
    - Added `GLFW_KEY_M` mute toggle in `processInput`.
    - Expanded BOD spell targeting across multi-layer elevations ($|\Delta Y| < 14.0\text{m}$, $|\Delta X| \in [3.0\text{m}, 14.0\text{m}]$).
    - Implemented 1.0s telegraphed gathering cloud warning phase and synchronized `thunder-after-crackle.mp3` SFX with lightning strike.
    - Updated spell rendering quad frame calculation.
- **UI Enhancements**:
  - Updated [[`include/UIManager.h`](/include/UIManager.h)] and [[`src/UIManager.cpp`](/src/UIManager.cpp)] with BGM mute status in `renderPauseMenu` and comprehensive control descriptions in `renderTutorialModal`.
- **Build & Verification**:
  - Recompiled binary with `g++.exe` into `build/main.exe`.
  - Generated updated standalone package `TheWenderer_Standalone.zip` via `mingw32-make package`.
  - Updated [[`README.md`](/README.md)] and [[`doc/changelog.md`](/doc/changelog.md)].

---

### v5.9
- Inspected spritesheet dimensions and alpha transparency of `assets/samurai/Defend_nobg.png`.
- Analyzed audio waveform envelope of `assets/thunder-after-crackle.mp3`.
- Updated [`include/Common.h`](/include/Common.h) to add `SFX_BLOCK` and `texDefend` texture handle.
- Implemented metallic deflection SFX synthesis in [`src/Audio.cpp`](/src/Audio.cpp).
- Updated [`include/UIManager.h`](/include/UIManager.h) and [`src/UIManager.cpp`](/src/UIManager.cpp) to display guard telemetry and tutorial instructions.
- Implemented blocking state machine, right-click input handling, damage negation, and spell audio pre-delay in [`src/main.cpp`](/src/main.cpp).
- Successfully compiled `build/main.exe` and tested with MSYS2 GCC 14.2.0.
- Re-packaged distribution package `TheWenderer_Standalone.zip` and updated `TheWenderer.exe`.
- Synchronized documentation in [`README.md`](/README.md), [`doc/changelog.md`](/doc/changelog.md), and [`doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md).

---

### v5.10
- Updated [`doc/explain.md`](/doc/explain.md) with comprehensive Modern OpenGL sprite rendering guide, 8-block modular architecture, combat & defense breakdowns, and viva questions.
- Rewrote [`doc/documentation.md`](/doc/documentation.md) with complete technical specifications of all subsystems, mathematical models, state machines, and build flags.
- Updated [`README.md`](/README.md) with corrected direct compilation command and heraldic shield pickup descriptions.
- Created session prompt archive [`doc/prompts/v5.10 - Documentation and Academic Guide Synchronization with Modern Subsystems.md`](/doc/prompts/v5.10%20-%20Documentation%20and%20Academic%20Guide%20Synchronization%20with%20Modern%20Subsystems.md).

---

### v5.11
- Inspected pixel bounding boxes of all 16 frames in `assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/Spell/`.
- Updated `src/main.cpp` active spell damage check to require `sFrame >= 7 && sFrame <= 10` ($0.58\text{s}$ to $0.85\text{s}$).
- Updated initial spell spawn floating text warning to `"THUNDER CLOUD GATHERING! MOVE!"`.
- Recompiled binary `build/main.exe` with GCC 14.2.0 (exit code 0).
- Re-packaged distribution package `TheWenderer_Standalone.zip` and updated `TheWenderer.exe`.
- Synchronized documentation in [`doc/explain.md`](/doc/explain.md), [`doc/documentation.md`](/doc/documentation.md), [`README.md`](/README.md), and [`doc/changelog.md`](/doc/changelog.md).

---

### v5.12
- Inspected [`/src/World.cpp`](/src/World.cpp) procedural chunk generation logic and enemy placement algorithms.
- Modified [`/src/World.cpp`](/src/World.cpp) with starter slimes, 100% chunk spawn roll, 2–4 slimes/chunk, cavern spawning, and vertical-aware distance buffers.
- Re-compiled engine with `mingw32-make` (`g++ -std=c++17 -Wall -Wextra -O2 ...`) verifying clean build.
- Packaged standalone distribution via `mingw32-make package`, synchronizing `TheWenderer.exe` and `TheWenderer_Standalone.zip`.
- Updated documentation:
  - [`/README.md`](/README.md): Added `[v0.6.12]` entry.
  - [`/doc/changelog.md`](/doc/changelog.md): Added `v0.6.12` to Table of Versions and Detailed Version Logs.
  - [`/doc/documentation.md`](/doc/documentation.md): Updated Section 3.6 to specify guaranteed slime frequency and multi-elevation cavern encounters.
  - [`/doc/prompts/v5.12 - Increase Slime Enemy Spawn Frequency and Multi-Elevation Density.md`](/doc/prompts/v5.12%20-%20Increase%20Slime%20Enemy%20Spawn%20Frequency%20and%20Multi-Elevation%20Density.md): Authored complete session archive.

---

### v5.13
- Developed diagnostic tool `build/analyze.cpp` using `stb_image.h` to measure bounding boxes and pivot anchors of `Idle.png`, `Run.png`, `Attack1.png`, and `Defend_nobg.png`.
- Developed `build/check_alpha.cpp` to analyze alpha transparency distributions.
- Authored and executed high-fidelity 64-sample supersampling tool `build/resample_defend.cpp` to generate calibrated [`assets/samurai/Defend_nobg.png`](/assets/samurai/Defend_nobg.png).
- Re-analyzed output with `build/analyze.exe`, confirming exact match with `Idle.png` dimensions ($51$–$53\,\text{px}$ height, $\text{BottomY}=121$, $\text{CenterX}=94$).
- Cleaned up temporary diagnostic executables and updated standalone distribution zip via `mingw32-make package`.
- Updated documentation:
  - [`/README.md`](/README.md): Added `[v0.6.13]` entry.
  - [`/doc/changelog.md`](/doc/changelog.md): Added `v0.6.13` to Table of Versions and Detailed Version Logs.
  - [`/doc/documentation.md`](/doc/documentation.md): Updated Section 3.4 with calibrated defend coordinate framing.
  - [`/doc/prompts/v5.13 - Scale Down Player Defend Animation to Match Idle Spritesheet Proportions.md`](/doc/prompts/v5.13%20-%20Scale%20Down%20Player%20Defend%20Animation%20to%20Match%20Idle%20Spritesheet%20Proportions.md): Generated session archive.

---

### v5.14
- Inspected slime texture geometry and frame bounds using diagnostic executable `build/check_slime.cpp`.
- Formulated the exact mathematical ground offset and collision tolerance discrepancy in [`/src/World.cpp`](/src/World.cpp) and [`/src/main.cpp`](/src/main.cpp).
- Modified [`/src/World.cpp`](/src/World.cpp): Raised starter and procedural slime spawn positions by $+0.5\,\text{m}$ (`spawnY = footY + 1.2f`).
- Modified [`/src/main.cpp`](/src/main.cpp): Restricted player-tracking facing orientation to active pursuit mode ($<8\,\text{m}$) and added `audioMgr.playBossDyingRoar()` on boss defeat.
- Modified [`/include/Audio.h`](/include/Audio.h) and [`/src/Audio.cpp`](/src/Audio.cpp): Implemented `playBossDyingRoar()` and MCI cleanup handlers.
- Re-compiled engine with `g++.exe` (`-std=c++17 -Wall -Wextra -O2 ...`) verifying clean exit code 0.
- Synchronized standalone portable distribution via `mingw32-make package`.
- Updated documentation:
  - [`/README.md`](/README.md): Added `[v0.6.14]` entry.
  - [`/doc/changelog.md`](/doc/changelog.md): Added `v0.6.14` to Table of Versions and Detailed Version Logs.
  - [`/doc/documentation.md`](/doc/documentation.md): Updated Sections 3.2 and 3.5.
  - [`/doc/prompts/v5.14 - Slime Ground Placement & AI Unfreezing, and Boss Demon Slime Dying Roar Audio Integration.md`](/doc/prompts/v5.14%20-%20Slime%20Ground%20Placement%20%26%20AI%20Unfreezing,%20and%20Boss%20Demon%20Slime%20Dying%20Roar%20Audio%20Integration.md): Recorded full prompt archive.

---

### v5.15
- Built and ran `build/analyze_audio.cpp` linking `mfplat`, `mfreadwrite`, and `ole32` to measure exact PCM sample counts, RMS energy, and duration of [`/assets/dyingRoar.mp3`](/assets/dyingRoar.mp3) ($3.958\,\text{s}$).
- Cleaned up temporary audio analysis binary.
- Modified [`/src/main.cpp`](/src/main.cpp):
  - Slowed Boss Demon Slime death animation sampling from $10.0\,\text{FPS}$ to $6.0\,\text{FPS}$.
  - Extended despawn lifetime from $2.4\,\text{s}$ to $4.0\,\text{s}$.
- Compiled `build/main.exe` using `g++.exe` (`-std=c++17 -Wall -Wextra -O2 ...`) verifying zero errors or warnings.
- Re-packaged standalone release [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip) via `mingw32-make package`.
- Updated documentation:
  - [`/README.md`](/README.md): Added `[v0.6.15]` entry.
  - [`/doc/changelog.md`](/doc/changelog.md): Added `v0.6.15` to Table of Versions and Detailed Version Logs.
  - [`/doc/documentation.md`](/doc/documentation.md): Updated Section 3.5.
  - [`/doc/prompts/v5.15 - Boss Demon Slime Death Animation Calibration to Match Updated Dying Roar Audio.md`](/doc/prompts/v5.15%20-%20Boss%20Demon%20Slime%20Death%20Animation%20Calibration%20to%20Match%20Updated%20Dying%20Roar%20Audio.md): Created session prompt archive.

---

---

## 4. Outcomes
- **v5.0**: A comprehensive breakdown of all implemented systems:
1. **Relic & Shield Deconfliction**: Shield relics spawn at calibrated rates away from direction change arrowheads.
2. **Hazard Spikes & Bringer of Death Enemy**: Procedural steel spike hazard texture replacing pumpkins; Bringer of Death roaming enemy with scythe attack.
3. **Demon Slime Boss Battle**: Manual engagement by attacking Initial Slime, delayed metamorphosis into giant Boss Demon Slime, proximity damage, and spatial avoidance/despawning.
4. **Player 3-Lives Pipeline & Heart Relics**: 3 life pips in HUD, ruby heart pickups, 1.25s grace invulnerability.
5. **Melee Sword Combat & Overhead Health Bars**: Katana slash attack (`J`/`X`/`LMB`), exact hit logic (BOD: 1 hit, Slime: 2 hits, Boss: 5 hits), and overhead segmented health bars.
6. **Procedural Enemy & Combat SFX**: 8 synthetic waveforms covering slashes, hits, attacks, deaths, roars, and pickups.
- **v5.1**: Explained why both files existed (compiler default `main.exe` vs branded player-facing `TheWenderer.exe`), detailed the cleanup performed, and confirmed that only `TheWenderer.exe` is now present in [`TheWenderer_Standalone.zip`](/TheWenderer_Standalone.zip).
- **v5.2**: The final response provides a comprehensive architectural breakdown of the modular separation, highlighting the extracted files, code size reduction, clean compilation status, and verification of standalone packaging.
- **v5.3**: *(See final response delivered to the user below.)*
- **v5.4**: *(See final response delivered to the user below.)*
- **v5.5**: *(See final response delivered to the user below.)*
- **v5.6**: [Comprehensive explanation of the root causes, spritesheet crop and scale metrics, physics state machine fixes, and packaging updates.]
- **v5.7**: *(Provided below in markdown detailing all changes, mathematical centering derivations, audio sequence timings, and updated control key bindings)*
- **v5.8**: [Full final response delivered below detailing each feature, code modifications, controls table, and build confirmation.]
- **v5.9**: [Detailed report with complete technical explanation of the spellcast audio alignment, right-click defend implementation, spritesheet configuration, compilation results, and controls guide.]
- **v5.10**: [Comprehensive summary of all documentation enhancements, including the newly added Modern OpenGL 2D sprite mechanics section, technical architecture breakdown, and viva presentation notes.]
- **v5.11**: [Detailed report with technical breakdown of the 16 spell animation frames, the mid-animation damage window calibration, compilation results, and gameplay behavior.]
- **v5.12**: Detailed report explaining starter slime placement, 2–4 slimes per chunk, subterranean cavern spawning, vertical-aware deconfliction buffers, and verification steps.

---
- **v5.13**: Comprehensive report detailing the pixel discrepancy discovery, mathematical scaling factor ($0.3377\times$), pivot grounding alignment ($Y=121$, $X=94$), and before/after verification metrics.

---
- **v5.14**: Comprehensive summary of the mathematical root cause of the underground offset and raycasting deadlock, details of the spawn height adjustment, patrol AI orientation unfreezing, and dying roar audio integration.

---
- **v5.15**: Concise summary explaining the measured $3.96\text{s}$ audio duration, the $6.0\text{ FPS}$ animation deceleration, the $4.0\text{s}$ despawn threshold alignment, and verification details.

---
