# The Wenderer: Presentation Guide & Video Narration Script 🎬🎤

> **Course**: Computer Graphics & Multimedia Lab (`CSE0611328`)  
> **Institution**: Department of Computer Science & Engineering, UITS (`Batch 55` · `Autumn 2026`)  
> **Faculty**: Engineer Robiul Islam (Assistant Professor) & Tasneen Tanvir (Lecturer)  
> **Group Members**: Gaus Saraf Murady (`0432410005101088`), Lima Akter (`0432410005101063`), Fahima Akter (`0432410005101093`)  
> **Reference Docs**: [`/doc/explain.md`](/doc/explain.md) · [`/doc/documentation.md`](/doc/documentation.md) · [`/doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md) · [`/README.md`](/README.md)

---

## 1. Pre-Gameplay Video Narration Script & Visual Storyboard (Screen Recording)

Use this **~75-second spoken script** at the beginning of your screen recording *before* transitioning into uninterrupted gameplay. Each block tells you **what to say** and **which short gameplay clips/visuals to show** on screen while reading it.

> [!TIP]
> **Recording Tip**: Record the gameplay clips first (1–2 minutes of raw footage), then record your voiceover cleanly while playing back a fast-paced **B-roll montage** of those clips, and finally cut straight into your full continuous gameplay run. Keep background music (`[M]` or Pause Menu volume slider at `20%–30%`) lower than your microphone voice.

---

### Intro & Visual Storyboard (Read Before Full Gameplay)

| Segment & Timing | Spoken Script (Read Out Loud) | Suggested Gameplay Visuals to Show on Screen |
|---|---|---|
| **1. Hook & Game Type / Genre** *(0:00 – 0:15)* | *"Hello everyone. Today we are presenting **The Wenderer**—a 2D side-view, multi-directional endless runner and combat platformer built from scratch in **C++17** and **Modern OpenGL 3.3** for our Computer Graphics & Multimedia Lab project."* | • **0:00–0:05**: Launch screen showing the procedural **Academic Proposal Splash Card** (`CSE0611328`, faculty & group table) fading into the **Media Foundation intro video**.<br>• **0:05–0:15**: Cut to **Samurai Mack** standing idle and then sprinting across grassy surface platforms; briefly tap **`B`** to flash through the 4 biomes (*Silvergrass → Autumn → Green Forest → Urban*) and **`N`** to show the Day/Night shift. |
| **2. Core Philosophy ("Why We Made It")** *(0:15 – 0:33)* | *"Most endless runners—like Subway Surfers or Temple Run—lock the player onto a single, predictable forward track. We wanted to break that static formula. The word **'Wend'** means to change course or travel along an altered path. Unlike a passive wanderer, **The Wenderer** is about adaptability: the world forces you to continuously react as your direction, elevation, and threats shift around you."* | • **0:15–0:24**: Quick split/montage cut showing horizontal sprint (`D` / `A`), dropping down a steep slope into the **Underground Cavern**, and climbing back up a vertical **ladder shaft**.<br>• **0:24–0:33**: Show **`F`** toggling **Auto-Runner** mode + **`Shift`** ghost-trail **Dash** over a cluster of metallic spikes. |
| **3. Traversal & Multi-Dimension Trials** *(0:33 – 0:50)* | *"Gameplay spans four vertical layers—from a high Sky Canopy down to subterranean caverns protected by a continuous bedrock floor, so you never die to unfair pit falls. Whenever you touch a glowing **Direction Relic**—or when autonomous shifts trigger—you are pulled into dedicated vertical mini-games: a 3-track **Ladder Ascent** dodging sawblades, or a high-speed **Sky-Fall Dive** dodging raptor birds and floating mines until the **Open Exit Gate** appears."* | • **0:33–0:40**: Slide down a steep ramp into a cavern, collect floating gold **Coins** (`+50` popup), and touch a **Green Up Arrow Relic**.<br>• **0:40–0:45**: 3-second clip inside **`LadderScene`** jumping between the 3 tracks around spinning sawblades with the `SURVIVE UNTIL EXIT` countdown.<br>• **0:45–0:50**: 3-second clip inside **`SkyFallScene`** dodging a raptor bird, grabbing a **Heraldic Shield** (glowing circular energy bubble), and entering the animated **Open Exit Gate** to return to the platformer. |
| **4. Combat, Defense & Escalating Bosses** *(0:50 – 1:10)* | *"To survive, you wield a katana for melee slashes and a timed **Defend Stance** that parries incoming strikes with zero damage. Meanwhile, enemy intensity escalates every two minutes across three phases—from roaming **Bringers of Death** that cast telegraphed multi-layer thunder spells, to **Slimes** that transform into a giant leaping **Demon Slime Boss**, all the way to full **Horde Waves**."* | • **0:50–0:56**: Samurai Mack slashing a **Bringer of Death** (`LMB`/`J`), then raising the **Katana Guard** (`RMB`/`K`) to block a scythe swing or thunder strike with the **`[PARRY]`** floating text.<br>• **0:56–1:04**: Defeating an Initial Slime (2 hits) → quick cut of the **5.5-second Boss Metamorphosis** → fighting the giant **Boss Demon Slime** with its overhead health bar.<br>• **1:04–1:10**: Show the **Thunder Cloud Gathering** telegraph warning (`[ ! ]`) and a quick peek at the **`ESC` Pause Menu** (30% BGM slider) before cutting to full gameplay. |
| **5. Transition to Gameplay** *(1:10 – 1:15)* | *"With our custom OpenGL rendering pipeline, procedural audio synthesis, and endless chunk streaming in place, let's jump straight into a live gameplay run of **The Wenderer**."* | • Smooth fade or wipe into the **uninterrupted live gameplay recording** starting from the Tutorial / Spawn point. |

---

### Quick Shot-List Checklist (How to Capture Each Clip Easily)

Use these exact keys while recording your raw B-roll clips so you can trigger every visual cleanly in seconds:

1. **Splash & Video Intro**: Run `./build/main.exe` and let the splash fade naturally (`5.5s`), then let a few seconds of `introVid.mp4` play before pressing `Space` to skip into the Tutorial Modal.
2. **Biomes & Day/Night**: On the starting platform, move slightly with `D`, press `B` 3 times (cycles *Silvergrass → Autumn → Green Forest → Urban*), and press `N` once (toggles Day/Night).
3. **Slopes, Ladders & Dash**: Run right or left to find a downward dive slope into the Underground Cavern (`Y = -4.5m`), hold `Left Shift` to show ghost afterimages over grouped spikes, and press `W`/`Space` at a ladder shaft to climb back up.
4. **Combat & Parry**: Attack the guaranteed starter Slime at `X = +17.5m` or `-18.0m` (`LMB`/`J`/`X`), or wait for its attack / a Bringer of Death wind-up and hold `RMB`/`K` to showcase the `1.0s` Defend stance and `[PARRY]` popup.
5. **Boss Transformation**: Finish the Initial Slime in 2 hits to record the 3-phase metamorphosis into the `8.5m` Boss Demon Slime and its platform-smash leaps.
6. **Mini-Game Trials**: Touch a **Green Up Arrow** relic for `LadderScene` or a **Gold Down Arrow** relic for `SkyFallScene`; survive `15s` until `=== EXIT APPEARED! ===` spawns the glowing **Open Exit Gate** (`assets/exit_gate.png`).
7. **Shield & Hearts**: Pick up `assets/shield_pickup.png` to display the shader-rendered **circular energy shield bubble**, and `assets/heart_pickup.png` to show 3-life healing / `+150` overheal.

---

## 2. Live Project Presentation & Demo Playbook (5–7 Minutes)

Follow this step-by-step sequence when demonstrating **The Wenderer** live to your instructors or evaluators.

### Step 0: Pre-Demo Preparation
1. **Build cleanly beforehand** using `make` (or `mingw32-make`) so `./build/main.exe` is ready to launch immediately.
2. **Verify audio & windowing**: Make sure system speakers are audible (the BGM defaults to **40%** so it won't overpower your voice). Know that pressing **`F11`** toggles letterboxed fullscreen with zero aspect distortion.
3. **Keep key files open** in your editor/IDE for the code walkthrough: [`/include/Common.h`](/include/Common.h), [`/src/Renderer.cpp`](/src/Renderer.cpp), [`/src/World.cpp`](/src/World.cpp), [`/src/Audio.cpp`](/src/Audio.cpp), and [`/src/UIManager.cpp`](/src/UIManager.cpp).

---

### Step 1: Opening & Academic Context *(~45 sec)*
- **Action**: Launch `./build/main.exe` (do **not** skip immediately—let the evaluators see the procedural academic splash screen).
- **What to point out**:
  - *"Notice our intro splash screen isn't a static PNG—it is rendered 100% procedurally using Modern OpenGL vector quads and an embedded 8×8 bitmap font (`font8x8_basic.h`), fading in and out via alpha interpolation before launching Media Foundation hardware video decoding (`introVid.mp4`)."*
- **Action**: Press `Space` to advance to the Tutorial Modal, briefly highlight the lore (*"The Essence of Wandering"*), and press `Space` to start.

---

### Step 2: Core Philosophy & Movement Mechanics *(~1 min)*
- **Action**: Stay idle for 1 second, then run with `A`/`D`, jump with `Space`, dash with `Left Shift`, and toggle Auto-Runner briefly with `F`.
- **What to point out**:
  - **Genre & Philosophy**: *"Unlike fixed-axis runners, The Wenderer challenges spatial adaptability across four vertical tiers—Sky Canopy, Surface Platforms, Surface Ground, and Underground Caverns—with a continuous Bedrock floor so you never die to cheap pit falls."*
  - **Visual Polish**: Tap **`B`** to show the 4 scenic backgrounds and **`N`** to demonstrate the Day/Night lighting cycle.

---

### Step 3: Multi-Directional Shifts & Mini-Game Trials *(~1.5 min)*
- **Action**: Locate a **Green Up Relic** (`LadderScene`) or **Gold Down Relic** (`SkyFallScene`) on an elevated platform—or toggle **`Y`** if demonstrating timed auto-shifts.
- **What to point out**:
  - Inside **`LadderScene`**: *"We switch to a 3-track vertical climbing state machine with spinning sawblades, rung physics, and a 15-second survival timer before the animated Open Exit Gate spawns."*
  - Inside **`SkyFallScene`**: *"Downward shifts drop into a high-speed aerial dive dodging raptor birds and mines; picking up a Shield renders a circular energy bubble using a fragment shader distance check ($r = \sqrt{x^2+y^2} > 0.5$ discard) without extra geometry."*

---

### Step 4: Combat, Katana Parry & Escalating Boss AI *(~1.5 min)*
- **Action**: Engage a starter Slime (`X = ±18m`) or a Bringer of Death.
- **What to point out**:
  - **Active Hitbox Window**: *"Katana slashes (`LMB`/`J`) only deal damage during frames 1–3 ($0.10\text{s}$–$0.32\text{s}$), matching the sword arc."*
  - **Defend / Parry (`RMB`/`K`)**: *"Right-click enters a 1-second defensive stance (`Defend_nobg.png`) that negates all enemy and thunder damage, plays a procedurally synthesized metallic parry clang, and triggers a 2-second cooldown."*
  - **Telegraphed Enemy AI**: *"Bringers of Death cast multi-elevation thunder spells with a 0.58s visual cloud-gathering warning before the bolt lands. Slaying an Initial Slime triggers a 5.5s 3-phase transformation into the 5-HP Demon Slime Boss, while spawn rates escalate across 3 time-scaled phases (1-minute Phase 1 & Phase 2, up to 5 per type/chunk + Horde Spawns in Phase 3)."*

---

### Step 5: Pause Menu, Architecture & Technical Wrap-Up *(~1 min)*
- **Action**: Press **`Esc`** to open the Pause Menu. Demonstrate the **10%-step BGM Volume Slider** (drag or click `[-]`/`[+]`), press **`T`** to show the wrapped Tutorial modal, and press **`F11`** to show letterboxed fullscreen (`glScissor`).
- **What to point out**: Briefly show the modular architecture diagram below and highlight how the project stays at 60+ FPS with constant memory.

---

## 3. Technical Architecture & Graphics Talking Points (For Slides / Board)

If you are using slides or explaining the code structure right after the demo, focus on these **4 core engineering pillars** from [`/doc/explain.md`](/doc/explain.md) and [`/doc/documentation.md`](/doc/documentation.md):

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                                  main.cpp                                   │
│           Master Game Loop, Player State Machine, Input & Camera            │
└──────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬────┘
       │          │          │          │          │          │          │
       ▼          ▼          ▼          ▼          ▼          ▼          ▼
   Common.h   Renderer    World      Audio     UIManager LadderScene SkyFallScene
  (Constants  (OpenGL    (Chunks &  (WinMM +   (HUD,      (3-Track   (Aerial Dive
   & Structs)  Shaders)   Spawns)    MCI)       Splash)    Climb)     Mini-Game)
```
*(Plus [`VideoPlayer`](/src/VideoPlayer.cpp) using Microsoft Media Foundation to decode `introVid.mp4` into an OpenGL texture.)*

1. **Single Reusable Unit Quad + MVP Matrices ([`/src/Renderer.cpp`](/src/Renderer.cpp))**:
   - Instead of allocating VBOs for every tile, coin, or enemy, **one $1 \times 1$ unit square** (6 vertices / 2 triangles) lives permanently in GPU VRAM.
   - Every sprite is drawn by transforming that quad via $\text{MVP} = P \times V \times M$ (`glm::ortho` 1 unit = 1 meter), slicing spritesheet frames via `uUVRect = (u, v, du, dv)`, flipping horizontally by swapping $u_0 \leftrightarrow u_1$, and discarding transparent alpha pixels (`if (texColor.a < 0.1) discard;`).
2. **Circular Energy Shield via Fragment Shader Math**:
   - Computes $r = \sqrt{(x - 0.5)^2 + (y - 0.5)^2}$ in the fragment shader and discards pixels with $r > 0.5$ to draw a crisp circular shield bubble on a standard quad.
3. **Endless World Streaming with Constant Memory ([`/src/World.cpp`](/src/World.cpp))**:
   - Generates procedural chunks **$35\,\text{m}$ ahead** in both $+X$ and $-X$ directions, and automatically prunes chunks farther than **$85\,\text{m}$ behind** the player—preventing RAM growth over long runs.
4. **Hybrid Audio Engine ([`/src/Audio.cpp`](/src/Audio.cpp))**:
   - **Procedural PCM Synthesis (`WinMM` `PlaySoundA`)**: Generates 16-bit mono $22{,}050\,\text{Hz}$ waveforms (sword slashes, parry clangs, jumps, coins, footsteps) directly in RAM via math formulas—zero missing-SFX errors.
   - **Hardware Streaming (`Windows MCI`)**: Streams looping BGM (`Quarter_in_the_Slot.mp3` at 40% default, 10%-step slider), thunder crackles, and the Boss Demon Slime's 3-phase transformation and dying roar.

---

## 4. Controls Cheat Sheet (Keep Open During Demo)

| Input | Action | Notes |
|---|---|---|
| **`A` / `D`** *(or `←` / `→`)* | Walk / Run | Free Exploration is default; stands idle when released |
| **`W` / `Space` / `↑`** | Jump / Climb Up | Jump traps & gaps, or ascend ladder shafts |
| **`S` / `↓`** | Drop / Descend / Slide | Drop through 1-way platforms or enter underground ladders |
| **`Left Shift`** | Dash | High-speed burst with ghost trails & spike invulnerability |
| **`LMB` / `J` / `X`** | Katana Slash | Bringer of Death (1 hit), Slime (2 hits), Demon Boss (5 hits) |
| **`RMB` / `K`** | Katana Defend / Parry | `1.0s` guard negating all damage; `2.0s` cooldown on parry |
| **`F`** | Toggle Auto-Runner | Switch between Free Exploration and continuous sprinting |
| **`Y`** | Toggle Auto-Shift | Switch `6.5s` timed shifts vs. Direction Relics only |
| **`B`** / **`N`** | Scenery / Day-Night | Cycle 4 backgrounds (`B`) or toggle Day/Night (`N`) |
| **`M`** / **`-` `=`** | Music Mute / Volume | Mute (`M`) or step BGM volume in `10%` increments (default `40%`) |
| **`Esc`** / **`T`** / **`F11`** | Pause / Tutorial / Fullscreen | Pause menu (with volume slider), How-To-Play modal, or letterboxed fullscreen |
| **`R`** / **`Q`** | Restart / Quit | Quick retry (`R`) or quit from Pause / Game Over (`Q`) |

---

## 5. Rapid Viva & Q&A Cheat Sheet

For full detailed answers and code snippets, refer to [`/doc/explain.md#6-questions-you-might-be-asked-in-a-lab-viva--evaluation-`](/doc/explain.md). Here are quick 2-sentence answers for live Q&A:

- **Q: How do you draw animated 2D sprites in Modern OpenGL (a 3D API)?**  
  We store one $1\times 1$ quad (2 triangles) in a VBO and transform it with an orthographic MVP matrix. We animate by passing normalized `uUVRect` bounds to slice frames from a horizontal spritesheet, and use `discard` in the fragment shader when `alpha < 0.1` for crisp transparency.
- **Q: How do you prevent infinite worlds from running out of RAM?**  
  [`World.cpp`](/src/World.cpp) streams chunks $35\,\text{m}$ ahead of the player in both horizontal directions and prunes chunks farther than $85\,\text{m}$ behind, keeping active memory flat.
- **Q: Why combine procedural audio and MCI streaming?**  
  Procedural `WinMM` synthesis computes short arcade SFX (slashes, parries, jumps) mathematically in RAM so they never fail to load, while Windows `MCI` streams long MP3 music and multi-second monster vocalizations efficiently without inflating RAM.
- **Q: How do you keep telegraphed enemy/boss attacks fair?**  
  Both the Bringer of Death's thunder spell ($0.58\text{s}$ cloud-gathering warning) and Demon Slime's melee/smash wind-ups lock enemy facing direction during the telegraph and restrict damage to mid-animation impact frames, letting players dash behind, jump out, or parry with `RMB`/`K`.
- **Q: How is the Academic Splash Screen rendered without an image file?**  
  [`UIManager::renderSplashScreen`](/src/UIManager.cpp) draws the card, borders, and table grid lines using scaled OpenGL quads and renders text from an embedded $8\times 8$ bitmap font ([`/include/font8x8_basic.h`](/include/font8x8_basic.h)) with linear alpha fade interpolation.

