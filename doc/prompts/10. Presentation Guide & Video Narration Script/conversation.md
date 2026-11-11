# Conversation Archive: Session 10 - Presentation Guide & Video Narration Script

- **Conversation ID**: `1a5f68b2-0077-4176-a249-974d0fc4db8`
- **Session Folder**: [`/doc/prompts/10. Presentation Guide & Video Narration Script/`](/doc/prompts/10.%20Presentation%20Guide%20%26%20Video%20Narration%20Script/)
- **Archive Note**: Raw `transcript_full.jsonl` was not accessible inside the sandbox during Turn 1 (`Cannot find path` / `Permission denied`). Verbatim user input and pre-commit final response are recorded below.

---

## Step 1 - USER_INPUT (USER_EXPLICIT)

````text
make a presentation guide as markdown based on the documentation @[d:\GitHub\UITS\CGMLabProject_2DGame\doc] so I can demonstrate the project. We also need to submit a screen recording of the project beforehand. So prepare a small script that I will read out before the actual gameplay video. The goal is to explain the game details (type / genre), the core philosphy of why we made it, and core gameplay mechanics. I wish to add visuals (parts og the actual gameplay while I read out the script) so suggest what I should add as well.
````

---

## Pending Final Response (Pre-Commit)
> Note: This pre-commit snapshot will be replaced verbatim by the runtime-committed `thinking` + `content` from `transcript_full.jsonl` on the next sync.

I created [PRESENTATION_GUIDE.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/PRESENTATION_GUIDE.md) based on your project documentation and synchronized the documentation links and session archive.

### What Was Added
1. **Presentation Guide ([PRESENTATION_GUIDE.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/PRESENTATION_GUIDE.md))**:
   - **Pre-Gameplay Video Narration Script (~75 seconds)**: A ready-to-read voiceover script covering the game's type/genre (2D multi-directional endless runner & combat platformer in C++17 / Modern OpenGL 3.3), the core philosophy of *"wending"* (adaptability vs. static single-axis runners), and core mechanics (4-tier traversal, `LadderScene` & `SkyFallScene` with 15s survival + Open Exit Gates, Katana slash & `RMB`/`K` parry, 10s circular energy shield, and 3-phase enemy spawning + Demon Slime Boss).
   - **Visual B-Roll Storyboard & Shot-List Checklist**: Paired line-by-line with the script, detailing exact gameplay clips and keystrokes (`B`, `N`, `F`, `Shift`, `LMB`/`RMB`, Up/Down relics) to show while reading each part.
   - **Live Demo Playbook (5–7 minutes)**: Step-by-step demonstration order from the procedural academic splash screen and Media Foundation intro video through traversal, mini-games, combat/parry, and Pause Menu / `F11` fullscreen.
   - **Technical Highlights, Controls Cheat Sheet & Rapid Viva Q&A**: Condensed talking points covering the single reusable quad + MVP/UV pipeline, circular shield fragment discard, constant-memory chunk streaming ($+35\,\text{m}$ ahead / $-85\,\text{m}$ prune), and hybrid `WinMM` + `MCI` audio.
2. **Documentation & Archive Updates**:
   - Linked the new guide in [README.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/README.md) and added `v0.8.1` to [changelog.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/changelog.md).
   - Created Session 10 archive files ([conversation.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/prompts/10.%20Presentation%20Guide%20&%20Video%20Narration%20Script/conversation.md) and [summary.md](file:///d:/GitHub/UITS/CGMLabProject_2DGame/doc/prompts/10.%20Presentation%20Guide%20&%20Video%20Narration%20Script/summary.md)); `transcript_full.jsonl` could not be copied from the sandbox during Turn 1.
