# The Wenderer: Academic Code Guide & Component Explanation 🎓

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Purpose**: Beginner-friendly academic guide explaining the structure, components, and concepts of **The Wenderer** in simple, intuitive terms for project presentations and lab viva examinations.

---

## 1. What is "The Wenderer"? (The Big Picture)

Most endless runner games (like *Subway Surfers* or *Temple Run*) and platformers (like *Super Mario*) follow one simple rule: the player always moves in one single forward direction.

**The Wenderer** changes this formula:
- The word **"Wend"** means *to travel along an altered course or change direction*.
- In this game, your character is forced to continuously adapt: the game dynamically changes direction—suddenly sprint right, flip backwards to run left, climb up vertical shafts, or slide down steep ramps into subterranean caves.
- It is a game about **adaptability**: surviving unpredictable shifts in direction and elevation while dodging spiky hazards, climbing vertical tunnels, and collecting bonus items.

---

## 2. Understanding the 5 Building Blocks of the Code

Instead of having thousands of lines of code dumped into one confusing file, the project is divided into **5 clean, specialized building blocks**. Think of them like different departments in a game studio:

```
┌─────────────────────────────────────────────────────────────────┐
│                           main.cpp                              │
│            (The Game Director / Conductor of the Show)          │
└────────┬──────────────┬──────────────────┬──────────────┬───────┘
         │              │                  │              │
         ▼              ▼                  ▼              ▼
   Common.h         Audio.cpp         Renderer.cpp     World.cpp
  (The Rulebook)  (The Musician)      (The Artist)   (The Architect)
```

---

### Block 1: `Common.h` — The Rulebook & Dictionary 📖
* **What is it?**  
  This file is the single shared dictionary for the entire project. It contains all the definitions, physics numbers, and data structures used across the game.
* **What does it do in simple terms?**
  - **Sets the Game Rules**: Defines how strong gravity is, how high the character jumps, how fast the character runs, and where the ground levels are located.
  - **Defines Game Objects**: Explains to the computer what a `PlatformTile`, a `LadderTunnel`, a `SteepSlope`, an `AnimatedCoin`, and a `SpikeTrap` look like (their coordinates, sizes, and behaviors).
  - **Sets the UV Coordinate Math**: Explains how to find specific frames on our character and world sticker sheets.

---

### Block 2: `Audio.h` / `Audio.cpp` — The In-Memory Sound Synthesizer 🎵
* **What is it?**  
  The department responsible for all sound effects (footsteps, jumps, climbing taps, coin chimes, sliding whoosh, and game-over crunches).
* **What makes it special?**
  - **No Audio Files on Disk**: The project does not load external `.mp3` or `.wav` files. External files can get lost, fail to load, or require heavy third-party audio software.
  - **Math-Generated Sounds**: Every sound effect is generated directly from mathematical formulas (sine waves and noise) in computer memory. For example:
    - A **Jump** is a sound wave that rapidly slides from low pitch to high pitch ($190 \rightarrow 620\,\text{Hz}$).
    - A **Coin** is a pair of bright crystal musical chimes.
    - A **Slope Slide** is rushing white noise mimicking air friction.
  - **Non-blocking Playback**: Sounds play in the background using the standard Windows Multimedia system without pausing or stuttering the game.

---

### Block 3: `Renderer.h` / `Renderer.cpp` — The Digital Artist & Screen Projector 🎨
* **What is it?**  
  The graphics engine. It takes objects from the game and draws them onto the computer screen using Modern OpenGL.
* **How does it work in simple terms?**
  - **The "One Single Square" Trick**:  
    Instead of sending millions of vertices to the graphics card every frame, the engine keeps **one single $1 \times 1$ square** in the graphics card's memory. To draw any platform, coin, or character, it simply moves, stretches, and colors that single square.
  - **The "Sticker Sheet" Trick (Sprite Sheets & UV Mapping)**:  
    All our character animations (running, climbing, jumping, falling) are stored on one single image sheet. The renderer cuts out the exact frame needed at that exact millisecond and paints it onto the square.
  - **The Pixel-Art Transparency Filter**:  
    When drawing tiles, it automatically discards transparent backgrounds so images have clean, sharp pixel-art edges without ugly black boxes.

---

### Block 4: `World.h` / `World.cpp` — The Stage Builder & Architect 🏗️
* **What is it?**  
  The procedural world generator. It continuously builds the stage ahead of the player as they run.
* **What does it do in simple terms?**
  - **The Endless Illusion**:  
    The computer cannot store an infinite level in memory. Instead, `World.cpp` watches where the player is. As you run forward, it randomly generates new platforms, ladders, and slopes ahead of you, while deleting old platforms far behind you. This allows the game to run forever without slowing down or running out of memory.
  - **Carving Open Cavern Chasms**:  
    When it creates a steep slope leading down into an underground cave, it intentionally cuts away any surface grass or platform tiles above it so the entrance to the cave is completely open and unblocked.
  - **Collision Checking (AABB)**:  
    It checks whether the player's rectangular bounding box overlaps with a platform (to stand on it), a coin (to collect it), or a spike (to trigger Game Over).

---

### Block 5: `main.cpp` — The Game Director & Conductor 🎬
* **What is it?**  
  The main engine that ties all the pieces together. It runs 60 times every second in a loop called the **Game Loop**.
* **What does it do in simple terms?**
  1. **Listens to You**: Detects keyboard presses (`Space` to jump, `W`/`S` to climb, `A`/`D` to steer).
  2. **Updates Physics**: Applies gravity, updates the player's position, checks for ladder climbing, and manages slope sliding.
  3. **Manages Traversal Memory**: Remembers which way you were running before you climbed a ladder, so when you reach the top, you keep running in that direction instead of being forced into an arbitrary turn.
  4. **Smooth Camera**: Smoothly slides the camera to keep the player nicely centered on the screen.
  5. **HUD Corner Cards**: Pops up informative cards in the top-left corner whenever you collect a coin or mystery relic.

---

## 3. How Traversal Works (Everyday Life Analogies)

```
[ SKY CANOPY TIER ]           Y = +5.5m   (Floating platforms in the sky)
         ▲                                      │
         │ (Ladder Shaft)                       ▼ (Grand Downhill Slide)
[ SURFACE PLATFORMS ]         Y = +1.5m - 2.8m (Elevated wooden/grass slabs)
         ▲                                      │
         │ (Ladder Shaft)                       ▼ (Cavern Dive Slope)
[ SURFACE GROUND LEVEL ]      Y = -0.5m   (Main grass floor with open chasms)
         ▲                                      │
         │ (Multiple Ladders)                   ▼ (Steep Slope)
[ UNDERGROUND CAVERN ]        Y = -4.5m   (Dark subterranean cave corridors)
─────────────────────────────────────────────────────────────────────────────
[ BEDROCK FOUNDATION ]        Y = -7.5m   (Safety net: Zero out-of-bounds fall deaths!)
```

### 1. Moving Up: Vertical Ladder Shafts
- **Like an Elevator Shaft**: When you run toward a ladder while set to climb (or when pressing `W`), the game immediately stops your horizontal running momentum and funnels you straight up the shaft.
- **Direction Memory**: If you were sprinting Left before climbing, the game remembers this. Once you step off the top of the ladder, you automatically resume running Left!

### 2. Moving Down: Steep Slopes
- **Like a Playground Slide**: When you walk or fall onto a steep ramp, your character leans into the incline and slides downward at high speed with a rushing wind sound effect.
- **Mid-Slide Jump Interrupt**: You are never trapped in a slide. If you see a spiky trap at the bottom, pressing `Space` lets you jump out of the slide at any midpoint!

### 3. The 4 Floors & Safety Net
- **Sky Canopy ($Y \approx +5.5\,\text{m}$)**: A high-altitude playground with coins and fruit relics. At the end, a grand downhill slope lets you slide all the way back to the surface.
- **Underground Cavern ($Y = -4.5\,\text{m}$)**: A mysterious cave beneath the surface. To get back up, multiple ladders connect the cave floor back to intermediate ledges and surface openings.
- **Bedrock Safety Floor ($Y = -7.5\,\text{m}$)**: In traditional games, falling off the screen causes you to die in an invisible pit. In **The Wenderer**, you **never** die from falling out of bounds. The bedrock floor always catches you. The only lethal danger is landing on **spiky traps**!

---

## 4. Questions You Might Be Asked in a Lab Viva / Evaluation

Here are sample questions an instructor, examiner, or professor might ask about this project, along with simple, accurate answers:

### Q1: Why did you divide the code into multiple files instead of leaving everything in `main.cpp`?
> **Answer**:  
> In software engineering, this is called **Separation of Concerns**. Having everything in one 2,000-line file makes it hard to debug and read. By separating the code into `Audio`, `Renderer`, `World`, `Common`, and `main`, each file has one single job. If we want to change how sound works, we only touch `Audio.cpp`. If we want to change platform generation, we only touch `World.cpp`.

### Q2: How does the game run endlessly without running out of computer memory?
> **Answer**:  
> Through **Procedural Generation and Dynamic Pruning**. The game only creates the level in small chunks around the player. As the player runs, new chunks generate ahead, while chunks farther than 85 meters behind the player are deleted from memory. This keeps memory usage completely flat and constant, allowing the game to run forever.

### Q3: Why did you synthesize sound effects with math instead of loading audio files?
> **Answer**:  
> Procedural audio eliminates external file dependencies. Loading `.mp3` or `.wav` files requires extra audio libraries, and if an audio file is missing or in the wrong folder, the game can fail. By calculating sound waves directly with math (PCM audio buffers) and passing them to the Windows Multimedia system (`PlaySoundA`), the game is 100% self-contained and loads instantaneously.

### Q4: How does the computer detect when the player touches a coin, spike, or platform?
> **Answer**:  
> We use **AABB (Axis-Aligned Bounding Box) Collision Detection**. Every object is treated as an upright rectangle with a minimum and maximum $X$ and $Y$ coordinate. If the player's rectangle overlaps horizontally and vertically with an object's rectangle, a collision is triggered.

### Q5: What is the benefit of Modern OpenGL over Older Immediate Mode (`glBegin`/`glEnd`)?
> **Answer**:  
> Immediate Mode sent vertex coordinates across the computer bus to the GPU one by one every single frame, which was very slow. Modern OpenGL (3.3 Core) stores vertex geometry directly in GPU memory using buffers (VBOs) and controls them using GPU programs called **Shaders**. This allows drawing thousands of objects at 60+ FPS with minimal CPU overhead.

### Q6: How does the player know what happened when picking up an item?
> **Answer**:  
> We implemented a **Corner HUD Notification Card**. When an item is picked up, an animated glassmorphism card slides in at the top-left corner, displaying the rotating item icon, category tag, title, and a clear explanation (e.g. *"+50 GOLD COIN"* or *"MYSTERY RELIC: DIRECTION SHIFT UP"*).
