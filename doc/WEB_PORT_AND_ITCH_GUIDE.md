# The Wenderer: Web Porting & itch.io Deployment Guide 🌐🕹️

> **Course**: Computer Graphics & Multimedia Lab (CSE0611328)  
> **Project**: The Wenderer (2D Multi-Elevation Endless Platformer & Runner)  
> **Target Platforms**: Web (WebAssembly / WebGL 2.0) & [itch.io](https://itch.io) Online Playable  
> **Native Baseline**: C++17, GLFW 3.3, OpenGL 3.3 Core (GLAD), WinMM/MCI Audio, Windows Media Foundation

---

## Table of Contents
1. [Executive Summary & Architectural Roadmap](#1-executive-summary--architectural-roadmap)
2. [Porting Matrix: Desktop vs. Web Architecture](#2-porting-matrix-desktop-vs-web-architecture)
3. [Prerequisites & Toolchain Setup (Emscripten SDK)](#3-prerequisites--toolchain-setup-emscripten-sdk)
4. [Step-by-Step Code Modifications in The Wenderer](#4-step-by-step-code-modifications-in-the-wenderer)
   - [Step 4.1: Asynchronous Browser Main Loop (`src/main.cpp`)](#step-41-asynchronous-browser-main-loop-srcmaincpp)
   - [Step 4.2: WebGL 2 & Shader Adaptation (`src/Renderer.cpp`)](#step-42-webgl-2--shader-adaptation-srcrenderercpp)
   - [Step 4.3: Cross-Platform Web Audio Bridge (`src/Audio.cpp`)](#step-43-cross-platform-web-audio-bridge-srcaudiocpp)
   - [Step 4.4: Video Playback & Splash Screen Handling (`src/VideoPlayer.cpp`)](#step-44-video-playback--splash-screen-handling-srcvideoplayercpp)
   - [Step 4.5: Virtual Filesystem & High Score Persistence](#step-45-virtual-filesystem--high-score-persistence)
5. [Build Pipelines & Automated Scripts](#5-build-pipelines--automated-scripts)
   - [Method A: Standalone Compilation Script (`build_web.bat` / `build_web.sh`)](#method-a-standalone-compilation-script-build_webbat--build_websh)
   - [Method B: CMake with `emcmake`](#method-b-cmake-with-emcmake)
   - [Method C: Makefile Target Integration](#method-c-makefile-target-integration)
6. [Custom itch.io HTML5 Shell Template (`shell_minimal.html`)](#6-custom-itchio-html5-shell-template-shell_minimalhtml)
7. [Local Testing & Verification (Avoiding CORS Pitfalls)](#7-local-testing--verification-avoiding-cors-pitfalls)
8. [Publishing on itch.io (Complete Step-by-Step Walkthrough)](#8-publishing-on-itchio-complete-step-by-step-walkthrough)
9. [Common Pitfalls, Web Autoplay & Debugging Cheat Sheet](#9-common-pitfalls-web-autoplay--debugging-cheat-sheet)

---

## 1. Executive Summary & Architectural Roadmap

**The Wenderer** was engineered as a high-performance, native desktop C++17 platformer utilizing hardware-accelerated Modern OpenGL 3.3 Core and Windows-native multimedia subsystems (`winmm.lib` for audio and Media Foundation for intro video decoding).

To make the game **playable online in modern web browsers** (Chrome, Firefox, Edge, Safari, Opera) and publishable on **itch.io** without requiring players to download an `.exe`, we compile the C++ codebase to **WebAssembly (`.wasm`)** and **JavaScript (`.js`)** using the **Emscripten SDK (`emsdk`)**.

WebAssembly executes at near-native speeds in the browser sandbox, while OpenGL 3.3 draw calls translate directly into **WebGL 2.0** (which corresponds to OpenGL ES 3.0).

```
+-----------------------------------------------------------------------------------+
|                            THE WENDERER ENGINE (C++17)                            |
+-----------------------------------------+-----------------------------------------+
|             DESKTOP BUILD               |                WEB BUILD                |
|               (Windows)                 |         (Emscripten / WebAssembly)      |
+-----------------------------------------+-----------------------------------------+
| Compiler: g++ / MSVC                    | Compiler: em++ (Emscripten)             |
| Window:   GLFW 3.3 (Desktop Win32)      | Window:   GLFW 3.3 (Emscripten Port)    |
| Graphics: OpenGL 3.3 Core + GLAD        | Graphics: WebGL 2.0 (GLES 3.0 / gl3.h)  |
| Shaders:  #version 330 core             | Shaders:  #version 300 es               |
| Loop:     Blocking while(!glfwClose)    | Loop:     emscripten_set_main_loop()    |
| Audio:    WinMM (MCI mciSendStringA)    | Audio:    Web Audio API / HTML5 Audio   |
| Video:    Media Foundation (mfplat.lib) | Video:    HTML5 <video> / Direct Splash |
| Files:    Direct OS Filesystem          | Files:    MEMFS Virtual Asset Pack      |
| Storage:  score_record.txt (Disk)       | Storage:  Browser localStorage / IDBFS  |
+-----------------------------------------+-----------------------------------------+
                                                       |
                                                       v
                                            +--------------------+
                                            |      itch.io       |
                                            |  Browser Playable  |
                                            |     HTML5 Game     |
                                            +--------------------+
```

---

## 2. Porting Matrix: Desktop vs. Web Architecture

| Subsystem | Desktop Implementation | Web / Browser Implementation | Required Changes |
|---|---|---|---|
| **Compilation Target** | `TheWenderer.exe` (PE x86_64) | `index.js` + `index.wasm` + `index.data` | Build with `em++` instead of `g++`. |
| **Main Loop** | Blocking `while (!glfwWindowShouldClose)` | Cooperative event loop (`emscripten_set_main_loop`) | Factor loop body into a single-frame callback function. |
| **Graphics API** | Desktop OpenGL 3.3 Core | WebGL 2.0 (OpenGL ES 3.0) | Replace `gladLoadGLLoader` with native Emscripten WebGL 2 loader. |
| **Shaders** | `#version 330 core` | `#version 300 es` with `precision highp float;` | Add `#version 300 es` header for Emscripten builds. |
| **Audio Subsystem** | `mciSendStringA(...)` (`winmm`) | Web Audio API / `Audio()` objects | Use Emscripten `EM_JS` JavaScript bridge for sound triggers. |
| **Cinematic Video** | Windows Media Foundation (`mfreadwrite.lib`) | HTML5 `<video>` element or procedural OpenGL splash screen | Web video bridge via DOM `<video>` or direct splash transition. |
| **Assets Directory** | `assets/` on physical disk | Preloaded Virtual File System (`--preload-file assets`) | Bundled automatically into `index.data`. File paths remain identical! |
| **Score Persistence** | `std::ofstream score_record.txt` | Virtual filesystem synced with `IDBFS` or `localStorage` | Minimal `EM_JS` wrapper for browser storage. |

---

## 3. Prerequisites & Toolchain Setup (Emscripten SDK)

The official compiler for compiling C/C++ to WebAssembly is **Emscripten**.

### 3.1 Installing Emscripten on Windows / Linux / macOS

1. Open PowerShell or Terminal and clone the official `emsdk` repository:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ```

2. Download and install the latest stable compiler tools:
   ```bash
   # Download and install the latest SDK tools
   ./emsdk install latest

   # Set the latest SDK as active
   ./emsdk activate latest
   ```

3. Activate the environment variables in your current terminal session:
   - **On Windows (PowerShell / CMD)**:
     ```cmd
     emsdk_env.bat
     ```
   - **On Linux / macOS (Bash / Zsh)**:
     ```bash
     source ./emsdk_env.sh
     ```

4. Verify the installation:
   ```bash
   em++ --version
   ```
   You should see output similar to `emcc (Emscripten gcc/clang-like replacement + linker emulating GCC) 3.1.x`.

> [!TIP]
> Keep the terminal window where you ran `emsdk_env.bat` open. Any build commands for the web port must be executed in an environment where Emscripten's tools (`em++`, `emcmake`) are in the `PATH`.

---

## 4. Step-by-Step Code Modifications in The Wenderer

To maintain **100% backward compatibility** with the native Windows desktop build, all platform-specific web changes are wrapped in `#ifdef __EMSCRIPTEN__` preprocessor checks.

### Step 4.1: Asynchronous Browser Main Loop (`src/main.cpp`)

#### The Problem
In desktop environments, the engine controls execution using a blocking while-loop:
```cpp
while (!glfwWindowShouldClose(window)) {
    // Process input, update physics, render frame
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```
In a web browser, JavaScript runs on a single-threaded cooperative event loop. If C++ enters an infinite `while(true)` loop, the browser cannot update the DOM, process user input, or repaint the screen. The tab will freeze immediately, prompting the browser to show: *"Page Unresponsive - Kill Page"*.

#### The Solution: `emscripten_set_main_loop`
Emscripten hooks into the browser's `requestAnimationFrame` API via `emscripten_set_main_loop`:

1. At the top of [`/src/main.cpp`](/src/main.cpp), include the Emscripten header:
   ```cpp
   #ifdef __EMSCRIPTEN__
   #include <emscripten.h>
   #endif
   ```

2. Extract the per-frame game update logic into a standalone tick function:
   ```cpp
   // Global or static reference to window for Emscripten callback
   GLFWwindow* g_window = nullptr;
   float lastFrameTime = 0.0f;

   void mainLoopTick() {
       float currentFrameTime = static_cast<float>(glfwGetTime());
       float rawDeltaTime = currentFrameTime - lastFrameTime;
       lastFrameTime = currentFrameTime;

       if (rawDeltaTime > 0.06f) rawDeltaTime = 0.06f; // Clamp delta time

       float deltaTime = (currentGameState == STATE_PAUSED || showTutorialModal) ? 0.0f : rawDeltaTime;

       // 1. Process Input
       glfwPollEvents();

       // 2. Update Scene & Physics Simulation
       updateGame(deltaTime); // Contains the physics, animations & combat logic

       // 3. Render Current State
       renderGame();

       // 4. Swap Framebuffers
       glfwSwapBuffers(g_window);

   #ifdef __EMSCRIPTEN__
       if (glfwWindowShouldClose(g_window)) {
           emscripten_cancel_main_loop();
       }
   #endif
   }
   ```

3. Refactor `int main()` to branch between Desktop and Web:
   ```cpp
   int main() {
       // ... [Existing initialization: GLFW, Window, Audio, Textures, World] ...

       g_window = window;
       lastFrameTime = static_cast<float>(glfwGetTime());

   #ifdef __EMSCRIPTEN__
       // 0 = use requestAnimationFrame (typically 60fps / 144fps matching the monitor)
       // 1 = simulate infinite loop (does not block execution)
       emscripten_set_main_loop(mainLoopTick, 0, 1);
   #else
       // Native Desktop Blocking Loop
       while (!glfwWindowShouldClose(window)) {
           mainLoopTick();
       }

       cleanupResources();
       glfwDestroyWindow(window);
       glfwTerminate();
   #endif

       return 0;
   }
   ```

---

### Step 4.2: WebGL 2 & Shader Adaptation (`src/Renderer.cpp`)

#### The Problem
1. Desktop uses `glad.c` and `#version 330 core`.
2. Browsers do not support Desktop OpenGL 3.3 Core directly; they support **WebGL 2.0**, which corresponds to **OpenGL ES 3.0** (`#version 300 es`).
3. Under Emscripten, `glad.c` is not needed because Emscripten automatically provides all standard WebGL 2 functions through `<GLES3/gl3.h>`.

#### The Solution: Shader Version Directives
In [`/src/Renderer.cpp`](/src/Renderer.cpp), adjust header includes and shader sources:

```cpp
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
```

Update vertex and fragment shader strings to support both WebGL 2 (`#version 300 es`) and Desktop OpenGL 3.3 (`#version 330 core`):

```cpp
#ifdef __EMSCRIPTEN__
const char *vertexShaderSource = R"(#version 300 es
precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 uMVP;
uniform vec4 uUVRect;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = vec2(uUVRect.x + aTexCoord.x * uUVRect.z,
                    uUVRect.y + aTexCoord.y * uUVRect.w);
}
)";

const char *fragmentShaderSource = R"(#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;
uniform bool uDiscardAlpha;
uniform bool uIsCircle;
uniform float uRimPower;

void main()
{
    if (uIsCircle) {
        vec2 p = TexCoord - vec2(0.5);
        float dist = length(p);
        if (dist > 0.5) discard;
        float edge = 1.0 - smoothstep(0.482, 0.50, dist);
        float rim = smoothstep(0.32, 0.49, dist);
        rim = pow(rim, max(0.2, uRimPower * 0.5));
        float core = smoothstep(0.50, 0.0, dist) * 0.22;
        float alpha = (core + rim * 0.85) * edge * uColor.a;
        vec3 col = mix(uColor.rgb, vec3(0.85, 0.96, 1.0), rim * 0.75);
        FragColor = vec4(col, alpha);
    } else if (uUseTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        if (uDiscardAlpha && texColor.a < 0.12) discard;
        FragColor = texColor * uColor;
    } else {
        FragColor = uColor;
    }
}
)";
#else
// [Existing #version 330 core shaders remain intact for native Windows builds]
#endif
```

In `init()` inside [`/src/Renderer.cpp`](/src/Renderer.cpp):
```cpp
bool Renderer::init() {
#ifndef __EMSCRIPTEN__
    // GLAD is only required on desktop
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
#endif
    // ... Compile shaders and set up Quad VAO/VBO ...
    return true;
}
```

---

### Step 4.3: Cross-Platform Web Audio Bridge (`src/Audio.cpp`)

#### The Problem
`src/Audio.cpp` utilizes Windows Multimedia (`mciSendStringA`, `mmsystem.h`, `winmm.lib`). This API does not exist on Linux, macOS, or in web browsers.

#### The Solution: Emscripten `EM_JS` Web Audio Bridge
Emscripten allows C++ to call inline JavaScript using the `EM_JS` macro. This gives us direct access to the browser's HTML5 Audio and Web Audio API without needing external third-party libraries!

Add the following web audio bridge to [`/src/Audio.cpp`](/src/Audio.cpp):

```cpp
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// JavaScript Bridge for Web Audio
EM_JS(void, js_play_sound, (const char* pathStr, float volume), {
    var path = UTF8ToString(pathStr);
    var audio = new Audio(path);
    audio.volume = Math.max(0.0, Math.min(1.0, volume));
    audio.play().catch(function(err) {
        // Suppress browser autoplay warning if player hasn't interacted yet
        console.log("Audio autoplay waiting for user interaction:", err);
    });
});

EM_JS(void, js_play_bgm, (const char* pathStr, float volume), {
    var path = UTF8ToString(pathStr);
    if (!window.g_bgmAudio) {
        window.g_bgmAudio = new Audio(path);
        window.g_bgmAudio.loop = true;
    }
    window.g_bgmAudio.volume = Math.max(0.0, Math.min(1.0, volume));
    window.g_bgmAudio.play().catch(function(err) {
        console.log("BGM autoplay waiting for user interaction:", err);
    });
});

EM_JS(void, js_set_bgm_volume, (float volume), {
    if (window.g_bgmAudio) {
        window.g_bgmAudio.volume = Math.max(0.0, Math.min(1.0, volume));
    }
});

EM_JS(void, js_stop_bgm, (), {
    if (window.g_bgmAudio) {
        window.g_bgmAudio.pause();
        window.g_bgmAudio.currentTime = 0;
    }
});
#endif
```

Now, map the existing `AudioManager` methods to these JS calls when compiling for Web:

```cpp
void AudioManager::playBGM() {
#ifdef __EMSCRIPTEN__
    if (!m_bgmMuted) {
        js_play_bgm("assets/Quarter_in_the_Slot.mp3", m_bgmVolume);
    }
#else
    // Existing MCI Windows implementation...
    mciSendStringA("open \"assets/Quarter_in_the_Slot.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
    mciSendStringA("play bgm repeat", NULL, 0, NULL);
#endif
}

void AudioManager::setBGMVolume(float volume) {
    m_bgmVolume = std::clamp(volume, 0.0f, 1.0f);
#ifdef __EMSCRIPTEN__
    js_set_bgm_volume(m_bgmMuted ? 0.0f : m_bgmVolume);
#else
    // Existing Windows MCI volume implementation...
#endif
}

void AudioManager::playKatanaSlash() {
#ifdef __EMSCRIPTEN__
    js_play_sound("assets/katana-slash.mp3", 0.7f);
#else
    PlaySoundA("assets/katana-slash.wav", NULL, SND_ASYNC | SND_FILENAME);
#endif
}

void AudioManager::playSlimeDeathSplat() {
#ifdef __EMSCRIPTEN__
    js_play_sound("assets/enemies/boss_demon_slime/universfield-wet-splat-impact-567197.mp3", 0.8f);
#else
    mciSendStringA("play sfx_splat from 0", NULL, 0, NULL);
#endif
}
```

> [!NOTE]
> Web browsers enforce an **Autoplay Policy**: Audio is automatically muted until the user clicks or presses a key on the webpage. Since *The Wenderer* starts with an Academic Splash Screen where the player presses `Space` or `Enter`, audio activates smoothly on first input!

---

### Step 4.4: Video Playback & Splash Screen Handling (`src/VideoPlayer.cpp`)

#### The Problem
[`/src/VideoPlayer.cpp`](/src/VideoPlayer.cpp) utilizes Windows Media Foundation (`mfplat.lib`, `mfreadwrite.lib`, `MFAudioFormat_PCM`). WebAssembly running inside browser sandboxes cannot invoke Windows COM interfaces.

#### The Solution: Procedural Splash or HTML5 Video
*The Wenderer* already contains an **academic procedural splash screen** rendered in pure Modern OpenGL (added in `v0.7.2`).

For the web build, the cleanest and most robust approach is:
1. In `VideoPlayer.h` and `VideoPlayer.cpp`, stub out Media Foundation calls under `#ifdef __EMSCRIPTEN__`.
2. Transition directly from the academic splash screen to the main title screen / gameplay when `STATE_INTRO_VIDEO` finishes or is skipped:

```cpp
#ifdef __EMSCRIPTEN__
bool VideoPlayer::open(const std::wstring& filepath) {
    // Under WebAssembly, skip heavy video decoding and mark as finished
    // or trigger an HTML5 <video> overlay via EM_JS
    m_hasFinished = true;
    m_isPlaying = false;
    return true;
}
void VideoPlayer::update(float dt) {}
void VideoPlayer::render() {}
void VideoPlayer::close() {}
#endif
```

This saves over **4.8 MB** of download size for web players while preserving the procedural academic introduction screen!

---

### Step 4.5: Virtual Filesystem & High Score Persistence

#### Assets Virtual Filesystem (`--preload-file assets`)
When compiling with Emscripten, passing the flag:
```bash
--preload-file assets
```
instructs Emscripten to scan the [`assets/`](/assets/) directory and package all PNG textures, fonts, and MP3 audio files into a binary file named `index.data`.

At runtime, Emscripten mounts this virtual filesystem into memory (`MEMFS`) at `/assets/`. All standard C/C++ path calls such as:
```cpp
gameRenderer.loadTexture("assets/samurai/Run.png");
```
continue to work without modifying a single line of code!

#### High Score Persistence (`score_record.txt`)
On desktop, high scores are stored in `score_record.txt`. In a browser, standard file writes are stored in temporary RAM and lost on page refresh.

To persist the high score in the user's browser, use `localStorage`:

```cpp
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(int, js_load_high_score, (), {
    var score = localStorage.getItem("thewenderer_highscore");
    return score ? parseInt(score, 10) : 0;
});

EM_JS(void, js_save_high_score, (int score), {
    localStorage.setItem("thewenderer_highscore", score);
});
#endif
```

Inside `loadScoreHistory()` and `saveScoreHistory()`:
```cpp
void loadScoreHistory() {
#ifdef __EMSCRIPTEN__
    highestScoreAllTime = js_load_high_score();
#else
    std::ifstream inFile("score_record.txt");
    if (inFile >> highestScoreAllTime) { /* Loaded */ }
#endif
}

void saveScoreHistory(int newScore) {
#ifdef __EMSCRIPTEN__
    js_save_high_score(newScore);
#else
    std::ofstream outFile("score_record.txt");
    outFile << newScore;
#endif
}
```

---

## 5. Build Pipelines & Automated Scripts

### Method A: Standalone Compilation Script (`build_web.bat` / `build_web.sh`)

Create a script named `build_web.bat` in the repository root:

```cmd
@echo off
echo =========================================================================
echo Compiling The Wenderer for Web (WebAssembly + WebGL 2) via Emscripten...
echo =========================================================================

REM 1. Ensure build-web directory exists
if not exist build-web mkdir build-web

REM 2. Compile C++ sources into WebAssembly
call em++ ^
    src/main.cpp ^
    src/Renderer.cpp ^
    src/World.cpp ^
    src/UIManager.cpp ^
    src/LadderScene.cpp ^
    src/SkyFallScene.cpp ^
    src/Audio.cpp ^
    src/VideoPlayer.cpp ^
    -Iinclude ^
    -std=c++17 ^
    -O3 ^
    -s USE_GLFW=3 ^
    -s MIN_WEBGL_VERSION=2 ^
    -s MAX_WEBGL_VERSION=2 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s INITIAL_MEMORY=67108864 ^
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" ^
    --preload-file assets ^
    --shell-file shell_minimal.html ^
    -o build-web/index.html

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Web build completed successfully!
    echo Output generated in: build-web/
    echo Files: index.html, index.js, index.wasm, index.data
) else (
    echo.
    echo [ERROR] Web build failed with error code %ERRORLEVEL%.
)
```

#### Breakdown of Important `em++` Compiler Flags:
- `-std=c++17`: Matches the desktop standard.
- `-O3`: Maximum speed and size optimization for WebAssembly bytecode.
- `-s USE_GLFW=3`: Links Emscripten's built-in GLFW 3 emulation layer.
- `-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2`: Targets WebGL 2.0 (OpenGL ES 3.0), matching the modern shaders.
- `-s ALLOW_MEMORY_GROWTH=1`: Permits the WebAssembly heap to expand dynamically as textures and chunk buffers are allocated.
- `-s INITIAL_MEMORY=67108864`: Allocates an initial 64MB memory pool to prevent reallocations on game startup.
- `--preload-file assets`: Automatically packages the entire `assets/` folder into `index.data`.
- `--shell-file shell_minimal.html`: Embeds the game inside our custom, clean itch.io-ready HTML template.

---

### Method B: CMake with `emcmake`

If using CMake, configure `CMakeLists.txt` to detect Emscripten:

```cmake
if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s USE_GLFW=3 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    
    # Exclude Windows-specific static libraries
    target_link_libraries(TheWenderer "")
    set_target_properties(TheWenderer PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@assets --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell_minimal.html")
else()
    # Native Desktop Linkage
    target_link_libraries(TheWenderer glfw3 opengl32 gdi32 winmm mfplat mfreadwrite mfuuid ole32)
endif()
```

Run the build commands:
```bash
emcmake cmake -B build-web
cmake --build build-web --config Release
```

---

### Method C: Makefile Target Integration

Add a `web` target directly to [`/Makefile`](/Makefile):

```makefile
web:
	@if not exist build-web mkdir build-web
	em++ src/main.cpp src/Renderer.cpp src/World.cpp src/UIManager.cpp src/LadderScene.cpp src/SkyFallScene.cpp src/Audio.cpp src/VideoPlayer.cpp \
		-Iinclude -std=c++17 -O3 -s USE_GLFW=3 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 \
		-s ALLOW_MEMORY_GROWTH=1 --preload-file assets --shell-file shell_minimal.html \
		-o build-web/index.html
```

---

## 6. Custom itch.io HTML5 Shell Template (`shell_minimal.html`)

By default, Emscripten produces an HTML template with a giant text log and redundant UI elements. For itch.io, we need a **sleek, responsive, dark-themed canvas** with crisp pixel rendering.

Save the following file as `shell_minimal.html` in the root of the project:

```html
<!DOCTYPE html>
<html lang="en-us">
<head>
    <meta charset="utf-8">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <title>The Wenderer - Online Web Edition</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            background-color: #0d0f14;
            color: #d8dee9;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            overflow: hidden;
        }
        #game-container {
            position: relative;
            width: 1024px;
            height: 720px;
            max-width: 100vw;
            max-height: 100vh;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.7);
            border-radius: 4px;
            overflow: hidden;
            background: #000;
        }
        canvas.emscripten {
            border: 0 none;
            width: 100%;
            height: 100%;
            display: block;
            image-rendering: -moz-crisp-edges;
            image-rendering: -webkit-crisp-edges;
            image-rendering: pixelated;
            image-rendering: crisp-edges;
            outline: none;
        }
        #loading-overlay {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: #0b0c10;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            z-index: 10;
            transition: opacity 0.5s ease;
        }
        .spinner {
            width: 50px;
            height: 50px;
            border: 5px solid rgba(0, 229, 255, 0.2);
            border-top-color: #00e5ff;
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
        #loading-text {
            margin-top: 20px;
            font-size: 16px;
            font-weight: 600;
            letter-spacing: 1px;
            color: #66fcf1;
        }
        #progress-bar-container {
            width: 300px;
            height: 8px;
            background: #1f2833;
            border-radius: 4px;
            margin-top: 15px;
            overflow: hidden;
        }
        #progress-bar {
            width: 0%;
            height: 100%;
            background: linear-gradient(90deg, #45a29e, #66fcf1);
            transition: width 0.2s ease;
        }
        .controls-hint {
            margin-top: 12px;
            font-size: 13px;
            color: #8892b0;
            text-align: center;
        }
        .controls-hint kbd {
            background-color: #1b202c;
            border: 1px solid #303c54;
            border-radius: 3px;
            padding: 2px 6px;
            font-size: 11px;
            color: #e6f1ff;
        }
    </style>
</head>
<body>
    <div id="game-container">
        <div id="loading-overlay">
            <div class="spinner"></div>
            <div id="loading-text">Loading The Wenderer (WebAssembly)...</div>
            <div id="progress-bar-container">
                <div id="progress-bar"></div>
            </div>
        </div>
        <canvas class="emscripten" id="canvas" oncontextmenu="event.preventDefault()" tabindex="-1"></canvas>
    </div>

    <div class="controls-hint">
        Click inside the game window to lock focus | Controls: <kbd>A</kbd>/<kbd>D</kbd> Move &bull; <kbd>Space</kbd> Jump &bull; <kbd>J</kbd> Attack &bull; <kbd>K</kbd> Defend &bull; <kbd>ESC</kbd> Pause
    </div>

    <script type="text/javascript">
        var loadingOverlay = document.getElementById('loading-overlay');
        var loadingText = document.getElementById('loading-text');
        var progressBar = document.getElementById('progress-bar');

        var Module = {
            preRun: [],
            postRun: [function() {
                // Focus the canvas immediately after launch
                var canvas = document.getElementById('canvas');
                if (canvas) canvas.focus();
            }],
            print: function(text) { console.log(text); },
            printErr: function(text) { console.error(text); },
            canvas: (function() {
                var canvas = document.getElementById('canvas');
                canvas.addEventListener("webglcontextlost", function(e) {
                    alert('WebGL context lost. Please reload the page.');
                    e.preventDefault();
                }, false);
                return canvas;
            })(),
            setStatus: function(text) {
                if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
                if (text === Module.setStatus.last.text) return;
                var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
                var now = Date.now();
                if (m) {
                    var current = parseInt(m[2]);
                    var total = parseInt(m[4]);
                    var pct = Math.round((current / total) * 100);
                    progressBar.style.width = pct + '%';
                    loadingText.innerText = 'Downloading Game Assets (' + pct + '%)...';
                } else if (!text) {
                    loadingOverlay.style.opacity = '0';
                    setTimeout(function() {
                        loadingOverlay.style.display = 'none';
                    }, 500);
                } else {
                    loadingText.innerText = text;
                }
            },
            totalDependencies: 0,
            monitorRunDependencies: function(left) {
                this.totalDependencies = Math.max(this.totalDependencies, left);
                Module.setStatus(left ? 'Preparing Engine (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')...' : '');
            }
        };

        Module.setStatus('Downloading WebAssembly binary...');

        window.onerror = function() {
            loadingText.innerText = 'Error loading game. Check browser console (F12).';
            loadingText.style.color = '#ff6b6b';
        };
    </script>
    {{{ SCRIPT }}}
</body>
</html>
```

---

## 7. Local Testing & Verification (Avoiding CORS Pitfalls)

> [!WARNING]
> **Do NOT double-click `build-web/index.html` directly in your file explorer!**  
> Modern web browsers block loading WebAssembly (`.wasm`) and binary asset packs (`.data`) via `file://` protocols due to Cross-Origin Resource Sharing (CORS) security restrictions. You will see an error: `Fetch API cannot load file://... URL scheme must be "http" or "https"`.

### Running a Local HTTP Server

You can launch a local web server using one of the following simple one-line commands:

#### Option 1: Using Python 3 (Installed by default on most systems)
In PowerShell or Terminal:
```bash
python -m http.server 8080 --directory TheWenderer_Web
```
Open your browser and navigate to: `http://localhost:8080`

#### Option 2: Using Emscripten's Built-in `emrun`
```bash
emrun --port 8080 TheWenderer_Web/index.html
```

#### Option 3: Using Node.js `npx serve`
```bash
npx serve TheWenderer_Web
```

### Verification Checklist in Browser Developer Tools (F12):
1. **Console Tab**: Verify there are no red errors regarding `WebAssembly.instantiate` or missing shader uniforms.
2. **Network Tab**: Ensure `index.wasm` (status 200) and `index.data` (status 200) load completely.
3. **Audio Autoplay**: Verify that when you click the canvas or press `Space`, background music (`Quarter_in_the_Slot.mp3`) starts playing.
4. **Framerate**: Verify smooth 60 FPS rendering in the Chrome Performance tab.

---

## 8. Publishing on itch.io (Complete Step-by-Step Walkthrough)

Deploying *The Wenderer* to itch.io takes only a few minutes.

### 8.1 Preparing the itch.io ZIP Package

Itch.io requires HTML5 games to be submitted as a single `.zip` archive. **Crucially, `index.html` must be placed directly at the root of the `.zip` file** (not nested inside a folder!).

The build script [`/build_web.ps1`](/build_web.ps1) and [`/Makefile`](/Makefile) (`make web` or `make package-web`) automatically compiles the code and generates the upload-ready ZIP inside [`/TheWenderer_Web/`](/TheWenderer_Web/):

```powershell
powershell -ExecutionPolicy Bypass -File build_web.ps1
```

The resulting zip archive is generated directly at [`/TheWenderer_Web/TheWenderer_Web_itch.zip`](/TheWenderer_Web/TheWenderer_Web_itch.zip) and contains:
```
TheWenderer_Web_itch.zip
├── index.html
├── index.js
├── index.wasm
└── index.data
```

---

### 8.2 Creating & Configuring the itch.io Project

1. Log in to your [itch.io](https://itch.io) creator dashboard and click **Create new project**.
2. Fill in the general metadata:
   - **Title**: `The Wenderer: 2D Multi-Elevation Platformer`
   - **Project URL**: `https://<your-username>.itch.io/the-wenderer`
   - **Short description / Tagline**: `A multi-elevation 2D runner and combat platformer featuring Samurai Mack, subterranean cavern traversal, and dynamic boss battles.`
   - **Classification**: **Games**

3. **Kind of Project (CRITICAL SETTING)**:
   - Select **HTML** from the dropdown menu: *"You have a ZIP or HTML file that will be played in the browser"*.

```
+-------------------------------------------------------------+
| Kind of project                                             |
| [ HTML - You have a ZIP or HTML file that will be played...]|
+-------------------------------------------------------------+
```

4. **Uploads Section**:
   - Click **Upload files** and select `TheWenderer_Web_itch.zip`.
   - Once uploaded, check the checkbox:  
     ☑ **This file will be played in the browser**

```
+-------------------------------------------------------------+
| [ TheWenderer_Web_itch.zip ]   [12.4 MB]                    |
| [*] This file will be played in the browser                 |
+-------------------------------------------------------------+
```

5. **Embed Options**:
   - **Viewport dimensions**:
     - **Width**: `1024`
     - **Height**: `720`
   - **Orientation**: Landscape
   - **Frame options**:
     - ☑ **Enable fullscreen button** (Allows players to enter full screen with `F11` or via itch.io's bottom-right overlay button).
     - ☑ **Automatically start on page load** (or leave unchecked if you want a "Click to Play" cover).
     - ☑ **Mobile friendly**: Leave unchecked (optimized for keyboard & mouse).

6. **Details & Controls Markdown**:
   Copy and paste the controls reference from [`/README.md`](/README.md):
   ```markdown
   ### Controls:
   - **A / D**: Walk & Run (Free Exploration)
   - **Space / W**: Jump across platforms or climb ladder shafts
   - **S / Down Arrow**: Descend into underground cavern ladders
   - **Left Shift**: Dash burst with hazard invulnerability
   - **J / X / Left Click**: Sword Slash Attack
   - **K / Right Click**: Defend / Parry Stance
   - **M**: Mute / Unmute Background Music
   - **Esc**: Pause Menu & Volume Slider
   - **F11**: Fullscreen
   ```

7. **Save & View**:
   - Set visibility to **Draft** or **Restricted** (with password) to test the live embed first.
   - Click **Save & View Page**.
   - Test gameplay, audio, and high score saving directly in the itch.io browser player.
   - When satisfied, switch visibility to **Public**!

---

## 9. Common Pitfalls, Web Autoplay & Debugging Cheat Sheet

### 1. Browser Autoplay Policy (No Audio on Startup)
- **Symptom**: Game loads and runs, but background music and sound effects do not play.
- **Cause**: Modern browsers (Chrome, Safari, Edge) block audio context instantiation until a user interaction (mouse click or keypress) occurs within the document.
- **Fix**: *The Wenderer* starts on the Academic Splash Screen where players must press `Space`, `Enter`, or click to proceed. Hook audio initialization to this first key/click event.

### 2. High-DPI / Retina Blurriness
- **Symptom**: Pixel art looks slightly blurry on 4K or MacBook Retina screens.
- **Fix**: The CSS rule in `shell_minimal.html`:
  ```css
  canvas.emscripten {
      image-rendering: pixelated;
      image-rendering: crisp-edges;
  }
  ```
  ensures nearest-neighbor scaling. Additionally, keep `initFbW` and `initFbH` locked to `1024x720` rather than scaling canvas internal resolution.

### 3. Out-of-Memory / Heap Growth
- **Symptom**: Browser tab crashes with `Cannot enlarge memory arrays. Either (1) compile with -sALLOW_MEMORY_GROWTH=1...`.
- **Fix**: Ensure `-sALLOW_MEMORY_GROWTH=1` is present in the `em++` command line.

### 4. Arrow Keys and Space Scrolling the Web Page
- **Symptom**: Pressing Space or Down Arrow scrolls the itch.io webpage down while playing.
- **Fix**: In `shell_minimal.html`, attach a key listener that calls `e.preventDefault()` for game keys:
  ```javascript
  window.addEventListener("keydown", function(e) {
      if(["Space", "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"].indexOf(e.code) > -1) {
          e.preventDefault();
      }
  }, false);
  ```

---

## 10. Summary & Next Steps

By leveraging **Emscripten**, **WebGL 2**, and **WebAssembly**, *The Wenderer* transitions from a Windows-only desktop executable into a cross-platform web experience playable on any device with a modern browser.

All existing engine math, OpenGL quad-batching, multi-elevation platform generation, and samurai combat remain completely intact, proving the strength and flexibility of the modular architecture.

For implementation plans and session transcripts, see:
- [`/doc/prompts/11. Web Port & Itch.io Deployment Guide/summary.md`](/doc/prompts/11.%20Web%20Port%20%26%20Itch.io%20Deployment%20Guide/summary.md)
- [`/doc/prompts/11. Web Port & Itch.io Deployment Guide/11.1 Implementation Plan - Web Port and Itch Deployment.md`](/doc/prompts/11.%20Web%20Port%20%26%20Itch.io%20Deployment%20Guide/11.1%20Implementation%20Plan%20-%20Web%20Port%20and%20Itch%20Deployment.md)

