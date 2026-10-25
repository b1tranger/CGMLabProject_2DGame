# The Wenderer: Academic Code Guide & Component Explanation 🎓

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Purpose**: Beginner-friendly academic guide explaining the structure, components, graphics pipeline, and mechanics of **The Wenderer** in simple, intuitive terms for project presentations and lab viva examinations.

---

## 1. What is "The Wenderer"? (The Big Picture)

Most endless runner games (like *Subway Surfers* or *Temple Run*) and platformers (like *Super Mario*) follow one static rule: the player always moves in one single forward direction.

**The Wenderer** changes this formula:
- The word **"Wend"** means *to travel along an altered course or change direction*.
- In this game, your character is forced to continuously adapt: the game dynamically changes direction—sprint right, flip backwards to run left, climb vertical ladder shafts, or slide down steep ramps into subterranean caves.
- It is a game about **adaptability**: surviving unpredictable shifts in direction and elevation while dodging spiky hazards, climbing vertical tunnels, collecting bonus relics, and defending against mythical roaming enemies.

---

## 2. Understanding the Modular Building Blocks of the Code

Instead of dumping thousands of lines of code into one confusing file, the project is divided into **modular, specialized building blocks**. Think of them like different departments in a game studio:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                                  main.cpp                                   │
│                 (The Game Director / Conductor of the Show)                 │
└──────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬────┘
       │          │          │          │          │          │          │
       ▼          ▼          ▼          ▼          ▼          ▼          ▼
   Common.h   Renderer    World      Audio     UIManager LadderScene SkyFallScene
 (Rulebook)   (Artist) (Architect) (Musician)  (Presenter) (Climber)   (Skydiver)
```

---

### Block 1: `Common.h` — The Rulebook & Dictionary 📖
* **What is it?**  
  The single shared header file included by every component in the engine. It contains game constants, physics values, and data structures.
* **What does it do in simple terms?**
  - **Sets the Game Rules**: Defines gravity ($g = -22.0\,\text{m/s}^2$), jump impulse ($v_y = 11.2\,\text{m/s}$), horizontal run speed ($4.8\,\text{m/s}$), climb speed, slide speed, and elevation strata.
  - **Defines Game Objects**: Defines structs for `PlatformTile`, `LadderTunnel`, `SteepSlope`, `SpikeTrap`, `DirectionRelic`, `AnimatedCoin`, `Enemy`, and `SpellHazard`.
  - **Sets the UV Math Helpers**: Provides helper functions (`getTileUV`, `getCoinUV`, `getFruitUV`, `getShieldUV`) that calculate normalized coordinates on sprite sheets.

---

### Block 2: `Renderer.h` / `Renderer.cpp` — The Digital Artist & Screen Projector 🎨
* **What is it?**  
  The graphics engine. It takes world objects and draws them onto the computer screen using Modern OpenGL (3.3 Core Profile).
* **How does it work in simple terms?**
  - **The "One Single Square" Trick (Instanced Quad Rendering)**:  
    Instead of constantly allocating thousands of triangles, the graphics card stores **one single $1 \times 1$ unit square** (2 triangles, 6 vertices) in GPU memory (VAO/VBO). To draw any platform, coin, or character, the shader scales, rotates, and moves that single square using matrix mathematics.
  - **The GLSL Shaders**:  
    Custom GPU programs. The **Vertex Shader** transforms coordinates using the Model-View-Projection ($P \times V \times M$) matrix. The **Fragment Shader** samples colors from image textures and discards transparent pixels.
  - **Circular Energy Shield Math**:  
    Renders a glowing energy shield bubble around the player by calculating pixel distance from quad center in the fragment shader: $r = \sqrt{x^2 + y^2}$. Pixels with $r > 0.5$ are discarded, creating a perfect circular bubble without needing round 3D geometry!

---

### Block 3: `World.h` / `World.cpp` — The Stage Builder & Architect 🏗️
* **What is it?**  
  The procedural world generator. It continuously builds the stage ahead of the player as they explore.
* **What does it do in simple terms?**
  - **The Endless Illusion**:  
    The computer cannot store an infinite level in memory. `World.cpp` creates procedural chunks 35 meters ahead in both directions ($+X$ and $-X$) while automatically deleting chunks farther than 85 meters behind the player. This keeps memory flat and constant.
  - **Multi-Elevation Strata**:  
    Generates 4 distinct vertical zones: Sky Canopy Tier ($Y \approx +5.5\text{m}$), Surface Platforms ($Y \approx +1.0$ to $+2.8\text{m}$), Surface Ground ($Y = -0.5\text{m}$), and Underground Caverns ($Y = -4.5\text{m}$).
  - **Bedrock Safety Floor**:  
    A continuous bedrock foundation at $Y = -7.5\text{m}$ guarantees zero out-of-bounds pit deaths. The only lethal hazards are spiky traps and enemy attacks!
  - **Chasm Carving & Enemy Spacing**:  
    Carves natural openings above steep cave dive slopes and enforces a guaranteed 12-meter separation between Bringer of Death and Slime spawns to prevent enemy crowding.

---

### Block 4: `Audio.h` / `Audio.cpp` — The Hybrid Sound Engine 🎵
* **What is it?**  
  The department responsible for sound effects, creature voices, and background music.
* **What makes it unique?**
  - **In-Memory Procedural Synthesizer**:  
    Action sound effects (swords, shields, parries, footsteps, ladder taps, jumps, coin chimes) are generated directly from **mathematical formulas** (sine waves, harmonic envelopes, and white noise) into RAM buffers played via Windows Multimedia (`PlaySoundA`). No audio files can ever go missing or fail to load!
    - **Sword Slash (`SFX_SLASH`)**: Sharp aerodynamic whoosh ($2400 \rightarrow 320\,\text{Hz}$) with steel ring harmonics ($3250\,\text{Hz}, 4950\,\text{Hz}$).
    - **Katana Defend / Parry Clang (`SFX_BLOCK`)**: Sharp metallic impulse click plus dual resonant ringing bells ($2850\,\text{Hz}$ and $4150\,\text{Hz}$) with slow decay.
  - **Hardware-Accelerated Streaming (Windows MCI)**:  
    For longer compositions, Windows Media Control Interface (`mciSendStringA`) streams background music (`Quarter_in_the_Slot.mp3` with `[M]` mute control) and multi-phase creature audio (3-phase Demon Slime metamorphosis and thunder strikes).

---

### Block 5: `UIManager.h` / `UIManager.cpp` — The Presenter & HUD System 📊
* **What is it?**  
  A dedicated thread-safe singleton managing on-screen telemetry, floating combat text, modals, and menus.
* **What does it handle?**
  - **Score & Health HUD**: Displays score, coins, distance, 3 animated heart life pips, and real-time Guard/Defend status (`[GUARD: ACTIVE]` or cooldown `[DEF: X.Xs]`).
  - **Floating World-Space Combat Text**: Animates rising numbers and badges (`+50`, `[PARRY]`, `CRITICAL!`, `BLOCKED!`).
  - **Overhead Enemy Health Bars**: Segmented health bars floating above roaming enemies showing exact HP slices.
  - **Pause Menu & Tutorial Modal**: Glassmorphic modal overlays explaining game lore, full controls, and gameplay rules.

---

### Block 6 & 7: `LadderScene` & `SkyFallScene` — Mini-Game Trials 🧗🪂
* **What are they?**  
  Dedicated mini-game scenes that challenge the player whenever a vertical direction shift occurs:
  - **`LadderScene` (Upward Shift)**: 3 parallel climbing tracks with alternating sawblades, moving spikes, and climbing shields.
  - **`SkyFallScene` (Downward Shift)**: High-speed aerial free-fall dive with steering controls to dodge raptor birds and floating spike mines.
  - **Exit Predictability**: Both trials feature a 15-second survival timer (`SURVIVE UNTIL EXIT: XX.Xs`) and a prominent glowing `=== EXIT APPEARED! ===` banner before directional arrow relics spawn to return the player to the runner.

---

### Block 8: `VideoPlayer.h` / `VideoPlayer.cpp` — The Cinematic Intro 🎥
* **What is it?**  
  An in-engine hardware-accelerated video player using Microsoft Media Foundation (`MFStartup`, `IMFSourceReader`).
* **What does it do?**
  - Streams and decodes the introductory cinematic video (`intro.mp4`) directly into an OpenGL texture.
  - Synchronizes video presentation timestamps (PTS) with audio playback and allows skipping via `Space`, `Enter`, or `Esc`.

---

## 3. How 2D Sprites Work in Modern OpenGL (The Complete Guide) 🖼️

A common question in Computer Graphics lab vivas is: **"OpenGL is a 3D graphics library. How does it draw 2D pixel-art characters and animations?"**

Here is the step-by-step breakdown of how 2D sprites work under the hood:

```
[PNG Image File on Disk]
       │
       ▼ (stb_image decodes pixel bytes)
[Raw RGBA Buffer in CPU RAM]
       │
       ▼ (glTexImage2D copies pixels to GPU memory)
[OpenGL Texture Object in GPU VRAM]
       │
       ▼ (UV Coordinates select frame rectangle)
[Normalized UV Coordinate Bounds (u0, v0, u1, v1)]
       │
       ▼ (Vertex Shader multiplies 1x1 Quad by Model Matrix)
[Transformed World Quad (Position, Rotation, Scale)]
       │
       ▼ (Fragment Shader discards transparent alpha pixels)
[Pixel-Perfect Character Rendered on Screen!]
```

---

### Step 1: What is a Sprite in OpenGL?
In Modern OpenGL, there is no built-in `drawSprite()` function. A sprite is simply a **flat 2D rectangle (a Quad) formed by 2 triangles (6 vertices)** with an image (a **Texture**) mapped onto its surface.

```text
(-0.5, +0.5) ┌───────────────┐ (+0.5, +0.5)
             │ ╲           2 │
             │   ╲           │
             │     ╲         │
             │       ╲       │
             │         ╲     │
             │ 1         ╲   │
(-0.5, -0.5) └───────────────┘ (+0.5, -0.5)
```

The engine keeps **one single unit quad** stored in GPU VRAM inside a Vertex Buffer Object (VBO). Whenever any character or tile is drawn, the engine sends the quad's world position, size, and rotation angle to the shader via a **Model Matrix ($M$)**.

---

### Step 2: The UV Coordinate System
How does OpenGL know which part of an image belongs to the square? It uses **UV Texture Coordinates**:
- $U$ is the horizontal axis (from $0.0$ on the left to $1.0$ on the right).
- $V$ is the vertical axis (from $0.0$ at the bottom to $1.0$ at the top).

Regardless of whether an image is $64\times 64$ pixels or $1600\times 200$ pixels, OpenGL **always normalizes coordinates between $0.0$ and $1.0$**.

```text
(0.0, 1.0) ┌─────────────────────────┐ (1.0, 1.0)
           │                         │
           │       IMAGE TEXTURE     │
           │                         │
(0.0, 0.0) └─────────────────────────┘ (1.0, 0.0)
```

---

### Step 3: Spritesheets & Frame Slicing
Instead of loading dozens of separate image files for every animation step, professional games pack all frames into a single image called a **Spritesheet**.

For example, Samurai Mack's Defend animation [`assets/samurai/Defend_nobg.png`](/assets/samurai/Defend_nobg.png) is $1600\text{ pixels wide}$ and $200\text{ pixels high}$, containing **8 frames** of $200\times 200\text{px}$ each:

```text
Frame:   0       1       2       3       4       5       6       7
      ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
      │   🛡️  │   🛡️  │   🛡️  │   🛡️  │   🛡️  │   🛡️  │   🛡️  │   🛡️  │
      └───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
U:   0/8     1/8     2/8     3/8     4/8     5/8     6/8     7/8    8/8 (1.0)
```

To display frame $i$ (where $i \in [0, 7]$):
$$u_0 = \frac{i}{8.0}, \quad u_1 = \frac{i + 1}{8.0}$$
$$v_0 = 0.0, \quad v_1 = 1.0$$

The engine passes `uUVRect = (u0, v0, u1 - u0, v1 - v0)` to the vertex shader. The vertex shader maps only that specific $200\times 200$ frame onto the quad!

---

### Step 4: Horizontal Mirroring (Sprite Flipping)
When the Samurai runs to the right, he faces right. When he runs to the left, we don't need a second spritesheet! We simply **invert the horizontal UV coordinates**:
- Facing Right: $U_{\text{left}} = u_0, \quad U_{\text{right}} = u_1$
- Facing Left: $U_{\text{left}} = u_1, \quad U_{\text{right}} = u_0$

By swapping $u_0$ and $u_1$, the graphics card automatically flips the sprite horizontally with zero CPU computation!

---

### Step 5: Transparency & The Fragment Discard
PNG image files store 4 color channels per pixel: **Red, Green, Blue, and Alpha (RGBA)**.
The Alpha channel ($A \in [0.0, 1.0]$) represents transparency ($0.0 = \text{completely invisible}$, $1.0 = \text{completely solid}$).

If you draw a quad without transparency handling, the transparent pixels will appear as a solid black or white box around the character.

In our GLSL Fragment Shader, we use the `discard` keyword:
```glsl
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    
    // Discard transparent background pixels
    if (texColor.a < 0.1) {
        discard;
    }
    
    FragColor = texColor * uColor;
}
```
When `discard` is triggered, the graphics pipeline completely stops processing that pixel. It does not overwrite the background or write to depth buffers. This creates clean, sharp pixel-art outlines!

---

### Step 6: World Scaling and Aspect Ratio Preservation
If you render a $200\times 200$ sprite onto an unscaled quad on a $1280\times 720$ widescreen monitor, it will look stretched.

The engine preserves true dimensions by multiplying the unit quad by the **Model Matrix ($M$)**:
$$M = \text{Translate}(x, y, 0) \times \text{Rotate}(\theta) \times \text{Scale}(w, h, 1)$$

Combined with an **Orthographic Projection Matrix** (`glm::ortho(-8.0, 8.0, -4.5, 4.5)`), 1 unit in OpenGL equals exactly 1 meter in the game world, ensuring characters and tiles look crisp regardless of monitor resolution.

---

## 4. Combat & Defense Mechanics: How Attack and Block Work ⚔️🛡️

### 1. Melee Katana Attack (`LMB` / `J` / `X`)
- **Hitbox Arc Sweep**: When the player presses attack, the game activates the 6-frame attack animation (`Attack1.png`).
- Rather than dealing damage on every single frame, damage is registered strictly during the active strike window ($t \in [0.10\text{s}, 0.32\text{s}]$).
- An oriented attack bounding box ($2.4\text{m} \times 2.0\text{m}$ reaching $1.35\text{m}$ forward) tests for intersection with enemies:
  - **Bringer of Death**: Slain in **1 hit** (+250 pts, +5 coins).
  - **Initial Slime**: Slain in **2 hits**, triggering a dramatic 3-phase transformation into the giant Demon Slime Boss!
  - **Boss Demon Slime**: Slain in **5 hits** (+1000 pts, +20 coins).

### 2. Katana Defend / Block Stance (`RMB` / `K`)
- **1-Second Guard Window**: Pressing Right Click puts the samurai into an 8-frame defensive guard (`Defend_nobg.png`) for up to $1.0\text{s}$, anchoring horizontal movement.
- **Complete Damage Negation**: While guarding, all enemy attacks (Slime body bumps, Bringer of Death scythe cleaves, telegraphed thundercloud strikes, and Demon Boss charges/smashes) deal **zero damage** to player lives.
- **Parry Clang & Cooldown**: Deflecting an attack plays the metallic `SFX_BLOCK` clang, spawns `"[PARRY]"` floating text, gives $0.6\text{s}$ grace invulnerability, and starts a **2.0-second cooldown** (`blockCooldown = 2.0f`).

---

## 5. Enemy AI & Boss Mechanics 👹

1. **Bringer of Death (Wraith)**:
   - Roams platform levels. When the player approaches within $2.4\text{m}$, it locks its facing direction and enters a telegraphed melee attack ($0.75\text{s}$ windup with `"[ ! ]"` warning).
   - **Ranged Multi-Elevation Thunder Spell**: If the player is at a distance ($3.0\text{m}$ to $14.0\text{m}$) across vertical elevations (e.g. underground to surface), it stops and casts a dark lightning spell:
     - **Phase 1: Cloud Gathering ($t \in [0.0\text{s}, 0.58\text{s}]$, frames 0–6)**: Dark swirling storm cloud forms overhead with an on-screen warning (`"THUNDER CLOUD GATHERING! MOVE!"`). Zero damage is dealt, giving the player ample time to sprint away, jump, dash, or raise katana defense!
     - **Phase 2: Mid-Animation Thunder Strike ($t \in [0.58\text{s}, 0.85\text{s}]$, frames 7–10)**: The lightning bolt slams down from the cloud into the ground with explosive acoustic impact. Damage is evaluated strictly during this mid-animation touchdown window.
     - **Phase 3: Dissipation ($t \in [0.85\text{s}, 1.35\text{s}]$, frames 11–15)**: Residual sparks and cloud smoke fade out harmlessly.
2. **Initial Slime & 3-Phase Transformation**:
   - Small green slime hopping across platforms. Defeating it in 2 hits triggers a $5.5\text{s}$ metamorphosis sequence with sequenced audio (wet splat $\rightarrow$ bubbling acid $\rightarrow$ demon roar) before the giant Demon Slime emerges.
3. **Boss Demon Slime**:
   - Giant $8.5\text{m}$ boss entity with 5 HP and an overhead health bar.
   - **Platform Jumping**: Leaps across platform tiers and gap chasms using an 18-frame sequence (`demon_smash.png`), slamming down with ground shockwave damage.
   - **Boss Evasion**: Players can choose to fight or flee! Running $>28\text{m}$ away or dashing through the boss ($>8\text{m}$) allows the player to bypass the fight safely.

---

## 6. Questions You Might Be Asked in a Lab Viva / Evaluation 🎯

### Q1: How do 2D sprites work in Modern OpenGL if OpenGL is a 3D API?
> **Answer**:  
> In Modern OpenGL, a sprite is a flat 2D rectangle (a Quad) made of 2 triangles stored in a VBO. We apply an image (Texture) to this quad using normalized UV coordinates ($0.0$ to $1.0$). To animate, we slice frames from a horizontal spritesheet by calculating the sub-rectangle for the current frame index. The fragment shader discards transparent pixels (`if (alpha < 0.1) discard;`), rendering clean pixel art.

### Q2: What is the benefit of the "Single Reusable Quad" architecture?
> **Answer**:  
> Instead of creating, allocating, and deleting new vertex buffers for every single tile, coin, enemy, and bullet, the engine keeps **one single $1 \times 1$ unit quad** permanently in GPU VRAM. The vertex shader scales, translates, and rotates this single quad using Model-View-Projection ($MVP$) matrices. This reduces memory usage and draw call overhead, maintaining 60+ FPS.

### Q3: Why does the game use both Procedural Audio and Windows MCI Streaming?
> **Answer**:  
> We use a **Hybrid Audio Architecture**:
> 1. **Procedural Synthesis (WinMM)** generates instantaneous arcade sound effects (sword slashes, parry clangs, jumps, coin chimes) directly in RAM via math formulas, guaranteeing zero missing asset crashes.
> 2. **Windows MCI Streaming** handles long-form background music (`Quarter_in_the_Slot.mp3`) with real-time volume/mute controls and complex creature vocalizations without consuming massive amounts of RAM.

### Q4: How does the game ensure the player can dodge telegraphed boss and spell attacks?
> **Answer**:  
> Through **Telegraphed Wind-Up Windows**:
> - The Demon Slime has a $1.125\text{s}$ wind-up animation with an overhead alert (`"[ ! ] DODGE!"`) before damage is dealt.
> - The Bringer of Death has a $0.75\text{s}$ melee wind-up and a $1.0\text{s}$ spell gathering phase.
> - Crucially, enemy facing direction is **locked** during the telegraph. If the player jumps or dashes behind the enemy during the windup, the attack misses completely.

### Q5: How is the Katana Defend / Block mechanic implemented?
> **Answer**:  
> Right Click (or `K`) triggers `isBlocking = true` for up to $1.0\text{s}$ using `Defend_nobg.png`. In the collision detection loops, if `isBlocking` is true when an enemy attack connects, player damage is bypassed, a metallic parry SFX (`SFX_BLOCK`) plays, floating `"[PARRY]"` text appears, and a $2.0\text{s}$ cooldown is applied to prevent parry spamming.

### Q6: How does the game manage endless world generation without running out of RAM?
> **Answer**:  
> Through **Procedural Generation and Dynamic Chunk Pruning**. The engine generates level chunks $35\,\text{m}$ ahead in both directions ($+X$ and $-X$). As the player moves, any chunks farther than $85\,\text{m}$ behind are pruned from memory arrays. This bounds active memory to a constant footprint.
