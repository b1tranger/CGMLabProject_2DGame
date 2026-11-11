# Conversation Archive: Session 1 - Phase 1 Prototype & Documentation

- **Session Number**: 1
- **Legacy Versions**: `v1.0 - v1.2`
- **Date(s)**: 2026-10-17 to 2026-10-21
- **Session Folder**: [`/doc/prompts/1. Phase 1 Prototype & Documentation/`](/doc/prompts/1.%20Phase%201%20Prototype%20&%20Documentation/)
- **Transcript Status**: Migrated verbatim from legacy Markdown session logs (raw `transcript_full.jsonl` not available for pre-Session-9 conversations).

---

## Original Raw Prompt Notes (Pre-Archive)

### `b1tranger.txt`
```text
For our CGM lab project, we need to make something using OpenGL. 

Read out project proposal from [CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf](/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf) and the research done on related ideas from [Platformer%20Game%20Research%20Analysis.pdf](/doc/research/Platformer%20Game%20Research%20Analysis.pdf) and implement the Phase 1 from [implementationSteps.pdf](/doc/research/implementationSteps.pdf) .
```

### `lima.txt`
```text
We need to build something for out CGM lab project. Our proposal was [CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf](/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf) . And other related files are at [research](/doc/research) . 

First explain me what is the project about.

Then create a Documenation in [doc](/doc) for learning purpose with guided steps explained in simple language. The goal is to simplify the implementation process. 

Also add a possible file structure for the project.
```

---

## Turn v1.0 - Project Analysis and Phase 1 Implementation - v1.0

- **Session Version**: v1.0
- **Date**: 2026-10-17
- **Context**: Project explanation, guided documentation generation, and complete Phase 1 implementation of The Wenderer endless runner.

---

### User Input
> We need to build something for our CGM lab project. Our proposal was [CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf](/doc/research/CSE55_088_063_093_CGMLab_Project_Proposal.docx.pdf) . And other related files are at [research](/doc/research) .
> First explain me what is the project about.
> Then create a Documentation in [doc](/doc) for learning purpose with guided steps explained in simple language. The goal is to simplify the implementation process.
> Also add a possible file structure for the project.
> Read out project proposal and implement Phase 1 from implementationSteps.pdf.

### High-Level Architectural Rationale & Design Decisions
> 1. Analyzed the project proposal, research analysis, and implementation roadmap. The core novelty is "Wending"—sudden 90-degree orthogonal shifts in running direction paired with a smoothly rotating camera follow.
> 2. Replaced multi-triangle hardcoded geometry with a single reusable 1x1 Quad VAO/VBO transformed via Model-View-Projection (MVP) matrices in GLSL, significantly reducing memory and draw call overhead.
> 3. Modeled track segments as vector-oriented lines and bounding areas. At the end of each segment, direction indices cycle, invoking `glm::mix` to interpolate camera rotation over ~0.45 seconds.
> 4. Structured collision detection using Axis-Aligned Bounding Box (AABB) checks evaluated against active obstacle arrays each frame.
> 5. Bundled GLFW, GLAD, and GLM libraries locally to make compilation completely independent and self-contained with zero external installation friction.

### Actions & Tools Executed
- Read project proposals and research papers in [`doc/research/`](/doc/research).
- Created beginner-friendly learning guide [`doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md).
- Created full Phase 1 game engine in [`src/main.cpp`](/src/main.cpp).
- Created build systems [`Makefile`](/Makefile) and [`CMakeLists.txt`](/CMakeLists.txt).
- Created [`.gitignore`](/.gitignore) to exclude build binaries (`build/`, `*.exe`, `*.o`).
- Created [`AGENTS.md`](/AGENTS.md) defining chat archiving, versioning protocols, and changelog updates.
- Updated [`README.md`](/README.md) with quick start instructions and Changelog.
- Compiled project with GCC MinGW into [`build/main.exe`](/build/main.exe).

### Final Response Delivered
Delivered full project overview, guided learning documentation, file hierarchy, and working implementation for all Phase 1 milestones with `make win` support.

---

---

## Turn v1.1 - Fix Makefile for MSYS2 Sh Environment - v1.1

- **Session Version**: v1.1
- **Date**: 2026-10-18
- **Context**: Resolving `/bin/sh` syntax error when executing `make win` on Windows via MSYS2 make.

---

### User Input
```text
PS D:\GitHub\UITS\CGMLabProject_2DGame> make win
/bin/sh: -c: line 2: syntax error: unexpected end of file from `if' command on line 1
make: *** [Makefile:4: win] Error 2
```

### High-Level Architectural Rationale & Design Decisions
> 1. Diagnosed the error: `make` invokes `/bin/sh` from MSYS2 (`C:\msys64\usr\bin\make.exe`), which cannot evaluate Windows batch `if not exist` commands.
> 2. Removed shell-dependent batch conditions from [`Makefile`](/Makefile), aligning it directly with the standard clean format used across the lab repository.
> 3. Updated [`.gitignore`](/.gitignore) to track [`build/.gitkeep`](/build/.gitkeep) and [`build/glfw3.dll`](/build/glfw3.dll) while ignoring compiled `.exe` and `.o` files. This ensures the output folder and runtime DLL always exist without requiring runtime shell scripts.

### Actions & Tools Executed
- Updated [`Makefile`](/Makefile) to cleanly compile and run without shell script discrepancies.
- Modified [`.gitignore`](/.gitignore) to keep `build/glfw3.dll` and `build/.gitkeep` tracked.
- Created [`build/.gitkeep`](/build/.gitkeep).
- Verified `make -n win` runs without syntax errors.

### Final Response Delivered
Explained why the error occurred (MSYS2 `make` uses `/bin/sh` rather than `cmd.exe`) and provided the resolved [`Makefile`](/Makefile).

---

---

## Turn v1.2 - Convert File Paths to Root-Relative - v1.2

- **Session Version**: v1.2
- **Date**: 2026-10-21
- **Context**: Migrating all file references and markdown links to repository root-relative paths.

---

### User Input
```text
make all the file path references absolute path relative to the root
```

### High-Level Architectural Rationale & Design Decisions
> 1. Machine-specific absolute URIs (e.g., `file:///d:/...`) break portability across different developer machines, operating systems, and remote Git hosting platforms (such as GitHub and GitLab).
> 2. Root-relative paths (starting with `/`, such as `/doc/LEARNING_GUIDE.md`) resolve consistently from the repository root across GitHub web navigation, IDE previews, and docs renderers.
> 3. Standardized all internal markdown file links across `README.md`, `AGENTS.md`, and past prompt logs to root-relative paths, and formalized this rule in `AGENTS.md` to ensure ongoing adherence.

### Actions & Tools Executed
- Updated [`/README.md`](/README.md) documentation links to use root-relative paths.
- Updated [`/AGENTS.md`](/AGENTS.md) internal links and added rule requiring root-relative paths.
- Migrated legacy `file:///` links across [`/doc/prompts/v1.0 - Project Analysis and Phase 1 Implementation.md`](/doc/prompts/v1.0%20-%20Project%20Analysis%20and%20Phase%201%20Implementation.md) and [`/doc/prompts/v1.1 - Fix Makefile for MSYS2 Sh Environment.md`](/doc/prompts/v1.1%20-%20Fix%20Makefile%20for%20MSYS2%20Sh%20Environment.md).
- Updated prompt reference files [`/doc/prompts/b1tranger.txt`](/doc/prompts/b1tranger.txt) and [`/doc/prompts/lima.txt`](/doc/prompts/lima.txt).

### Final Response Delivered
Delivered verification summary and list of modified files using clean root-relative links.

---

---

