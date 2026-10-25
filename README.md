
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
g++.exe -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32

# Run the game
./build/main.exe

# Package standalone portable game distribution zip (~5.8 MB):
mingw32-make package
```

### Controls
- **`A` / `D` (or Left / Right Arrow)**: **Walk & Run (Free Exploration - DEFAULT)** — Explore the world at your own pace! Samurai Mack stands still in idle stance when keys are released.
- **`F`**: **Toggle Auto-Runner Mode** — Switches between default manual Free Exploration and continuous autonomous sprinting platformer mode.
- **`M`**: **Mute / Unmute Background Music** — Toggles looping background music (`Quarter_in_the_Slot.mp3`, default 50% volume) with real-time HUD notification.
- **`J` / `X` / `Left Mouse Click`**: **Sword Slash Attack** — Swing katana to defeat Bringer of Death (1 hit), Initial Slime (2 hits), and Demon Slime Boss (5 hits)!
- **`Right Mouse Click` / `K`**: **Defend / Parry Stance** — Raise katana in defensive guard for up to 1.0s using `Defend_nobg.png`! Absorbs all enemy attacks (Slime, Bringer of Death scythe, Thunder cloud spell, Demon Boss cleave/smash) with a resonant metallic parry clang (`SFX_BLOCK`), floating `[PARRY]` text, and 0.6s grace invulnerability (2.0s cooldown on deflection).
- **`Space` / `Esc` / `Enter` (During Intro)**: **Skip Intro Video** — Immediately interrupt the opening cinematic and advance to game spawn with tutorial modal.
- **`Left Shift`**: **Dash Movement** — High-speed burst with ghost trail afterimages and spike/hazard invulnerability!
- **`Space` / `W` / `Up Arrow`**: Jump across gaps and over spiky traps, or latch on and climb upward inside ladder shafts
- **`S` / `Down Arrow`**: Drop down through one-way platforms or descend into underground ladder shafts
- **`Esc`**: Open / Close **Pause Menu** (pause physics simulation, inspect stats, restart, or quit)
- **`B`**: **Cycle Background Scenery** (Silvergrass Field <-> Autumn Foliage <-> Green Forest Foliage <-> Urban Area)
- **`N`**: **Fast-Forward Day / Night Cycle** (Instantly advance 90-second half-cycle between Day and Night)
- **`T`**: **Open Tutorial Window** — View the comprehensive How-to-Play guide modal dialog during active exploration or inside the Pause Menu.
- **`Y`**: **Toggle Shift Mode** (`ENABLED` timer shifts every 6.5s <-> `DISABLED` direction relics only)
- **`R`**: Quick restart run (when paused or after Game Over)
- **`Q`**: Quit game to desktop (from Pause Menu or Game Over screen)
- **3-Lives Health System & Heart Relics**: The player possesses 3 life pips displayed on the HUD. Collect ruby heart relics (spawned alongside shields) to restore lost lives, or gain +150 bonus overheal points.
- **Roaming Enemies & Boss Fight**:
  - **Bringer of Death**: Roaming scythe-wielding wraith. Now accurately pixel-centered on platforms. Casts ranged thunder spells targeting the player across multi-layer elevations (underground cavern to overground surface/sky canopy). Features a generous 1.0s telegraphed gathering cloud window allowing players to dodge before lightning strikes with `thunder-after-crackle.mp3` SFX! Cleave in **1 hit** before it attacks.
  - **Enemy Separation**: Bringer of Death and Slime spawns now maintain a guaranteed 12.0m horizontal distance buffer, preventing cluttered spawn clumping.
  - **Initial Slime & Boss Demon Slime**: Defeating the green slime in **2 hits** triggers a sequenced 3-phase audio metamorphosis ($5.5\text{s}$) with wet splat impact, acid bubbling, and demon roar. The Boss Demon Slime leaps between platforms with ground smash attacks that produce a heavy distant bang on impact. Slay the boss in **5 hits** or dash away to despawn!
- **Grouped Platform Spikes**: Metallic spikes now spawn in challenging clusters of 2 to 4 spikes on elevated platforms, requiring careful jumps or dash bursts.
- **Mini-Game Scene Exits**: Mini-game scenes (3-Ladder Climbing and Sky-Fall Diving) can strictly **only** be exited by collecting Direction Arrow Relics (which unlock and spawn after the 15-second survival trial) or by taking an obstacle hit (resurrecting back on the main platformer). Manual scene switching via key presses is disabled.
- **Underground Ladder Shafts**: Approach any underground ladder shaft from the surface and press `S` or `Down Arrow` to smoothly latch on and climb down into the subterranean caverns.
- **Idle Stance at Start**: The Samurai starts in a grounded idle breathing stance upon game launch and restart until directional movement keys (`A`, `D`) or `Space` are pressed.
- **Direction Arrow Relics**: Distinct glowing neon arrowheads (Left, Right, Up, Down) explicitly indicate which direction change or scene transition they trigger when touched.
- **Heraldic Shield Pickups (10s Energy Shield)**: Collecting the glowing golden-cyan shield icon (`assets/shield_pickup.png`) grants a 10-second circular energy shield bubble that deflects spikes, floating mines, and raptor birds!
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

refer to [changelog.md](doc/changelog.md)