
# The Wenderer: A Multi-Directional Endless Runner

> Computer Graphics & Multimedia Lab Project (CSE0611328)  
> Department of Computer Science & Engineering, UITS

---

## 🎮 About The Project

**The Wenderer** is a 2D multi-directional endless runner built in **C++** using **Modern OpenGL (3.3 Core Profile)**. 

Unlike traditional endless runners where movement is locked to a single forward axis, **The Wenderer** features dynamic traversal redirections (90-degree orthogonal pivot points) called **"Wending"**, paired with a smooth rotational camera follow, vector-adaptive obstacles, and AABB collision detection.

---

## 🚀 Quick Start (Build & Run)

### Requirements
- MinGW-w64 (`g++` supporting C++17) or CMake
- All OpenGL libraries (`GLFW`, `GLAD`, `GLM`) are already bundled in the repo!

### Building with Makefile (Recommended)
```bash
# Build the project
mingw32-make

# Or compile directly with g++:
g++.exe -std=c++17 -Wall -Wextra -O2 -I./include ./src/main.cpp ./src/glad.c -o ./build/main.exe -L./lib -lglfw3 -lopengl32 -lgdi32

# Run the game
./build/main.exe
```

### Controls (Phase 1)
- **`A` / `Left Arrow`**: Move left relative to running path
- **`D` / `Right Arrow`**: Move right relative to running path
- **`W` / `Up Arrow` / `Space`**: Jump over low hurdles
- **`Space`**: Restart after Game Over
- **`Esc`**: Exit game

---

## 📚 Documentation & Guides

- **[Learning & Implementation Guide](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/LEARNING_GUIDE.md)**: Beginner-friendly step-by-step breakdown of OpenGL concepts, mathematics, and guided milestones.
- **[Project Proposal](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf)**: Formal proposal for the CGM Lab course.
- **[Research Analysis](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/research/Platformer%20Game%20Research%20Analysis.pdf)**: Architectural, ludonarrative, and market research.
- **[Implementation Steps](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/research/implementationSteps.pdf)**: Core roadmap and code milestones.

---

## 📜 Version History (Changelog)

### [v0.1.0] - 2026-10-17 (Phase 1 Complete)
- **Reusable Quad & Shader**: Implemented a unified $1 \times 1$ Quad VAO/VBO and MVP GLSL shader replacing individual hardcoded triangle geometry.
- **Delta-Time Game Loop**: Added frame-rate independent physics and rendering loop using GLFW time.
- **Vector-Adaptive Movement**: Added forward traversal with lateral lane shifting and jumping relative to the active movement axis.
- **Procedural Obstacles & AABB Collision**: Added dynamic track generation, multiple obstacle types (hurdles vs barrier blocks), and AABB overlap detection with game reset states.
- **The "Wending" Mechanic**: Implemented 90-degree orthogonal track pivot points and smooth rotational camera follow using `glm::mix`.
- **Project Structure & Rules**: Bundled all necessary third-party libraries (`GLFW`, `GLAD`, `GLM`), created `Makefile`, `CMakeLists.txt`, `.gitignore`, and `AGENTS.md`.
