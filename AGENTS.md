# Agent Guidelines & Workflow Rules

These instructions define the mandatory operational protocols, archiving procedures, and documentation standards for AI agents working in this repository.

---

## 1. Chat & Conversation Archiving Protocol

### Target Archive Directory
All session archives must be saved in:
[`doc/prompts/`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/prompts)

### Versioning & Sequential Serial Convention
Every conversation archive file in `doc/prompts/` must follow a structured versioning scheme:
- **Major Version (`vX.0`)**: Every **new chat session** initiates a new major version family (e.g., `v1.0 - <Topic>.md`, `v2.0 - <Topic>.md`).
- **Minor Version (`vX.Y`)**: Subsequent turns, Implementation Plans, walkthroughs, or follow-up iterations within the **same chat session** must use an incremented minor version (e.g., `v1.1 - <Milestone>.md`, `v1.2 - <Walkthrough>.md`).

#### Determining the Next Version:
1. Inspect `doc/prompts/` to find the highest existing major and minor versions.
2. For a new chat: assign `v(X+1).0 - <Descriptive Title>.md`.
3. For continued work within an existing chat: assign `vX.(Y+1) - <Descriptive Title>.md`.

---

## 2. Archive File Structure & Format

Each archive file in `doc/prompts/` must adhere to the following Markdown format:

```markdown
# Session Archive: [Descriptive Title]

- **Session Version**: vX.Y
- **Date**: YYYY-MM-DD
- **Context**: [Brief context / scope of the turn]

---

### User Input
[Exact user prompt or request verbatim]

### High-Level Architectural Rationale & Design Decisions
> [High-level summary of the architectural reasoning, technical considerations, formulas, and direct design decisions leading to the implementation.]

### Actions & Tools Executed
- [Tool action summary, e.g., Inspected files, compiled binaries, created components]
- [List of modified or created files with clickable markdown links]

### Final Response Delivered
[Full final response delivered to the user, including code snippets, guides, and tables]

---
```

---

## 3. Documentation & Changelog Update Protocol

Whenever changes, features, refactoring, or phase milestones are implemented:

1. **Update Relevant Documentation in [`doc/`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc)**:
   - Keep [`doc/LEARNING_GUIDE.md`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/LEARNING_GUIDE.md) synchronized with any architectural changes, new shader uniforms, controls, or data structures.
   - Explain all concepts in simple, accessible language for learning purposes.

2. **Update [`README.md`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/README.md)**:
   - Ensure Quick Start, controls, and build instructions reflect the latest state of the codebase.

3. **Maintain a Version History (Changelog)**:
   - Maintain a clear **Version History / Changelog** in [`README.md`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/README.md) (and/or [`doc/CHANGELOG.md`](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/CHANGELOG.md)).
   - Record the release version (e.g., `v0.1.0 - Phase 1 Complete`), date, summary of changes, new mechanics introduced, and resolved issues.
