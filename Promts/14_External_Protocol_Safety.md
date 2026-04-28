# Prompt 14: External Protocol Safety

You are the AI coding agent for CyberDeck Browser.

Project goal:
Build a native Windows 11 desktop web browser in C++20 with a retro-futuristic 1980s terminal/cyberpunk aesthetic and a real OpenGL 3D bookmark system called Deck Space. The web engine must be reused from Chromium Embedded Framework (CEF). Do not build a browser engine.

Core stack:
- Windows 11 target
- C++20
- CMake
- Visual Studio 2022 / MSVC
- Chromium Embedded Framework (CEF)
- Native Win32 windowing preferred
- OpenGL 4.x for the 3D Deck Space
- nlohmann/json or equivalent permissive JSON library
- Inno Setup or NSIS for installer later

Product identity:
- App name: CyberDeck Browser
- Bookmark system name: Deck Space
- Bookmarks are called Nodes
- Add bookmark action: Add Node
- Main visual style: black background, neon green text, yellow links/labels, red warnings/actions, monospace UI, CRT/scanline/glow effects where appropriate

Hard rules:
1. Build incrementally. Do not rewrite working code unless the current prompt explicitly requires it.
2. Keep changes modular and easy to review.
3. Do not fake success. If you did not build or run a check, say exactly that.
4. At the end of every stage, output:
   - files added
   - files changed
   - exact build commands
   - exact run commands
   - verification performed
   - known issues
5. Do not copy large unknown code from random repositories. Use official samples, documented APIs, and permissively licensed libraries only.
6. Keep third-party license attribution.
7. Never expose unsafe browser capabilities by default.
8. Prefer a working, stable v1 over experimental architecture.
9. The v1 browser should render normal websites through CEF child browser views. Do not implement live websites as OpenGL textures in v1 unless explicitly requested in a later prompt.
10. OpenGL is for the app shell identity and Deck Space, not for replacing Chromium rendering.


Task:
Prevent unsafe or confusing external protocol launches.

Requirements:
1. Intercept navigation or protocol attempts for:
   - mailto:
   - tel:
   - file:
   - javascript:
   - data:
   - custom external protocols
2. Safe default:
   - block `javascript:` entered in URL bar
   - block or confirm external app launch protocols
   - block local file browsing unless explicitly enabled later
3. Add a small warning dialog styled in CyberDeck colors:
   - green normal text
   - yellow explanation
   - red cancel/danger action
4. Log all blocked/confirmed protocol attempts.

Acceptance criteria:
- Dangerous protocols do not execute silently.
- Normal https/http browsing still works.
- User can cancel external app launch.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.
