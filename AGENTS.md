# Agent Guidelines & Workflow Rules

These instructions define the mandatory operational protocols, archiving procedures, and documentation standards for AI agents working in this repository.

---

## 1. Chat & Conversation Archiving Protocol

### Target Archive Directory & Serial Session Folders
- All session archives reside inside [`doc/prompts/`](/doc/prompts/) in dedicated serial folders named with an incrementing major integer prefix:
  `doc/prompts/<N>. <Session Title>/` (e.g., `doc/prompts/6. Boss Battle Polish/`).
- **Determining `<N>`:** Inspect `doc/prompts/` to find the highest existing major integer prefix across existing session folders (`<N>. .../`), legacy `vX.Y` files (e.g. `v5.0` -> `5`), and legacy flat files, and assign `N = N_max + 1` for a brand-new conversation session.
- **Session Continuity:** Always update **the same session folder** (`doc/prompts/<N>. <Session Title>/`) on each turn or when resuming an existing conversation; do NOT create a new major folder per turn.

### Required Files Inside `doc/prompts/<N>. <Session Title>/`
1. **`transcript_full.jsonl`**
   - An exact, unmodified UTF-8 copy of `<appDataDir>/brain/<conversation-id>/.system_generated/logs/transcript_full.jsonl`.
   - Never truncate, edit, or reformat lines in `transcript_full.jsonl`.
2. **`conversation.md`**
   - A verbatim Markdown rendering generated directly from `transcript_full.jsonl`, including **every step exactly** without summarizing, paraphrasing, or omitting content:
     - Conversation metadata (`Conversation ID`, `Session Folder`, and `Continued From` if applicable).
     - All user messages (`USER_INPUT.content`).
     - All agent thoughts / thinking (`PLANNER_RESPONSE.thinking`) verbatim.
     - All tool calls (`PLANNER_RESPONSE.tool_calls`) and tool/system outputs (`GENERIC` / `SYSTEM_MESSAGE`).
     - All agent responses (`PLANNER_RESPONSE.content`) verbatim.
3. **`summary.md` (Conversation Summary Markdown)**
   - Maintained alongside `transcript_full.jsonl` and `conversation.md` in the same folder.
   - Summarizes session metadata (`Conversation ID`, date, context), user goals, engineering/physics analysis, files created/modified (using root-relative links such as `[/README.md](/README.md)`), verification steps, and outcomes.
4. **Companion Plans & Walkthroughs (Fractional Suffixes)**
   - Stored inside the same `doc/prompts/<N>. <Session Title>/` folder:
     - Implementation plan: `<N>.1 Implementation Plan - <Plan Title>.md`
     - Walkthrough document: `<N>.2 Walkthrough - <Title>.md`
     - Subsequent plans/walkthroughs in the same session: `<N>.3 ...`, `<N>.4 ...`, etc.

---

## 2. Update Cadence (Keeping the Archive Current)

Because the runtime writes a turn's final `PLANNER_RESPONSE` (including its final `thinking` and `content`) to `transcript_full.jsonl` after the turn finishes:

1. **At the start of each turn (or when continuing from a prior `<conversation-id>`):**
   - Copy `<appDataDir>/brain/<conversation-id>/.system_generated/logs/transcript_full.jsonl` to `doc/prompts/<N>. <Session Title>/transcript_full.jsonl` and regenerate `conversation.md` so the previous turn's finalized `thinking` and `content` are captured verbatim.
   - If continuing from a prior conversation ID, ensure its archive folder is also synced and finalized.
2. **Before sending each new final response:**
   - Re-copy `transcript_full.jsonl` and regenerate `conversation.md` to capture all completed steps of the current turn (new user prompt, tool calls, intermediate `thinking`, and tool/system outputs).
   - Append the exact final response text being sent under `## Pending Final Response (Pre-Commit)` at the bottom of `conversation.md` (with a note that it will be replaced verbatim by the runtime-committed `thinking` + `content` from `transcript_full.jsonl` on the next sync).
   - Create or update `summary.md` in the same folder with the latest session summary.


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
