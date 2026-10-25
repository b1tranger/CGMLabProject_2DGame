
# The Wenderer: A Multi-Directional Endless Runner

> Computer Graphics & Multimedia Lab Project (CSE0611328)  
> Department of Computer Science & Engineering, UITS

---

## 🎮 About The Project

**The Wenderer** is a 2D side-view endless runner and platformer built in **C++** using **Modern OpenGL (3.3 Core Profile)**. 

Unlike traditional static runners, **The Wenderer** features autonomous dynamic direction changes (suddenly flipping between running right, left, and climbing ladders) in random intervals, paired with a smoothly reframing side-view camera, animated character sprites (running, climbing, falling, jumping), procedural platforms, ladders, spikes, barriers, and collectible gems.

---

## 🚀 Quick Start (Build & Run)

### Requirements
- MinGW-w64 (`g++` supporting C++17) or CMake
- All OpenGL and imaging libraries (`GLFW`, `GLAD`, `GLM`, `stb_image`) are bundled in the repo!

### Building with Makefile (Recommended)
```bash
# Build the project
mingw32-make

# Or compile directly with g++:
g++.exe -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32

# Run the game
./build/main.exe

# Package standalone portable game distribution zip (~5.8 MB):
mingw32-make package
```

### Controls
- **`J` / `X` / `Left Mouse Click`**: **Sword Slash Attack** — Swing katana to defeat Bringer of Death (1 hit), Initial Slime (2 hits), and Demon Slime Boss (5 hits)!
- **`F`**: **Toggle Free Exploration Mode** — Disables continuous autonomous running! Walk and sprint with precision using `A` and `D`, rest in idle breathing stance when stationary, and hold position on ladders unless pressing `W`/`S`. Press `F` again to re-engage auto-runner mode.
- **`Space` / `Esc` / `Enter` (During Intro)**: **Skip Intro Video** — Immediately interrupt the opening cinematic and advance to game spawn.
- **`Left Shift`**: **Dash Movement** — High-speed burst with ghost trail afterimages and spike/hazard invulnerability!
- **`Space` / `W` / `Up Arrow`**: Jump across gaps and over spiky traps, or latch on and climb upward inside ladder shafts
- **`S` / `Down Arrow`**: Drop down through one-way platforms or descend into underground ladder shafts
- **`A` / `Left Arrow`**: Run left (in Ladder Climb: leap to left ladder track; in Sky-Fall: aerial steer left)
- **`D` / `Right Arrow`**: Run right (in Ladder Climb: leap to right ladder track; in Sky-Fall: aerial steer right)
- **`Esc`**: Open / Close **Pause Menu** (pause physics simulation, inspect stats, restart, or quit)
- **`B`**: **Cycle Background Scenery** (Silvergrass Field <-> Autumn Foliage <-> Green Forest Foliage <-> Urban Area)
- **`N`**: **Fast-Forward Day / Night Cycle** (Instantly advance 90-second half-cycle between Day and Night)
- **`T`**: Toggle autonomous direction shifts (`ENABLED` timer shifts every 6.5s <-> `DISABLED` direction relics only)
- **`R`**: Quick restart run (when paused or after Game Over)
- **`Q`**: Quit game to desktop (from Pause Menu or Game Over screen)
- **3-Lives Health System & Heart Relics**: The player possesses 3 life pips displayed on the HUD. Collect ruby heart relics (spawned alongside shields) to restore lost lives, or gain +150 bonus overheal points.
- **Roaming Enemies & Boss Fight**:
  - **Bringer of Death**: Roaming scythe-wielding wraith. Defeat in **1 hit** before it cleaves you!
  - **Initial Slime & Boss Demon Slime**: Defeating the green slime in **2 hits** triggers a 3.2-second transformation into the massive Boss Demon Slime. Slay the boss in **5 hits** or avoid it by sprinting away in the opposite direction ($>28\text{m}$) or dashing through it to despawn!
- **Procedural Metallic Spikes**: Traps look like razor-sharp steel spikes with hazard red bases, replacing old pumpkin tiles.
- **Mini-Game Scene Exits**: Mini-game scenes (3-Ladder Climbing and Sky-Fall Diving) can strictly **only** be exited by collecting Direction Arrow Relics (which unlock and spawn after the 15-second survival trial) or by taking an obstacle hit (resurrecting back on the main platformer). Manual scene switching via key presses is disabled.
- **Underground Ladder Shafts**: Approach any underground ladder shaft from the surface and press `S` or `Down Arrow` to smoothly latch on and climb down into the subterranean caverns.
- **Idle Stance at Start**: The Samurai starts in a grounded idle breathing stance upon game launch and restart until directional movement keys (`A`, `D`) or `Space` are pressed.
- **Direction Arrow Relics**: Distinct glowing neon arrowheads (Left, Right, Up, Down) explicitly indicate which direction change or scene transition they trigger when touched.
- **Fruit Pickups (10s Energy Shield)**: Collecting tasty fruit grants a 10-second glowing energy shield bubble that deflects spikes, floating mines, and raptor birds!
- **Steep Slopes**: Walk or fall onto an angled slope to automatically engage controllable downhill **Sliding**! (Press `Space` to jump out at any midpoint)
- **Ladder Tunnels**: Approach a ladder shaft to immediately stop horizontal run and ascend/descend
- **Touch / Collect Gold Coin**: Scores bonus points (+50 pts) with floating `+50` text popups, real-time corner HUD feedback, and on-screen score tracking

---

## 📚 Documentation & Guides

- **[Complete Project Changelog](/doc/changelog.md)**: Detailed chronological history of all releases mapped to session prompt archives.
- **[Technical Progress Documentation](/doc/documentation.md)**: Comprehensive architectural, mathematical, and subsystem specification.
- **[Academic Code Guide & Explanations](/doc/explain.md)**: Beginner-friendly academic guide explaining components in simple terms for lab presentations & vivas.
- **[Learning & Implementation Guide](/doc/LEARNING_GUIDE.md)**: Beginner-friendly step-by-step breakdown of OpenGL concepts, mathematics, and guided milestones.
- **[Project Proposal](/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf)**: Formal proposal for the CGM Lab course.
- **[Research Analysis](/doc/research/Platformer%20Game%20Research%20Analysis.pdf)**: Architectural, ludonarrative, and market research.
- **[Implementation Steps](/doc/research/implementationSteps.pdf)**: Core roadmap and code milestones.

---

## 📜 Version History (Changelog)

### [v0.6.6] - 2026-10-25 (Updated Slime Animations & Boss Demon Slime Platform Jump Mechanics)
- **Updated Slime Animation Spritesheets**:
  - Converted the updated initial slime GIF assets from `assets/enemies/boss_demon_slime/initial_slime/` into horizontal transparent PNG spritesheets: `slime_idle.png` (6 frames), `slime_move.png` (8 frames), `slime_take_hit.png` (6 frames), and `slime_transform.png` (33 frames).
  - Updated OpenGL engine frame calculations to support the 33-frame transformation sequence.
- **Boss Demon Slime Platform Jumping (`06_Smash.gif` Integration)**:
  - Extracted and assembled `06_Smash.gif` into `assets/enemies/demon_smash.png` (18 frames at $576\times 320$).
  - Added `ENEMY_STATE_JUMP` state machine and `jumpCooldown` timer to Boss Demon Slime.
  - Implemented multi-phase platform jumping behavior:
    - **Crouch Charge (Frames 0–4)**: Boss gathers dark power on current platform.
    - **Leap Launch (Frame 5)**: Ballistic jump trajectory towards the player ($v_y = 11.0$–$14.5\text{ m/s}$, $v_x = 3.5$–$8.5\text{ m/s}$).
    - **Airborne Flight & Dynamic Platform Landing (Frames 5–9)**: Boss experiences platformer gravity and evaluates solid tile collisions across `gameWorld.tiles`, snapping feet smoothly onto elevated or lower platform tiers.
    - **Ground Slam & Shockwave Impact (Frames 10–16)**: Smashes the ground with impact SFX, floating text, and a localized frontal shockwave damaging players who fail to dodge.
    - **Recovery (Frame 17)**: Recovers into patrol stance with a 3.2s jump cooldown.
  - Autonomous AI triggers jumps when encountering platform gaps/chasms, pursuing players on elevated platforms, or closing mid-range distances.
- **Standalone Package Update**: Re-compiled and packaged `TheWenderer_Standalone.zip` with the updated binary and enemy assets.

### [v0.6.5] - 2026-10-25 (Directional Attack Commitment: Locked Boss Demon Slime & Bringer of Death Facing Direction)
- **Boss Attack Directional Commitment (Zero Mid-Attack Spinning)**:
  - Fixed an issue where the Demon Slime Boss (and Bringer of Death) continually updated their facing direction based on `distX < 0.0f` on every frame, even mid-swing, causing them to unnaturally flip $180^\circ$ mid-animation if the player jumped or dashed behind them.
  - Locked `enemy.facingLeft` at the start of the attack animation: while `enemy.state == ENEMY_STATE_ATTACK`, facing direction updates are suppressed.
- **Frontal Impact Damage Validation**:
  - Cleave and shockwave damage now strictly requires the player to be physically situated in front of the attacking enemy: `playerInFront = (enemy.facingLeft && distX <= 0.4f) || (!enemy.facingLeft && distX >= -0.4f)`.
  - Dashing or jumping behind the boss during its telegraph wind-up now cleanly evades the attack.

### [v0.6.4] - 2026-10-25 (Fixed Enemy Facing Direction, Metallic Katana Slash SFX, Guaranteed Tutorial Window & Idle Stillness on Restart)
- **Enemy Facing Direction Correction (Eliminated Moonwalking)**:
  - Corrected horizontal UV flip calculation for enemy sprite sheets (`bod_walk.png`, `slime_move.png`, `demon_walk.png`), which natively face **LEFT** in raw texture space.
  - Setting `flipHorizontal = !enemy.facingLeft` renders un-flipped textures when walking left and flipped textures when moving right, ensuring Bringer of Death always faces and walks forward.
- **Realistic Procedural Katana Sword Slash SFX**:
  - Overhauled `SFX_SLASH` in [`src/Audio.cpp`](/src/Audio.cpp) to synthesize a sharp, resonant Japanese katana sword slash combining an aerodynamic whoosh (descending 2400 Hz to 320 Hz exponential sweep) with multi-frequency steel ring harmonics (3250 Hz, 4950 Hz, 1920 Hz) and a sharp cutting transient bite.
- **Guaranteed Post-Intro Tutorial Window Modal**:
  - Fixed race condition where holding or pressing `Space`, `Enter`, or `Esc` to finish or skip the intro video immediately dismissed the tutorial window on the subsequent frame.
  - Initialized `tutDismissLast = true` upon video skip/completion, strictly requiring the player to release and freshly press a key to dismiss the tutorial guide.
  - Froze gameplay simulation (`deltaTime = 0.0f`) while `showTutorialModal` is active so no enemies or hazards can advance while reading the tutorial.
- **Grounded Idle Stillness on Tutorial Dismiss & Game Over Restart**:
  - Ensured player remains completely motionless (`playerVel = (0, 0)`, `isGameIdleAtStart = true`) upon tutorial dismissal and upon Game Over restart.
  - Guarded Game Over input with `gameOverRestartLast` and prevented `Space` from registering as jump on the restart frame.
  - Preserved idle stance until directional navigation keys (`A`, `D`, `Left`, `Right`) are deliberately pressed, allowing the player full control over when to start running.

### [v0.6.3] - 2026-10-25 (Calibrated Enemy Proportions, Speeds, Platform Grounding & Telegraphed Attack Wind-Ups)
- **Enemy Quad Aspect Ratios & Proportional Rendering**:
  - **Bringer of Death**: Scaled render quad from square `(4.8, 4.8)` to true 1.5:1 aspect ratio `(4.5, 3.0)` matching $140\times 93$ pixels, fully eliminating vertical stretching.
  - **Initial Slime**: Scaled render quad from `(2.4, 2.4)` to `(4.5, 2.5)` (1.8:1 aspect ratio matching $576\times 320$ pixels), making it visibly prominent, clear, and perfectly proportioned.
  - **Boss Demon Slime**: Scaled render quad to true 1.8:1 aspect ratio `(10.8, 6.0)` matching $288\times 160$ pixels.
- **Platform Grounding & Ledge Boundary Safety**:
  - Added [`World::hasSolidGroundAt`](/src/World.cpp) to verify solid ground beneath feet. Procedural spawning verifies positions are not in chasms or ladder shafts.
  - Raised enemy vertical center offsets so feet touch ground levels ($Y = 0.0\text{m}$ surface, $Y = -4.0\text{m}$ cavern).
  - Added forward ledge detection in AI: patrolling enemies reverse at edges and chasing enemies halt at ledges rather than walking off platforms and floating in mid-air.
- **Pacing & Speed Calibration**:
  - Bringer of Death: patrol at $1.0\text{m/s}$, chase at $1.3\text{m/s}$ (down from $2.2\text{m/s}$).
  - Slime: wander at $0.75\text{m/s}$, approach at $0.9\text{m/s}$ (down from $1.5\text{m/s}$).
  - Boss Demon Slime: approach at $0.9\text{m/s}$ (down from $1.8\text{m/s}$).
- **Telegraphed Attack Wind-Up Animations**:
  - Bringer of Death pauses movement and raises scythe for $0.75\text{s}$ (frames 0-5 at 8 fps) with overhead red alert `"[ ! ]"` before cleave impact lands on frames 6-7.
  - Boss Demon Slime removed instant walking contact damage. Telegraphed cleave attack plays $1.125\text{s}$ wind-up (frames 0-8 at 8 fps) with overhead alert `"[ ! ] DODGE!"` before ground slam shockwave strikes on frames 9-11.
  - Standardized `enemy.animTimer` accumulation in seconds, eliminating compounded 10x double-multiplication glitch.

### [v0.5.15] - 2026-10-25 (Ladder Hero Scale Fix, Post-Intro Tutorial Modal, Shield Icon Pickups & Scene Exit Timers)
- **Ladder Climb Hero Scale Restored**: Fixed missing matrix scale transformation on `modelHero` in `SCENE_LADDER_CLIMB`, scaling the climbing Samurai Mack sprite by `playerRenderSize` ($5.5\times 5.5$) to match runner and sky-fall proportions.
- **Post-Intro Tutorial Window Modal**:
  - Displays a glassmorphic tutorial window automatically right after intro video playback (or skip), strictly once per application launch.
  - Explains the essence of "Wandering": *"Endless runner with traps to avoid and shields to pickup. Change movement direction and find the end goal of this venture."*
  - Instructs the player to inspect the Pause Menu for control details and press **`[T]`** while paused to reopen the tutorial guide at any time.
- **Dedicated Shield Icon Pickups**: Replaced ambiguous Fruit sprites with a glowing, heraldic Shield icon (`assets/shield_pickup.png`) featuring golden/cyan trims and radiant star emblem, eliminating confusion between direction warps and defensive force fields.
- **Mini-Game Scene Countdown & Exit Prompts**:
  - Added an on-screen top-center countdown badge (`SURVIVE UNTIL EXIT: 15.0s -> 0.0s`) with an integrated progress bar in `SCENE_LADDER_CLIMB` and `SCENE_SKY_FALL`.
  - Displays a prominent, glowing banner prompt: `=== EXIT APPEARED! === [TOUCH DIRECTION ARROW TO RETURN]` when the survival trial completes, cleanly alerting players as directional exit relics appear in the path.

### [v0.5.14] - 2026-10-25 (Circular Energy Shield Bubble & Single-Hit Trap Absorption)
- **Single-Hit Trap Absorption & Removal**: Collecting fruits equips a protective energy shield that now absorbs exactly one trap/hazard hit and immediately breaks (`shieldTimer = 0.0f`).
- **Post-Hit Grace Invulnerability Window**: Added a $0.75\,\text{s}$ grace period (`postHitInvulnTimer`) upon shield shatter, preventing instant death on the subsequent frame while the player clears the hazard bounding box.
- **Immediate Deflection Feedback**: Replaced repetitive spam of the "DEFLECTED" notification with a single, clear floating announcement: `"SHIELD BROKE! [TRAP DEFLECTED]"` accompanied by sound effects.
- **Anti-Aliased Circular Energy Shield Rendering**: Implemented `drawCircle()` with analytical distance testing (`length(TexCoord - 0.5) > 0.5 discard;`) and smooth-step edge filtering in the fragment shader. Replaced the flat box quad with a pulsing, glowing circular force-field bubble wrapped closely around the hero across all scenes.

### [v0.5.13] - 2026-10-25 (Mini-Game Scene Exit Restrictions & Underground Ladder Descent Fix)
- **Scene Exit Restrictions**: Removed arbitrary manual key triggers between Ladder Climbing and Sky-Fall scenes (and outer-lane leaps). Mini-game scenes can strictly only be exited by collecting Direction Arrow Relics (which spawn post-15s) or by suffering an obstacle hit (triggering cinematic hit/resurrection and returning to the runner).
- **Underground Ladder Descent Fix**: Resolved collision deadlock where players would freeze on top of underground ladders when pressing `S`/`Down Arrow`.
  - Added pre-planned underground ladder shaft registration in procedural generation.
  - Configured surface ground tiles directly over ladder shafts as one-way traversable tiles (`isOneWay = true`) and eliminated solid underground dirt blocks (`sDirt`) plugging the shaft interior.
  - Expanded ladder detection vertical threshold to `tunnel.maxY + 1.4f` and bypassed interior shaft collisions during active ladder climbing.
  - Added support for `GLFW_KEY_DOWN` alongside `GLFW_KEY_S` for dropping through one-way tiles and entering shafts.

### [v0.5.12] - 2026-10-25 (Free Exploration Mode & Manual Movement Toggle)
- **Free Exploration Mode (`F` Key)**: Allows the player to switch between autonomous endless-runner sprinting and traditional, precision platformer exploration at will.
- **Velocity Arrest on Key Release**: When Free Exploration is active, releasing `A` and `D` brings horizontal velocity to a complete stop ($v_x = 0$), rather than continuing to auto-run indefinitely.
- **Dynamic Idle Stance Resolution**: Character transitions seamlessly to `ANIM_IDLE` breathing animation when stopped on solid ground, and immediately transitions to `ANIM_RUN` when moving.
- **Stationary Ladder Holding**: Climbing ladders in Free Exploration halts vertical climbing when neither `W` nor `S` is pressed, allowing players to pause and survey obstacles.
- **HUD & Pause Menu Indicators**: Real-time floating announcement (`FREE EXPLORATION [ON/OFF]`), on-screen HUD badge, window title indicator, and pause menu mode status.

### [v0.5.11] - 2026-10-25 (Missing DLL Fix & Intro Video Visuals Restored)
- **Eliminated Missing `msys-stdc++-6.dll` Error**: Added `-static-libgcc -static-libstdc++` compiler flags to embed the C++ runtime directly into `main.exe`, and updated the `package` target to bundle `msys-2.0.dll` automatically for flawless portable execution.
- **Fixed Invisible Video (Alpha Discard Bug)**: Added `uDiscardAlpha` uniform to the rendering pipeline, set `GL_TEXTURE_SWIZZLE_A = GL_ONE` and `GL_RGB` on the video texture, preventing the GPU from discarding opaque video frames whose 4th byte is zero in Windows Media Foundation.
- **Dynamic Viewport & Framebuffer Alignment**: Initialized `glViewport` with real GLFW framebuffer dimensions to ensure sharp, centered video projection across all monitor scaling modes.

### [v0.5.10] - 2026-10-25 (Standalone Single-File Game Packaging Analysis & Distribution)
- **Standalone Distribution Architecture**: Comprehensive analysis and verification of standalone game packaging methods (Virtual Container `.exe`, In-Binary Resource Embedding, and Portable Single-Zip Distribution).
- **Automated `package` Build Target**: Added `mingw32-make package` target generating a clean, self-contained standalone zip (`TheWenderer_Standalone.zip`, ~5.8 MB) containing `main.exe`, `glfw3.dll`, and all assets ready for instant distribution and execution on any Windows machine.
- **Runtime Dependency Verification**: Verified standard Windows Universal C Runtime (UCRT), Media Foundation, GDI32, and WinMM system library linkage with zero compiler dependencies required for end users.

### [v0.5.9] - 2026-10-25 (In-Engine Intro Video Playback & Asset Size Analysis)
- **In-Engine Video Player (`VideoPlayer`)**: Built native Windows Media Foundation (`IMFSourceReader`) video and audio player in [`include/VideoPlayer.h`](/include/VideoPlayer.h) and [`src/VideoPlayer.cpp`](/src/VideoPlayer.cpp) with zero external binary dependencies.
- **Hardware-Accelerated Frame Uploads**: Streams 1080p video frames into an OpenGL texture with dynamic letterboxing to preserve aspect ratio in any resolution.
- **Synchronized Audio & Skip Support**: Pre-decodes audio stream to in-memory PCM Wave played asynchronously via `PlaySoundA`. Allows skipping at any moment via `Space`, `Enter`, or `Esc`.
- **Cinematic Presentation**: Displays an animated bottom progress bar and pulsing skip indicator during intro playback.

### [v0.5.8] - 2026-10-25 (Sky-Fall Down Scene Left & Right Direction Relics)
- **Sky-Fall Direction Relic Gates**: Staggered multi-lane gates with glowing Left (`DIR_LEFT`, Cyan) and Right (`DIR_RIGHT`, Gold) direction arrowheads actively spawn in the Down direction Sky-Fall scene after the 15-second trial completes.
- **Retroactive Conversion Ahead of Descent**: Uncollected coins/fruits immediately ahead of the falling character are dynamically converted into directional exit relics upon 15s trial completion.
- **Expanded Aerial Collision Volume**: Increased aerial pickup hitbox ($1.85\,\text{m} \times 2.1\,\text{m}$) and sprite scale ($1.65\,\text{m}$) for effortless, satisfying collection during high-speed vertical dives.
- **Platform Runner Resumption**: Touching any Left or Right arrow relic immediately returns the player to the regular platformer runner sprinting in that direction, safely snapped to solid ground.

### [v0.5.7] - 2026-10-25 (Ladder Scene Direction Trigger Relics & Post-15s Exit Spawning)
- **Ladder Scene Direction Arrow Relics**: Direction trigger objects with clear glowing Left and Right arrowheads now actively spawn in the 3-Ladder Climbing (`UP`) scene after the 15-second survival trial completes.
- **Immediate Post-Trial Relic Injection**: Reaching 15 seconds immediately spawns Left and Right arrow relics on Lanes 0, 2, and 1 directly ahead of the player, accompanied by an announcement banner.
- **Touch & Return to Platformer Runner**: Collecting an arrowhead relic plays a sound, triggers floating warp text, and immediately transitions the character back to the horizontal platformer running in that chosen direction (Left or Right).
- **Procedural Ladder Relic Streaming**: Runtime ladder procedural generation dynamically mixes Direction Arrow Relics (40% probability) alongside Fruit Shields and Gold Coins once the trial is cleared.
- **Sky-Fall Parity & Leap Direction Tracking**: Extended identical post-15s directional exit relics to Sky-Fall and ensured outer-lane leaps off Lane 0 or Lane 2 set horizontal direction memory accurately.

### [v0.5.6] - 2026-10-25 (Left Chunk Persistence, Direction Arrows, Shield Fruits, Idle Start, Single Hero, and Sky-Fall Tuning)
- **Zero Mid-Air Floating on Scene Return**: `returnToPlatformerScene()` invokes `World::ensureGroundAround()` and conducts a vertical raycast across the platformer world, snapping the player flush to the highest solid tile or keeping standard airborne gravity over chasms.
- **Continuous Leftward World Streaming**: Clamped `leftGenEdge` and `rightGenEdge` to active world tile boundaries, ensuring seamless infinite procedural platform generation when returning or running leftward.
- **Direction Relic Arrowheads**: Custom generated spritesheet (`assets/direction_arrows.png`) renders distinct glowing cyan/gold/crimson/emerald directional arrowheads for Left, Right, Up, and Down triggers.
- **Fruit Pickups Grant 10s Energy Shield**: Separated fruit pickups from direction shifts; collecting fruits grants a 10-second temporary energy shield with animated bubble rendering and complete hazard deflection.
- **Idle Stance at Launch**: Character begins in idle breathing stance at game start and restarts; pressing `A`, `D`, or `Space` begins traversal.
- **Dedicated Single Hero**: Completely excised Ninja character assets and toggle logic to focus purely on Samurai Mack.
- **Sky-Fall Mini-Game Retuning**: Increased obstacle vertical spacing to $14.0\,\text{m}$, halved bird flock spawns, reduced spike mines to 33%, and tightened collision volumes for fair and readable diving gameplay.

### [v0.5.5] - 2026-10-25 (4 Thematic Background Sceneries, 1.5m Day/Night Cycle & 15s Scene Trial Cooldown)
- **Four Thematic Background Sceneries**: Seamless $512 \times 256$ parallax artwork for **Silvergrass Field**, **Autumn Foliage**, **Green Forest Foliage**, and **Urban Area** skyline with lit glowing windows. Cycle with **`[B]`** key.
- **Dynamic Day / Night Cycle (1.5 min / 90s, Starts at Night)**: Alternates every 1.5 minutes with smooth 15s dawn/dusk transitions. Features deep indigo night sky with 64 twinkling stars and silver moon disk, transitioning into radiant daylight with solar corona. Advance instantly with **`[N]`** key.
- **15-Second Scene Trial Cooldown**: Direction changes and exits inside 3-Ladder Climbing (`UP`) and Sky-Fall Diving (`DOWN`) scenes are locked for the first 15 seconds of gameplay, ensuring dedicated survival gameplay before direction warps unlock.

### [v0.5.4] - 2026-10-25 (Dash on SHIFT, Left Starting Platforms, 3-Parallel Ladder Climb Scene & Sky-Fall Diving Scene)
- **High-Velocity Dash (`SHIFT` Key)**: Instant $15.0\,\text{m/s}$ speed burst with multi-frame ghost trail afterimages and spike invulnerability phasing.
- **Left-Side Starting Platforms**: Structured initial chunks left of spawn ($x \in [-24, -2]$) with low, mid, and sky-canopy platforms, ladders, mystery relics, and underground cavern shaft.
- **3-Parallel Endless Ladder Scene (`SCENE_LADDER_CLIMB`)**: Initiated by UP direction changes. Climb 3 parallel endless ladder tracks, leap between tracks with `A`/`D` to avoid moving traps, and experience reverse `Death.png` resurrection animation upon trap hit before returning to run.
- **Sky-Fall Diving Scene (`SCENE_SKY_FALL`)**: Initiated by DOWN direction changes. Free-fall aerial dive with horizontal steering, dodging animated birds (`assets/bird.png`) and floating spikes. Trap hit displays `Take Hit.png` rotated $90^\circ$ anti-clockwise.
- **Scene-to-Scene Transitions**: Smooth bidirectional switching between Ladder Climbing, Sky-Fall diving, and the regular platform runner via directional inputs.

### [v0.5.3] - 2026-10-25 (Downward Ladder Entry & Procedural World Generation Analysis)
- **Downward Ladder Entry**: Pressing `S`/`Down Arrow` while standing near a ladder shaft now latches the player and initiates downward climbing into underground caverns.
- **Bidirectional World Confirmed Endless**: Confirmed via code analysis that the procedural world generates identically in both directions. Leftward movement IS infinite — each new 35-unit chunk is spawned on-demand.
- **Procedural vs. Static Resource Comparison**: Documented that procedural generation (~170 units in memory) is vastly more efficient than pre-generating a static infinite or very long level.

### [v0.5.2] - 2026-10-25 (Floating Text for Fruit Pickups & Disabling Invisible Auto-Shifts)
- **World-Space Floating Text Engine**: Extended `Renderer::drawText` to support camera view matrices, allowing text to float in real game-world coordinates with smooth rise and alpha fade.
- **Explicit Direction Warp Floating Text**: Collecting Mystery Fruit Relics now displays floating world text right above the fruit: `RELIC COLLECTED!\nWARP >>> RIGHT` (or whichever direction is chosen).
- **Coin Score Popups**: Collecting Gold Coins displays floating golden `+50` score indicators.
- **Invisible Pickups Addressed**: Isolated and disabled the autonomous background timer (`autoShiftEnabled = false`), ensuring direction shifts only trigger upon picking up visible Mystery Fruit Relics.
- **Autonomous Shift Toggle (`T` Key)**: Added `T` key toggle and Pause Menu option to re-enable autonomous timer-based direction shifts for players desiring extra chaos.

### [v0.5.1] - 2026-10-25 (Running & Ladder Climbing Animation Framerate Calibration)
- **Eliminated Double-Multiplication Framerate Glitch**: Removed compounded framerate scaling in `animTimer`, normalizing accumulation to real-world seconds.
- **Calibrated Running Playback ($8.0\,\text{FPS}$)**: Running animation now smoothly cycles at 8 FPS (125ms/frame), making leg strides, flowing clothes, and katana motion clear and readable.
- **Calibrated Ladder Climbing ($3.5\,\text{FPS}$)**: Climbing animation now cycles at 3.5 FPS (285ms/step), producing tactile, deliberate vertical hand-over-hand movement.
- **Calibrated Idle Breathing ($4.0\,\text{FPS}$)**: Combat stance breathing animation cycles at a calm, natural 4 FPS.

### [v0.5.0] - 2026-10-24 (Samurai & Ninja Animated Characters, ESC Pause Menu & On-Screen Score Counter)
- **Samurai & Ninja Animated Character Sprites**:
  - Replaced legacy LPC composite sprites with high-definition, hand-crafted pixel-art animations for **Samurai Mack** and **Shinobi Kenji**.
  - Smooth animation states for running right, flipped running left, jumping rise, falling descent, slope sliding dash, ladder climbing strides, breathing idle, and ground death defeat.
  - Interactive hero switching via the `C` key or Pause Menu with instant on-screen HUD confirmation.
- **Interactive ESC Pause Menu**:
  - Pressing `Esc` halts simulation physics and displays a glassmorphism modal dialog.
  - Live run statistics: total calculated score, distance traveled, coins collected, relics triggered, and active character.
  - Responsive menu controls: `[Esc/Space]` Resume, `[C]` Switch Hero, `[R]` Restart Run, `[Q]` Quit to Desktop.
- **On-Screen Score Counter HUD**:
  - Top-right corner HUD card featuring a glassmorphism panel with glowing golden border.
  - Dynamic score tally: $\text{Total Score} = \text{Distance (m)} \times 10 + \text{Coins} \times 50 + \text{Relics} \times 100$.
  - Displays spinning gold coin icon with collected count and traveled meters.
- **In-Memory 8x8 Bitmap Font Rendering**:
  - Zero-dependency embedded ASCII bitmap font renderer (`font8x8_basic.h`) uploaded directly as an OpenGL texture in `Renderer`.
  - Enables crisp, arbitrary-color vector-aligned text anywhere on screen with `gameRenderer.drawText()`.

### [v0.4.2] - 2026-10-24 (Modular Architecture, Ladder Latching & Unblocked Cavern Slopes)
- **Modular Code Architecture**: Decoupled the monolithic codebase into structured, focused modules:
  - [`/include/Common.h`](/include/Common.h): Shared constants, data models, physics tuning, and UV coordinate calculation helpers.
  - [`/include/Audio.h`](/include/Audio.h) & [`/src/Audio.cpp`](/src/Audio.cpp): In-memory WinMM synthesized procedural sound effects and playback.
  - [`/include/Renderer.h`](/include/Renderer.h) & [`/src/Renderer.cpp`](/src/Renderer.cpp): OpenGL shaders, reusable quad geometry, textures, and quad rendering pipeline.
  - [`/include/World.h`](/include/World.h) & [`/src/World.cpp`](/src/World.cpp): Dynamic multi-elevation procedural terrain generation, collision detection, and unblocked chasm management.
  - [`/src/main.cpp`](/src/main.cpp): Focused main game loop, player state machine, input handling, and HUD presentation.
- **Ladder Stopping & Auto-Latching**: When passing a ladder in the `DIR_UP` state (or pressing `W`/Up near a shaft), horizontal movement instantly stops ($v_x = 0$) and the player latches onto the ladder shaft to ascend cleanly.
- **Unblocked Underground Cavern Chasms**: Cavern dive slopes now carve out wide open surface chasms, explicitly preventing surface ground and elevated platforms from generating across the slope span so the path underground is never blocked.
- **Balanced, Enjoyable Gameplay Pacing**: Re-calibrated platformer physics for readable, comfortable pacing (`BASE_RUN_SPEED = 4.8 m/s`, `GRAVITY = -22.0 m/s^2`, `JUMP_BURST = 11.2 m/s`, `CLIMB_SPEED = 3.8 m/s`, `SLIDE_SPEED = 7.8 m/s`).

### [v0.4.1] - 2026-10-24 (Multi-Elevation Caverns, Sky Tier & Midpoint Interrupts)
- **Horizontal Direction Memory**: Ascending or descending ladder shafts now remembers the player's prior horizontal trajectory (`lastHorizontalDir`), smoothly resuming either Rightward sprint or Leftward backtracking upon exiting the shaft.
- **Midpoint Slide Jumping & Traversal Interrupts**: Mid-slope sliding can now be interrupted at any point by jumping (`Space`/`W`/Up) or steering (`A`/`D`), giving complete control over downward velocity.
- **Underground Cavern Zone ($Y = -4.5$)**: Added subterranean cavern corridors entered via downward dive slopes and surface chasms, featuring multiple upward ladder shafts leading back to intermediate ledges and the surface.
- **Super-Elevated Sky Canopy Tier ($Y = +5.5$)**: High-altitude sky platforms accessed via extended ladder shafts, concluding with grand downward slopes leading back to the surface platforms.
- **Locked Vertical Ladder Climbing**: Sidewise drift is completely eliminated while climbing ladders unless the player deliberately triggers a mid-ladder jump or dismount.

### [v0.4.0] - 2026-10-24 (Dynamic Endless World, Slopes, Tunnels, Sound FX & Corner HUD)
- **Truly Endless Procedural Platforms**: Replaced fixed layer structures with a randomized multi-tier platform generator producing randomized elevations (`-0.6`, `1.2`, `2.8`), randomized lengths, randomized gaps, and dynamic biomes.
- **Vertical Ladder Tunnel Shafts**: Added masonry-walled vertical shafts enclosing ladders with dark interior backdrops and guide glows, smoothly funneling player climbing up/down between tiers.
- **Steep Slide Slopes for Downward Traversal**: Implemented angled ramp physics connecting higher tiers to lower levels; entering a slope triggers high-speed sliding (`11.5 m/s`), tilted sprite animation, and rushing wind SFX.
- **In-Memory Procedural Audio Engine (`winmm`)**: Added zero-dependency synthesized PCM WAV audio for footsteps, jumps, ladder climb, slope slides, coin pickups, relic pickups, direction warp transitions, and spike death impacts.
- **Comprehensible Corner HUD Notifications**: Added glassmorphism top-left notification cards with rotating icons, category tags, titles, and explanations on every pickup.
- **Dynamic Biomes & Transition Phase**: Added 4 distinct themes (Emerald Forest, Frozen Glacier, Sunset Canyon, Mystic Cavern) with cinematic slow-mo transition flashes and smooth environmental color interpolation on direction shifts.

### [v0.3.0] - 2026-10-24 (Brackeys Platformer World, Relics, Ladders & Spikes)
- **Brackeys Asset Integration**: Integrated world tileset, platforms, animated coins, and fruit relics from the acclaimed Brackeys Platformer bundle (`assets/world_tileset.png`, `assets/platforms.png`, `assets/coin.png`, `assets/fruit.png`).
- **Direction-Shift Collectible Relics**: Implemented Mystery Fruit Relics that instantly trigger autonomous direction flips to a random direction (Right, Left, Up, or Down) on pickup with an animated HUD notification banner.
- **Vertical Traversal (Ladders & Pitfalls)**:
  - Added climbable wooden rung ladders rendered via Brackeys tileset.
  - Added intentional pitfalls and one-way drop-through platforms (`S` / Down) allowing players to deliberately transition down between multi-tier platform elevations.
- **Abyss Immunity (No Out-of-Bounds Deaths)**: Replaced bottomless abyss deaths with an endless bedrock safety foundation with textured grass and dirt tiles, ensuring players are never killed by falling out of bounds.
- **Lethal Spiky Traps**: Added hazardous spikes resting on platforms and pit floors; walking into or falling onto spikes triggers instant death and Game Over.
- **Animated Coin Collectibles**: Added 12-frame rotating gold coins along platform runways for score rewards (+50 pts).

### [v0.2.0] - 2026-10-24 (Phase 2: Side-View Platformer & Animated Sprites)
- **Side-View Platformer Engine**: Converted from top-down to side-view 2D world with gravity, jump physics, platform collision resolution, and abyss pitfall detection.
- **Animated Sprite Integration**: Integrated Liberated Pixel Cup (LPC) composite character sprite sheet (`assets/player_sheet.png`) and `stb_image.h` for seamless Modern OpenGL texture mapping.
- **Dynamic Animation States**: Implemented animated Run Right (Row 37), Run Left (Row 35), Climb (Row 30), Fall (Row 26 col 3), and Jump (Row 26 col 1).
- **Autonomous Direction Flipping**: Added timer-driven autonomous direction shifts at random intervals (3.0s–6.0s) between running right, left, and climbing, complete with on-screen visual alert banners.
- **Procedural Platform & Ladder World**: Added procedural generation of platforms at varying elevations, climbable ladders with rungs, ground hazard spikes, tall barrier blocks, and floating collectible gems.

### [v0.1.0] - 2026-10-17 (Phase 1 Complete)
- **Reusable Quad & Shader**: Implemented a unified $1 \times 1$ Quad VAO/VBO and MVP GLSL shader replacing individual hardcoded triangle geometry.
- **Delta-Time Game Loop**: Added frame-rate independent physics and rendering loop using GLFW time.
- **Vector-Adaptive Movement**: Added forward traversal with lateral lane shifting and jumping relative to the active movement axis.
- **Procedural Obstacles & AABB Collision**: Added dynamic track generation, multiple obstacle types (hurdles vs barrier blocks), and AABB overlap detection with game reset states.
- **The "Wending" Mechanic**: Implemented 90-degree orthogonal track pivot points and smooth rotational camera follow using `glm::mix`.
- **Project Structure & Rules**: Bundled all necessary third-party libraries (`GLFW`, `GLAD`, `GLM`), created `Makefile`, `CMakeLists.txt`, `.gitignore`, and `AGENTS.md`.

