# Session 7 Summary: Academic Intro Splash Screen & OpenGL Rendering

- **Session Number**: 7
- **Legacy Versions**: `v7.0 - v7.2`
- **Session Folder**: [`/doc/prompts/7. Academic Intro Splash Screen & OpenGL Rendering/`](/doc/prompts/7.%20Academic%20Intro%20Splash%20Screen%20&%20OpenGL%20Rendering/)
- **Date(s)**: 2026-11-02
- **Conversation Log**: [`conversation.md`](/doc/prompts/7.%20Academic%20Intro%20Splash%20Screen%20&%20OpenGL%20Rendering/conversation.md)
- **Transcript Status**: Migrated from legacy Markdown archives (raw `transcript_full.jsonl` unavailable).

---

## 1. User Goals
- **v7.0** (Academic Intro Splash Screen with Course and Group Details and Fade Transition): Implementation of an academic introductory splash screen displaying course details, faculty information, and student group details prior to the intro video, completed with a smooth fade in-out transition, bilinear filtering, and procedural fallback UI.
- **v7.1** (Fix OpenGL Header Order Conflict in MSYS2 and Restore Splash Screen Build): Resolved GCC compilation failure in MSYS2 environment where GLAD threw `#error OpenGL header already included`, preventing the new binary with the intro splash screen from compiling when executing `make win`.
- **v7.2** (Render Academic Splash Screen Graphically in OpenGL and Purge Bitmap Texture): Conversion of the academic introductory splash screen from loading a static PNG texture from disk to a 100% procedural vector/text presentation rendered directly in Modern OpenGL.

---

## 2. Engineering & Implementation Details
### v7.0 - Academic Intro Splash Screen with Course and Group Details and Fade Transition
> **1. Requirement Decomposition & Architectural Sequencing**:
> - The game engine previously launched immediately into `STATE_INTRO_VIDEO`, rendering an MP4 video stream using Windows Media Foundation (`IMFSourceReader`) with asynchronous sound playback (`PlaySoundA`).
> - The requirement dictates inserting an academic introductory splash screen displaying official project submission metadata (Course: *Computer Graphics & Multimedia Lab - CSE0611328*, Faculty: *Engineer Robiul Islam - Assistant Professor* & *Tasneen Tanvir - Lecturer*, Group Members: *Gaus Saraf Murady 0432410005101088*, *Lima Akter 0432410005101063*, *Fahima Akter 0432410005101093*, *Semester Autumn 2026, Batch 55, Dept CSE*) before the video begins.
> - An audio lifecycle race condition was diagnosed: `introVideoPlayer.open()` immediately starts decoding audio and calls `PlaySoundA`. If the video player was initialized at program start, video audio would overlap the splash screen. To guarantee audio-visual isolation, video initialization is decoupled and deferred to `startIntroVideo()`, invoked only when the splash sequence concludes.
>
> **2. State Machine Pipeline**:
> - Updated `enum GameState` in `[/include/Common.h](/include/Common.h)` to place `STATE_INTRO_SPLASH = 0` as the initial game state, followed sequentially by `STATE_INTRO_VIDEO = 1`, `STATE_RUNNING = 2`, `STATE_PAUSED = 3`, and `STATE_GAME_OVER = 4`.
> - Initialized the engine in `STATE_INTRO_SPLASH`, configuring dedicated timers: total duration $T_{\text{total}} = 5.5\,\text{s}$, fade-in duration $T_{\text{in}} = 1.0\,\text{s}$, hold duration $T_{\text{hold}} = 3.5\,\text{s}$, and fade-out duration $T_{\text{out}} = 1.0\,\text{s}$.
>
> **3. Mathematical Model for Fade In-Out Alpha Blending**:
> - The opacity multiplier $\alpha(t) \in [0.0, 1.0]$ is computed using piecewise linear interpolation:
>   $$\alpha(t) = \begin{cases} \frac{t}{T_{\text{in}}} & 0 \le t < T_{\text{in}} \\ 1.0 & T_{\text{in}} \le t \le T_{\text{total}} - T_{\text{out}} \\ \frac{T_{\text{total}} - t}{T_{\text{out}}} & T_{\text{total}} - T_{\text{out}} < t \le T_{\text{total}} \end{cases}$$
> - In OpenGL fragment execution, the texture color is modulated by uniform color $u_{\text{color}} = (1, 1, 1, \alpha)$:
>   $$\text{FragColor} = \text{Texture}(\text{TexCoord}) \times u_{\text{color}}$$
> - With `glEnable(GL_BLEND)` and `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`, this yields a gradual fade from pure cinematic black, pristine presentation at full opacity, and a gentle dissolve back to black before video initialization.
>
> **4. Dual Filtering Pipeline (`GL_LINEAR` vs `GL_NEAREST`)**:
> - Sprite sheets (e.g., pixel-art samurai, slimes, and tilesets) require nearest-neighbor sampling (`GL_NEAREST`) to prevent pixel blurring.
> - High-density text and document graphics (such as `assets/splash.png` at $762 \times 769\,\text{px}$) become heavily aliased and jagged under nearest-neighbor filtering.
> - Extended `Renderer::loadTexture(const char* filepath, bool linear = false)` in `[/include/Renderer.h](/include/Renderer.h)` and `[/src/Renderer.cpp](/src/Renderer.cpp)` to parameterize the minification and magnification filters (`GL_TEXTURE_MIN_FILTER` / `GL_TEXTURE_MAG_FILTER`). When `linear = true`, OpenGL enables hardware bilinear interpolation, producing crisp, smooth document typography.
>
> **5. Visual Staging & Procedural Fallback UI**:
> - Implemented `UIManager::renderSplashScreen` in `[/src/UIManager.cpp](/src/UIManager.cpp)`:
>   - **Cinematic Studio Backdrop**: Fullscreen quad in deep charcoal slate (`glm::vec4(0.03f, 0.04f, 0.06f, 1.0f)`).
>   - **Document Card Framing**: Centered document quad scaled to preserve the $762:769$ aspect ratio, layered over a soft drop shadow (`glm::vec4(0, 0, 0, 0.65f * alpha)`) and a fine cyan accent border (`glm::vec4(0.25f, 0.70f, 0.95f, 0.85f * alpha)`).
>   - **Procedural Fallback**: If `assets/splash.png` is not found, the renderer automatically builds an in-engine vector presentation card using `gameRenderer.drawText` and geometric bounding quads with exact academic metadata.
>   - **Progress & Interactive Skip**: Displays a bottom progress bar and pulsing skip indicator `[SPACE / ESC / ENTER] SKIP`.
>
> **6. Edge-Triggered Key Debouncing & VRAM Management**:
> - Skipped transitions trigger `startIntroVideo()`, which deletes the OpenGL splash texture from VRAM (`glDeleteTextures`) to eliminate memory overhead.
> - Integrated edge-triggered key debounce flags (`splashSkipLast` and `videoSkipLast`), guaranteeing that a key held down to skip the splash screen does not bleed through to skip the intro video or game tutorial modal.

### v7.1 - Fix OpenGL Header Order Conflict in MSYS2 and Restore Splash Screen Build
> **1. Root Cause Analysis**:
> - The terminal error reveals why the splash screen did not appear: `make win` failed at the compilation stage (`Error 1`), meaning `build/main.exe` was never built or replaced with the new splash screen executable.
> - In `include/Common.h`:
>   ```cpp
>   #include <GLFW/glfw3.h>
>   #include <glad/glad.h>
>   ```
> - Under MSYS2 GCC, `<GLFW/glfw3.h>` automatically includes `<GL/gl.h>` by default if `GLFW_INCLUDE_NONE` is not defined. `<GL/gl.h>` defines the macro `__gl_h_`.
> - Immediately afterwards, when `include/glad/glad.h` is parsed, lines 4-6 evaluate:
>   ```c
>   #ifdef __gl_h_
>   #error OpenGL header already included, remove this include, glad already provides it
>   #endif
>   ```
>   GLAD intentionally triggers `#error` because Modern OpenGL function pointers must be declared by GLAD rather than static legacy OpenGL 1.1 symbols.
>
> **2. Header Architecture Standardization**:
> - Reordered includes in `[/include/Common.h](/include/Common.h)`:
>   ```cpp
>   #include <glad/glad.h>
>   #ifndef GLFW_INCLUDE_NONE
>   #define GLFW_INCLUDE_NONE
>   #endif
>   #include <GLFW/glfw3.h>
>   ```
> - By including `<glad/glad.h>` before GLFW and defining `GLFW_INCLUDE_NONE`, GLFW is strictly instructed not to include any system OpenGL headers.
>
> **3. Compilation & Build Verification**:
> - Tested compilation with both `make build` and `mingw32-make build`.
> - Both completed with exit code 0 and zero warnings.
> - Synced the resulting binary to `TheWenderer/TheWenderer.exe`.

### v7.2 - Render Academic Splash Screen Graphically in OpenGL and Purge Bitmap Texture
> **1. Requirement Analysis & Architectural Shift**:
> - The initial implementation loaded `assets/splash.png` into GPU memory via `stbi_load` and textured a single centered quad.
> - The user specified making the splash intro load graphically rather than loading a static raster image file from disk.
> - A fully procedural/graphical approach offers distinct advantages:
>   - Zero image file I/O on game boot, eliminating disk latency and missing file risks.
>   - Infinite scaling sharpness across any display resolution (1080p, 1440p, 4K) without bitmap blurriness or texture downsampling artifacts.
>   - Clean vector lines, stylized table grids, and color-coded typography perfectly unified with the game's dark sci-fi UI styling.
>
> **2. Procedural Geometry & Vector Pipeline**:
> - Re-engineered `UIManager::renderSplashScreen(halfW, halfH, fadeAlpha, progress)` in `[/src/UIManager.cpp](/src/UIManager.cpp)`:
>   - Embedded inline vector drawing lambdas: `drawHLine`, `drawVLine`, and `drawFilledRect` built on `gameRenderer.drawQuad` using orthogonal projection (`orthoUI`).
>   - **Card Presentation**: $15.6 \times 8.6$ unit slate card with an ambient drop shadow (`glm::vec4(0, 0, 0, 0.65f * alpha)`), cyan outer accent frame (`glm::vec4(0.20f, 0.65f, 0.95f, 0.85f * alpha)`), and a top cyan header banner.
>   - **Titles & Underlines**: Centered `"Computer Graphics & Multimedia Lab"` (Ice Cyan) and `"Project Proposal"` (Gold) with an underline vector quad.
>   - **Course Metadata**: `"Course Title : Computer Graphics & Multimedia Lab"` and `"Course Code  : CSE0611328"`.
>   - **Vector Grid Tables**:
>     - **Faculty Table**: Dual-column grid with dark navy header fill, outer border, vertical column separator, and horizontal row separators displaying *Engineer Robiul Islam* (Assistant Professor) and *Tasneen Tanvir* (Lecturer).
>     - **Student Group Table**: Dual-column grid with header fill and row borders displaying *Gaus Saraf Murady* (`0432410005101088`), *Lima Akter* (`0432410005101063`), and *Fahima Akter* (`0432410005101093`).
>   - **Academic Footer**: Emerald metadata highlights for *Semester : Autumn 2026*, *Batch : 55*, and *Department : CSE*.
>   - **Cinematic Progress Bar & Skip Hint**: Smoothly fills according to `progress` with pulsing `[SPACE / ESC / ENTER] SKIP` prompt.
>
> **3. Pipeline & Memory Cleanup**:
> - Removed `splashTextureID` variable and `gameRenderer.loadTexture("assets/splash.png")` from `[/src/main.cpp](/src/main.cpp)`.
> - Removed texture deallocation calls (`glDeleteTextures`) as no GPU texture handles are allocated.
> - Purged `assets/splash.png` from disk and distribution folders, saving package size and zeroing memory overhead.

---

## 3. Actions, Files & Verification
### v7.0
- Copied uploaded document proposal image `media_1793627403679_8d8b7c15.png` to [`/assets/splash.png`](/assets/splash.png) and synced to [`/TheWenderer/assets/splash.png`](/TheWenderer/assets/splash.png).
- Modified [`/include/Common.h`](/include/Common.h) to add `STATE_INTRO_SPLASH = 0` to `enum GameState`.
- Modified [`/include/Renderer.h`](/include/Renderer.h) and [`/src/Renderer.cpp`](/src/Renderer.cpp) to support bilinear filtering (`GL_LINEAR`) in `Renderer::loadTexture`.
- Modified [`/include/UIManager.h`](/include/UIManager.h) and [`/src/UIManager.cpp`](/src/UIManager.cpp) to implement `UIManager::renderSplashScreen` with texture rendering and procedural fallback.
- Modified [`/src/main.cpp`](/src/main.cpp) to configure splash timing, main loop rendering, input handling, and decoupled intro video launching.
- Compiled and validated executable via `mingw32-make build` (Zero warnings/errors).
- Synced build binaries to [`/TheWenderer/TheWenderer.exe`](/TheWenderer/TheWenderer.exe).
- Updated documentation and changelog in [`/README.md`](/README.md), [`/doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md), and [`/doc/changelog.md`](/doc/changelog.md).

### v7.1
- Modified [`/include/Common.h`](/include/Common.h) to order `glad/glad.h` before `GLFW/glfw3.h` and define `GLFW_INCLUDE_NONE`.
- Compiled clean build with `make build` (Exit code 0).
- Synced binary to [`/TheWenderer/TheWenderer.exe`](/TheWenderer/TheWenderer.exe).
- Updated version history in [`/doc/changelog.md`](/doc/changelog.md) to `v0.7.1`.

### v7.2
- Updated [`/include/UIManager.h`](/include/UIManager.h) to simplify `renderSplashScreen` signature.
- Rewrote `UIManager::renderSplashScreen` in [`/src/UIManager.cpp`](/src/UIManager.cpp) using vector geometry, table grids, and colored typography.
- Updated [`/src/main.cpp`](/src/main.cpp) to remove `splashTextureID` and invoke the purely graphical splash screen.
- Deleted obsolete `assets/splash.png` from root and standalone folder.
- Recompiled and tested executable using `make build` (Exit code 0, clean build).
- Synced binary to [`/TheWenderer/TheWenderer.exe`](/TheWenderer/TheWenderer.exe).
- Updated version history in [`/doc/changelog.md`](/doc/changelog.md) (`v0.7.2`).

---

## 4. Outcomes
- **v7.0**: *(Refer to final response below)*

---
- **v7.1**: *(Refer to final response below)*

---
- **v7.2**: *(Refer to final response below)*

---
