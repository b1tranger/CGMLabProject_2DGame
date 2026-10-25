# Agent Guidelines & Workflow Rules

These instructions define the mandatory operational protocols, archiving procedures, and documentation standards for AI agents working in this repository.

---

## 1. Chat & Conversation Archiving Protocol

### Target Archive Directory
All session archives must be saved in:
[`doc/prompts/`](/doc/prompts/)

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

### AI Thinking & Planning Summary
> [Comprehensive summary of the agent's analytical thought process, requirement breakdown, architectural rationale, formulas, technical trade-offs, and step-by-step implementation strategy leading directly to the solution.]

### Actions & Tools Executed
- [Tool action summary, e.g., Inspected files, compiled binaries, created components]
- [List of modified or created files with clickable markdown links]

### Final Response Delivered
[Full final response delivered to the user, including code snippets, guides, and tables]

---
```

#### Guidelines for Archiving Thought Process:
The **AI Thinking & Planning Summary** must record the core analytical thought process and reasoning of the agent:
- Decomposition of user requirements and edge-case identification.
- Mathematical models, coordinate transformations, and physics formulas applied.
- Structural decisions, component interfaces, and API integrations.
- Direct problem-solving steps and rationale leading to the final delivered changes.


---

## 3. Documentation & Changelog Update Protocol

Whenever changes, features, refactoring, or phase milestones are implemented:

1. **Update Relevant Documentation in [`doc/`](/doc/)**:
   - Keep [`doc/LEARNING_GUIDE.md`](/doc/LEARNING_GUIDE.md) synchronized with any architectural changes, new shader uniforms, controls, or data structures.
   - Explain all concepts in simple, accessible language for learning purposes.

2. **Update [`README.md`](/README.md)**:
   - Ensure Quick Start, controls, and build instructions reflect the latest state of the codebase.

3. **Maintain a Version History (Changelog)**:
   - Maintain a clear **Version History / Changelog** in [`README.md`](/README.md) (and/or [`doc/CHANGELOG.md`](/doc/CHANGELOG.md)).
   - Record the release version (e.g., `v0.1.0 - Phase 1 Complete`), date, summary of changes, new mechanics introduced, and resolved issues.

4. **Root-Relative File Path References**:
   - All internal repository file references and links in documentation, prompt archives, and markdown files MUST use absolute paths relative to the repository root (e.g., `[/README.md](/README.md)` or `[/doc/LEARNING_GUIDE.md](/doc/LEARNING_GUIDE.md)`).
   - NEVER use machine-specific absolute file URIs (such as `file:///...`).
