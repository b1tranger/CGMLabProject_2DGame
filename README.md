
# The Wenderer: A Multi-Directional Endless Runner

### 🎓 Academic Project Information
- **Course Title**: Computer Graphics & Multimedia Lab
- **Course Code**: CSE0611328
- **Department**: Department of Computer Science & Engineering (CSE)
- **Semester**: Autumn 2026
- **Batch**: 55

---

### 👨‍🏫 Submitted to:
| Teacher's Name | Designation |
| :--- | :--- |
| **Engineer Robiul Islam** | Assistant Professor |
| **Tasneen Tanvir** | Lecturer |

### 👥 Submitted by (Group Details):
| Student's Name | Student ID | Forks | 
| :--- | :--- | - |
| **Gaus Saraf Murady** | `0432410005101088` | Root |
| **Lima Akter** | `0432410005101063` | [Link](https://github.com/Badhon63/CGMLabProject_2DGame)  |
| **Fahima Akter** | `0432410005101093` | |

---

## 🎮 About The Project

**The Wenderer** is a 2D side-view endless runner and platformer built in **C++** using **Modern OpenGL (3.3 Core Profile)**. 

Unlike traditional static runners, **The Wenderer** features autonomous dynamic direction changes (suddenly flipping between running right, left, and climbing ladders) in random intervals, paired with a smoothly reframing side-view camera, animated character sprites (running, climbing, falling, jumping), procedural platforms, ladders, spikes, barriers, and collectible gems.

---

## 🚀 Quick Start (Build & Run)

### Requirements
- MinGW-w64 (`g++` supporting C++17) or CMake
- All OpenGL and imaging libraries (`GLFW`, `GLAD`, `GLM`, `stb_image`) are bundled in the repo!

### Building with Make (Recommended)

Depending on your environment (MSYS2, Git Bash, or standalone MinGW), the GNU make executable is named `make` or `mingw32-make`:

```bash
# Build the project (creates ./build/ and compiles main.exe)
make
# (or: mingw32-make)

# Run the game:
# In PowerShell / Bash:
./build/main.exe
# In Windows Command Prompt (cmd.exe):
build\main.exe

# Or compile and launch immediately in one step:
make win
# (or: mingw32-make win)

# Package standalone portable game distribution zip:
make package
# (or: mingw32-make package)
```

### Direct Compilation with g++ (No Make required)

If `make` is not available, you can compile directly. Note that `./build/` must exist and include the runtime DLLs:

**In PowerShell:**
```powershell
if (!(Test-Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }; Copy-Item lib/*.dll build/ -Force
g++.exe -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32
./build/main.exe
```

**In Windows Command Prompt (cmd.exe):**
```cmd
if not exist build mkdir build
copy /y lib\*.dll build\
g++.exe -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32
build\main.exe
```

### Controls
- **`A` / `D` (or Left / Right Arrow)**: **Walk & Run (Free Exploration - DEFAULT)** — Explore the world at your own pace! Samurai Mack stands still in idle stance when keys are released.
- **`F`**: **Toggle Auto-Runner Mode** — Switches between default manual Free Exploration and continuous autonomous sprinting platformer mode.
- **`M`**: **Mute / Unmute Background Music** — Toggles looping background music (`Quarter_in_the_Slot.mp3`, default 40% volume) with real-time HUD notification.
- **Music Volume Slider (`ESC` Pause Menu / `-` & `=`)**: Drag the interactive Pause Menu volume slider (or press `Left`/`Right`, `-`/`=`, or click `[-]`/`[+]`) to adjust BGM volume in **10% increments** (0%–100%, default **40%**).
- **`J` / `X` / `Left Mouse Click`**: **Sword Slash Attack** — Swing katana to defeat Bringer of Death (1 hit), Initial Slime (2 hits), and Demon Slime Boss (5 hits)!
- **`Right Mouse Click` / `K`**: **Defend / Parry Stance** — Raise katana in defensive guard for up to 1.0s using `Defend_nobg.png`! Absorbs all enemy attacks (Slime, Bringer of Death scythe, Thunder cloud spell, Demon Boss cleave/smash) with a resonant metallic parry clang (`SFX_BLOCK`), floating `[PARRY]` text, and 0.6s grace invulnerability (2.0s cooldown on deflection).
- **`Space` / `Esc` / `Enter` (During Intro Splash & Video)**: **Skip Intro Screen & Video** — Instantly advance from the academic course/group intro splash screen or skip the cinematic video directly into gameplay.
- **`Left Shift`**: **Dash Movement** — High-speed burst with ghost trail afterimages and spike/hazard invulnerability!
- **`Space` / `W` / `Up Arrow`**: Jump across gaps and over spiky traps, or latch on and climb upward inside ladder shafts
- **`S` / `Down Arrow`**: Drop down through one-way platforms or descend into underground ladder shafts
- **`Esc`**: Open / Close **Pause Menu** (pause physics simulation, inspect stats, adjust BGM slider, restart, or quit)
- **`F11`**: **Toggle Fullscreen** — Expands the horizontal field of view across the full screen width with vertical black letterbox borders and zero stretching.
- **`B`**: **Cycle Background Scenery** (Silvergrass Field <-> Autumn Foliage <-> Green Forest Foliage <-> Urban Area)
- **`N`**: **Fast-Forward Day / Night Cycle** (Instantly advance 90-second half-cycle between Day and Night)
- **`T`**: **Open Tutorial Window** — View the comprehensive How-to-Play guide modal dialog during active exploration or inside the Pause Menu.
- **`Y`**: **Toggle Shift Mode** (`ENABLED` timer shifts every 6.5s <-> `DISABLED` direction relics only)
- **`R`**: Quick restart run (when paused or after Game Over)
- **`Q`**: Quit game to desktop (from Pause Menu or Game Over screen)
- **3-Lives Health System & Heart Relics**: The player possesses 3 life pips displayed on the HUD. Collect ruby heart relics (spawned alongside shields) to restore lost lives, or gain +150 bonus overheal points.
- **Roaming Enemies, Time-Escalating Spawn Phases & Boss Fight**:
  - **3-Phase Dynamic Enemy Spawning**: Enemy spawn rate escalates automatically every **1 minute** (`ENEMY_PHASE_DURATION = 60s`):
    - **Phase 1 (0–1m)**: Rare enemy spawns, capped at a **maximum of 2 active enemies** at once.
    - **Phase 2 (1–2m)**: Increased spawn rate (up to 6 active enemies across chunks and periodic patrols).
    - **Phase 3 (2m+)**: Peak spawn density (**up to 5 Bringers of Death & 5 Slimes** per chunk, up to 14 active enemies) plus **Horde Spawn** — randomly spawns **3 Slimes** and **3 Bringers of Death** around the player on a **3-minute cooldown** (`180s`).
  - **Bringer of Death**: Roaming scythe-wielding wraith. Accurately pixel-centered on platforms. Casts ranged thunder spells targeting the player across multi-layer elevations (underground cavern to overground surface/sky canopy) with a 1.0s telegraphed gathering cloud window before lightning strikes (`thunder-after-crackle.mp3`). Cleave in **1 hit**.
  - **Enemy Separation**: Bringer of Death and Slime spawns maintain a 9.0m horizontal distance buffer on the same elevation tier, preventing cluttered spawn clumping.
  - **Initial Slime & Boss Demon Slime**: Defeating the green slime in **2 hits** triggers a sequenced 3-phase audio metamorphosis ($5.5\text{s}$) with wet splat impact, acid bubbling, and demon roar. The Boss Demon Slime leaps between platforms with ground smash attacks. Slay the boss in **5 hits** or dash away to despawn!
- **Grouped Platform Spikes**: Metallic spikes now spawn in challenging clusters of 2 to 4 spikes on elevated platforms, requiring careful jumps or dash bursts.
- **Mini-Game Scene Exits**: Mini-game scenes (3-Ladder Climbing and Sky-Fall Diving) can strictly **only** be exited by entering an **Open Exit Gate** (`assets/exit_gate.png`, which spawns after the 15-second survival trial) or by taking an obstacle hit (resurrecting back on the main platformer). Manual scene switching via key presses is disabled.
- **Underground Ladder Shafts**: Approach any underground ladder shaft from the surface and press `S` or `Down Arrow` to smoothly latch on and climb down into the subterranean caverns.
- **Idle Stance at Start**: The Samurai starts in a grounded idle breathing stance upon game launch and restart until directional movement keys (`A`, `D`) or `Space` are pressed.
- **Direction Arrow Relics (Regular Run Scene)**: In the regular platformer scene, glowing **Up** (green, Ladder Ascent) and **Down** (gold, Sky-Fall Dive) arrowheads spawn on elevated platforms (redundant Left/Right direction relics removed). Inside mini-games, **Open Exit Gate** relics return you to the platform runner.
- **Heraldic Shield Pickups (10s Energy Shield)**: Collecting the glowing golden-cyan shield icon (`assets/shield_pickup.png`) grants a 10-second circular energy shield bubble that deflects spikes, floating mines, and raptor birds!
- **Steep Slopes**: Walk or fall onto an angled slope to automatically engage controllable downhill **Sliding**! (Press `Space` to jump out at any midpoint)
- **Ladder Tunnels**: Approach a ladder shaft to immediately stop horizontal run and ascend/descend
- **Touch / Collect Gold Coin**: Scores bonus points (+50 pts) with floating `+50` text popups, real-time corner HUD feedback, and on-screen score tracking

---

## 📚 Documentation & Guides

- **[Complete Project Changelog](/doc/changelog.md)**: Detailed chronological history of all releases mapped to session prompt archives.
- **[Technical Progress Documentation](/doc/documentation.md)**: Comprehensive architectural, mathematical, and subsystem specification.
- **[Academic Code Guide & Explanations](/doc/explain.md)**: Beginner-friendly academic guide explaining components in simple terms for lab presentations & vivas.
- **[Presentation Guide & Video Narration Script](/doc/PRESENTATION_GUIDE.md)**: Pre-gameplay spoken script, visual B-roll storyboard, live demo playbook, and rapid viva Q&A cheat sheet.
- **[Learning & Implementation Guide](/doc/LEARNING_GUIDE.md)**: Beginner-friendly step-by-step breakdown of OpenGL concepts, mathematics, and guided milestones.
- **[Project Proposal](/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf)**: Formal proposal for the CGM Lab course.
- **[Research Analysis](/doc/research/Platformer%20Game%20Research%20Analysis.pdf)**: Architectural, ludonarrative, and market research.
- **[Implementation Steps](/doc/research/implementationSteps.pdf)**: Core roadmap and code milestones.

---

## 📜 Version History (Changelog)

refer to [changelog.md](doc/changelog.md)