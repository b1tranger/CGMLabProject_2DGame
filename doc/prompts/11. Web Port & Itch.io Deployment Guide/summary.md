# Conversation Summary: Session 11 — Web Port & Itch.io Deployment Guide + WebGL 1-Click Setup Script

## 1. Session Metadata
- **Date**: 2026-11-13
- **Conversation ID**: `2d323f1e-f3b4-4713-94a4-8240185698a6`
- **Session Archive Folder**: [`/doc/prompts/11. Web Port & Itch.io Deployment Guide/`](/doc/prompts/11.%20Web%20Port%20%26%20Itch.io%20Deployment%20Guide/)
- **Topics**: WebAssembly (Wasm), WebGL 2.0, OpenGL ES 3.0, Emscripten Toolchain (`emsdk`), HTML5 Canvas, Web Audio API, itch.io Web Distribution, Standalone WebGL 1-Click Setup Automation Repository (`webgl_setup_script/`), itch.io Upload Troubleshooting.

---

## 2. Key Goals & User Requests
1. **Itch.io Web Port Guide**:
   - Provide a comprehensive technical and actionable deployment guide for porting *The Wenderer* into a web game playable online on itch.io.
   - Document in [`/doc/WEB_PORT_AND_ITCH_GUIDE.md`](/doc/WEB_PORT_AND_ITCH_GUIDE.md).
2. **Execute Full Web Build**:
   - Refactor codebase for cross-platform WebAssembly/WebGL 2.0 while maintaining 100% desktop compatibility.
   - Compile and package release bundle and zip ready for itch.io upload in [`/TheWenderer_Web/`](/TheWenderer_Web/).
3. **WebGL 1-Click Setup Package (`webgl_setup_script/`)**:
   - Answer whether WebGL requires installation similar to `D:\GitHub\[personal]\opengl_setup_script`.
   - Create a localized, turnkey repository folder `webgl_setup_script/` in project root and add it to `.gitignore`.
   - Implement self-elevating admin installer/uninstaller, diagnostic test tool, boilerplate templates, project scaffolder, and 3 progressive sample projects.
4. **Resolve itch.io Upload & Runtime Errors**:
   - Resolve `"Failed to find index.html"`.
   - Resolve `"Quarter_in_the_Slot.mp3 404 (Not Found)"` and game appearing stuck on `"RUNNING"`.

---

## 3. Engineering & Architecture Deliverables

### A. Core Engine WebGL 2.0 & WebAssembly Adaptation
- [`/src/main.cpp`](/src/main.cpp):
  - Refactored main loop from blocking `while(!glfwWindowShouldClose())` into `emscripten_set_main_loop(mainLoopIteration, 0, 1)`, syncing render ticks to `requestAnimationFrame`. Added web `localStorage` persistence for high scores.
  - Set WebGL 2.0 / GLES 3.0 window hints (`GLFW_CLIENT_API, GLFW_OPENGL_ES_API`, major 3, minor 0).
  - Added left mouse click dismissal for `showTutorialModal` and `STATE_INTRO_SPLASH`.
- [`/src/Renderer.cpp`](/src/Renderer.cpp): Conditionalized shaders for `#version 300 es` with `precision highp float;` and WebGL-compliant coordinate system.
- [`/include/Common.h`](/include/Common.h): Conditionalized OpenGL headers to `<GLES3/gl3.h>` and `<emscripten.h>` under `__EMSCRIPTEN__`.
- [`/src/Audio.cpp`](/src/Audio.cpp):
  - Built an in-memory Web Audio bridge via `EM_JS`: reads audio bytes directly from Emscripten's virtual filesystem (`FS.readFile(path)`), wraps into a `Blob`, and creates an in-memory `blob:` URL. Completely eliminates HTTP 404 errors on itch.io CDN.
- [`/src/VideoPlayer.cpp`](/src/VideoPlayer.cpp): Created safe WebAssembly stubs bypassing Windows Media Foundation.
- [`/shell_minimal.html`](/shell_minimal.html): Added automatic canvas focus and audio context resume on click, touch, and keydown.

### B. Deployment Artifacts
- **Output Directory**: [`/TheWenderer_Web/`](/TheWenderer_Web/)
  - `index.html` (4.3 KB): Responsive, dark-themed viewport with loading progress monitor and auto-focus event handling.
  - `index.js` (150.6 KB): Emscripten WebAssembly runtime glue code.
  - `index.wasm` (440.8 KB): Optimized binary executable bytecode.
  - `index.data` (13.0 MB): Virtual filesystem package containing game textures, audio, fonts, and levels.
  - `TheWenderer_Web_itch.zip` (12.8 MB): Production-ready zip package with `index.html` sitting at the root of the archive.

### C. Standalone `webgl_setup_script/` Repository
Modeled after `D:\GitHub\[personal]\opengl_setup_script`, gitignored from the main repo:
- **`install_webgl_admin.bat`**: Self-elevating 1-Click installer deploying `C:\emsdk`, compiling LLVM wasm toolchain, and setting `EM_CACHE=C:\emsdk\cache`.
- **`uninstall_webgl_admin.bat`**: Clean uninstaller purging system PATH and SDK directories.
- **`test_webgl_env.bat`**: Verification tool checking Python, Node, Git, `em++`, and compiling a test wasm binary.
- **`Dependencies/template/`**: Boilerplate C++ WebGL 2.0 project (`Common.h`, `main.cpp`, `shell_minimal.html`, `Makefile`, `build.bat`).
- **`Dependencies/server/run_local_server.bat`**: 1-Click local HTTP server bypassing browser CORS security restrictions.
- **`PROJECTS/create_webgl_project.bat`**: Interactive project scaffolder.
- **`sample_projects/`**:
  - `Lab0_WebGL_ClearScreen/`: Minimal GLFW + WebGL 2.0 clear screen and color oscillation.
  - `Lab1_WebGL_Triangle/`: Modern GLSL 3.00 ES RGB triangle with VAO/VBO.
  - `Lab2_WebGL_2D_Game_Sprite/`: 2D sprite physics, keyboard input, Web Audio synthesis bridge, and `--preload-file assets`.
- **`README.md` & `guide.md`**: Complete setup instructions and comprehensive technical manual.

---

## 4. Verification & Testing
1. **Desktop Build Regression**: Recompiled desktop binary with `build/win_tasks.ps1` (exit code 0).
2. **Web Build Validation**: Successfully recompiled web port to `TheWenderer_Web/` (exit code 0).
3. **Archive Verification**: Verified `TheWenderer_Web_itch.zip` contains `index.html`, `index.js`, `index.wasm`, and `index.data` at root level.
4. **Git Isolation**: Confirmed `webgl_setup_script/` is ignored by root `.gitignore`.
