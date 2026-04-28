# Prompt 24: Hex Prism and Cube Geometry

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
Implement real 3D bookmark object geometry.

Requirements:
1. Add geometry generators for:
   - cube
   - hexagonal prism
   - flat beveled tile or panel
2. Each object must support:
   - position
   - rotation
   - scale
   - material color
   - hover/selected state later
3. Hex prism should be the primary visual identity.
4. Cube should be available as alternate Node shape.
5. Add debug scene showing several objects.

Acceptance criteria:
- Hex prisms render correctly.
- Cubes render correctly.
- Objects can rotate independently.
- Geometry code is clean and reusable.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.
