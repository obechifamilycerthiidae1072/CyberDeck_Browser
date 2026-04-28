# CyberDeck Browser: All Prompts Combined

Use the staged files instead when possible. This combined file is for reference or bulk import.



---


# Prompt 00: Global Rules, Product Vision, and Non-Negotiables

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


Use this prompt before every coding stage. Treat it as the standing constitution for the project.

Product summary:
CyberDeck Browser is not meant to be a generic Chrome clone. It is a native C++ Windows 11 Chromium-powered browser with a unique real-time OpenGL 3D bookmark world. It should feel like an 80s hacker terminal crossed with a cyberpunk arcade control deck.

V1 scope:
- Native Windows executable.
- CEF-based browsing.
- Basic tabs.
- URL bar.
- Back/forward/reload/stop.
- Local history.
- Local bookmarks stored as JSON Nodes.
- Deck Space OpenGL scene showing bookmarks as real 3D hex prisms/cubes/tiles.
- Terminal Mode CSS injection toggle.
- Retro terminal/cyberpunk UI.
- Installer packaging.
- Clear README and license notices.

V1 non-scope:
- Building a web engine.
- Browser extensions.
- Cloud sync.
- Account system.
- Password manager.
- Ad blocker.
- Live websites mapped onto 3D objects.
- Mobile support.
- Cross-platform support.
- AI features.
- Proprietary assets.

Commercial-readiness rules:
- Keep licenses clean.
- Avoid GPL-only dependencies unless explicitly approved.
- Use permissive libraries where practical: BSD, MIT, Apache-2.0, zlib/libpng.
- Document third-party components.
- Store user data under the correct Windows app data directory.
- Handle corrupted local JSON files safely.
- Do not crash on missing GPU features; provide fallback messages.

Every stage must end with:
- files added
- files changed
- build command
- run command
- what was tested
- what still does not work

Do not claim the app is production-ready unless the installer, fresh install, launch, browsing, tabs, Deck Space, bookmarks, and Terminal Mode were tested.



---


# Prompt 01: Architecture Decision Record

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
Create the initial architecture documentation for CyberDeck Browser before writing core code.

Requirements:
1. Add a `/docs` directory.
2. Create `/docs/ARCHITECTURE.md`.
3. Explain why the chosen stack is:
   - C++20
   - CMake
   - CEF for Chromium browsing
   - Native Win32 windowing
   - OpenGL 4.x for Deck Space
   - JSON for v1 persistence
4. Explain why Electron and Qt WebEngine are not used for this version.
5. Explain the major subsystems:
   - App lifecycle
   - CEF integration
   - Browser tab manager
   - Toolbar/navigation UI
   - Terminal Mode injection
   - Bookmark Node storage
   - OpenGL Deck Space renderer
   - Packaging
6. Include a simple ASCII module diagram.
7. Include v1 non-goals.

Acceptance criteria:
- Architecture doc is clear enough for another coder to continue the project.
- It explicitly says CEF renders normal web pages and OpenGL renders Deck Space.
- It warns that live websites as 3D textures are a future v2 feature, not v1.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 02: License and Third Party Reuse Plan

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
Create the licensing and third-party dependency plan.

Requirements:
1. Add `/docs/LICENSING_PLAN.md`.
2. Add root `LICENSE` placeholder using a permissive project license chosen for this app, preferably MIT unless there is a reason not to.
3. Add `THIRD_PARTY_NOTICES.md`.
4. List planned dependencies:
   - CEF / Chromium Embedded Framework
   - CMake
   - nlohmann/json or equivalent
   - OpenGL loader if used, such as GLAD
   - Optional stb_image for image loading
   - Optional Inno Setup / NSIS for installer
5. For each dependency, include:
   - purpose
   - expected license
   - official source URL placeholder
   - whether it is bundled or installed externally
6. Add a rule: do not copy random CSS/3D code from the internet unless license is verified.
7. Add a rule: any reused code must have attribution if required.

Acceptance criteria:
- The project has license files before major code starts.
- The plan avoids unclear licensing.
- The plan is suitable for a product that may be sold commercially.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 03: CEF Project Scaffold

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
Scaffold a minimal C++20 CEF project based on the official CEF project pattern.

Requirements:
1. Create a clean CMake-based project structure:
   - `/CMakeLists.txt`
   - `/src/main`
   - `/src/browser`
   - `/src/app`
   - `/src/common`
   - `/assets`
   - `/docs`
   - `/third_party`
   - `/scripts`
2. Add a minimal Windows executable target named `CyberDeckBrowser`.
3. Integrate CEF in the cleanest practical way:
   - either expect `CEF_ROOT` to be provided
   - or add a script/documented process to download/extract CEF
4. Do not commit massive CEF binary files directly unless the project policy says so.
5. Add a minimal app entry point that starts and exits cleanly.
6. Add `README.md` section for configuring CEF.

Acceptance criteria:
- CMake configure succeeds when CEF is available.
- The app builds to a Windows executable.
- The app can launch and close without opening a browser yet.
- The folder structure is ready for later stages.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 04: Native Windows App Window

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
Create the native Windows 11 main application window.

Requirements:
1. Implement a Win32 main window with:
   - app title: CyberDeck Browser
   - minimum size around 1100x700
   - dark background
   - proper WM_CLOSE/WM_DESTROY handling
2. Keep the code separated:
   - app lifecycle in `/src/app`
   - main window in `/src/main` or `/src/ui`
3. Add basic logging to a file under a local dev/logs directory for now.
4. Add a simple placeholder client area:
   - black background
   - green text saying `CYBERDECK BOOT SEQUENCE READY`
5. Prepare the window layout for:
   - top toolbar
   - tab strip
   - browser content area
   - future Deck Space overlay

Acceptance criteria:
- App opens a native Windows window.
- App closes cleanly.
- No CEF browser view is required yet.
- Resize works without obvious flicker/crash.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 05: Embed First CEF Browser View

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
Embed the first real CEF Chromium browser view inside the main window.

Requirements:
1. Initialize CEF correctly during startup.
2. Create a single CEF browser child view inside the content area.
3. Load a default homepage:
   - use `https://www.example.com` for first test
   - later this can become a local cyberdeck start page
4. Resize the CEF child browser when the main window resizes.
5. Shut down CEF cleanly on app exit.
6. Log page load start/end/fail events.

Acceptance criteria:
- App opens and displays a real webpage rendered by CEF.
- HTTPS site loads.
- Resizing keeps the browser view inside the content area.
- App closes without hanging CEF processes.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 06: Toolbar V1 Navigation Controls

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
Add the first usable browser toolbar.

Requirements:
1. Add a top toolbar above the web content.
2. Toolbar must include:
   - Back button
   - Forward button
   - Reload button
   - Stop button if loading
   - URL edit box
   - Go button or Enter-to-go behavior
3. Style:
   - black background
   - green borders/text
   - yellow hover/accent
   - red for stop/danger
   - monospace font
4. Buttons should call the CEF browser navigation APIs.
5. URL edit box should navigate the active browser when Enter is pressed.

Acceptance criteria:
- User can type a full URL and navigate.
- Back/forward/reload work when available.
- Buttons do not crash when no page is loaded.
- Toolbar layout survives window resize.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 07: URL Normalization and Search Fallback

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
Make the URL bar feel like a real browser address bar.

Requirements:
1. Implement URL normalization:
   - `example.com` -> `https://example.com`
   - `http://example.com` remains as entered
   - `https://example.com` remains as entered
   - local file paths should not be enabled by default unless explicitly handled
2. Add optional search fallback:
   - If input contains spaces and is not a URL, navigate to a configurable search engine URL.
   - Default search engine can be DuckDuckGo or a configurable placeholder.
3. Add validation and basic error logging.
4. Do not allow dangerous protocols silently:
   - confirm or block `file:`, `javascript:`, `data:`, custom protocols for now
5. Add unit-testable helper functions where practical.

Acceptance criteria:
- Typing `youtube.com` loads `https://youtube.com`.
- Typing `hello world` opens the configured search URL.
- Dangerous schemes do not execute silently from the URL bar.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 08: Basic Tab Manager

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
Implement real multi-tab support.

Requirements:
1. Add a tab strip above or integrated with the toolbar.
2. Each tab should own or reference its own CEF browser instance.
3. Features:
   - New tab button
   - Close tab button
   - Switch tab by clicking
   - At least one tab must remain open
4. Inactive tab browser views should be hidden or detached cleanly.
5. Active tab should update the URL bar and title.
6. Keep tab state in a clear model:
   - id
   - title
   - url
   - loading state
   - canGoBack
   - canGoForward

Acceptance criteria:
- User can open at least 5 tabs.
- Switching tabs shows the correct page.
- Closing active/inactive tabs works.
- The last tab does not leave the app in a broken state.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 09: Cyberpunk Tab UI Styling

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
Polish the tab strip into a CyberDeck-style UI.

Requirements:
1. Style active tab:
   - neon green outline
   - subtle glow
   - title in bright green or yellow
2. Style inactive tabs:
   - dark background
   - dim green text
3. Close button:
   - red accent
   - clear hover state
4. New tab button:
   - yellow or green accent
5. Add truncation for long titles.
6. Add loading indicator:
   - simple text spinner, progress pulse, or small animated bar
7. Keep performance lightweight.

Acceptance criteria:
- Tabs look visually consistent with 80s terminal/cyberpunk style.
- Long titles do not break layout.
- Active tab is obvious.
- UI remains usable at 1100px width.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 10: Navigation State Synchronization

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
Synchronize browser events with the app UI.

Requirements:
1. Update URL bar when:
   - navigation starts
   - navigation commits
   - navigation completes
   - user switches tabs
2. Update tab title from page title.
3. Update back/forward button enabled state.
4. Update reload/stop state based on loading state.
5. Log load failures with error code and failed URL.
6. Prevent UI event loops:
   - do not overwrite user typing in URL bar while the URL edit has focus unless navigation commits.

Acceptance criteria:
- URL bar accurately reflects the active tab.
- Tab titles update after page load.
- Back/forward disabled/enabled states are correct.
- Switching tabs updates toolbar state.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 11: History V1 JSON Persistence

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
Add simple local browsing history.

Requirements:
1. Store history under `%APPDATA%/CyberDeckBrowser/history.json`.
2. Data fields:
   - id
   - title
   - url
   - visitCount
   - firstVisitedUtc
   - lastVisitedUtc
3. Record successful top-level navigations only.
4. Use atomic JSON writes:
   - write temp file
   - flush
   - rename/replace
5. On corrupted JSON:
   - rename corrupted file with timestamp
   - start a clean history file
   - log the event
6. Add a minimal internal history view later-ready, but a simple debug command/log is acceptable in this stage.

Acceptance criteria:
- Visiting pages creates/updates history entries.
- Restarting app preserves history.
- Corrupt history file does not crash the browser.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 12: Downloads V1

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
Handle file downloads safely.

Requirements:
1. Implement CEF download handler.
2. Default download folder:
   - Windows Downloads folder if easily available
   - otherwise `%USERPROFILE%/Downloads`
3. Show a simple CyberDeck-styled download status panel or status line.
4. Ask before overwriting files.
5. Log:
   - started downloads
   - completed downloads
   - failed/canceled downloads
6. Do not auto-run downloaded files.

Acceptance criteria:
- Downloading a small test file works.
- User can see that a download started and completed.
- Failed downloads do not crash the browser.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 13: Permissions V1

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
Add safe browser permission behavior.

Requirements:
1. Handle permission requests from CEF:
   - camera
   - microphone
   - geolocation
   - notifications
   - MIDI or other special permissions if exposed
2. Default behavior:
   - deny silently for dangerous permissions until UI is implemented
   - or show a simple explicit allow/deny prompt
3. Add setting hooks for future per-site permissions.
4. Log permission requests.
5. Never auto-allow camera/mic/location.

Acceptance criteria:
- Sites cannot silently get camera/mic/location.
- Permission request does not crash or hang the app.
- User-facing behavior is clear.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


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



---


# Prompt 15: Custom Retro Error Pages

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
Create custom CyberDeck error pages for failed navigation.

Requirements:
1. When a main-frame load fails, show a local error page instead of a blank/ugly error.
2. Error page style:
   - black background
   - green monospace text
   - red error code
   - yellow URL
   - optional ASCII boot/failure message
3. Include buttons:
   - Retry
   - Back
   - Copy URL
4. Show:
   - failed URL
   - CEF error code/name
   - short friendly explanation
5. Keep local error page implementation safe and not dependent on internet.

Acceptance criteria:
- Invalid domain shows custom error page.
- Retry button tries again.
- Back button works if history exists.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 16: Global Browser Shell Theme

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
Implement the base 80s terminal/cyberpunk visual theme for the whole native shell.

Requirements:
1. Create centralized theme constants:
   - black: #000000
   - green: #00ff00
   - yellow: #ffff00
   - red: #ff0000
   - dim green
   - dark panel background
2. Apply theme to:
   - main window background
   - toolbar
   - tab strip
   - buttons
   - URL bar
   - status panels
   - dialogs
3. Use monospace fonts where practical:
   - Consolas
   - Cascadia Mono
   - Courier New fallback
4. Add subtle neon borders and hover states.
5. Avoid unreadable tiny text.
6. Keep UI fast and native.

Acceptance criteria:
- The browser shell has a consistent identity.
- It looks old-school and futuristic at the same time.
- It does not look like a default Windows Forms/MFC/Qt app.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 17: Terminal Mode CSS Injection

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
Add Terminal Mode that restyles loaded websites.

Requirements:
1. Add a visible Terminal Mode toggle in the toolbar.
2. When enabled, inject CSS into every loaded main frame:
   - background #000000
   - text #00ff00
   - links #ffff00
   - strong/bold/emphasis #ff0000
   - monospace font
3. Add optional CSS for:
   - text glow
   - border highlights
   - image dimming toggle if desired
4. When disabled, stop injecting new CSS and try to remove the injected style tag from current pages.
5. Reapply after navigation completes.
6. Store state in settings later, or temporary state in this stage.
7. Use CEF-safe JavaScript injection. Do not expose native objects to remote pages.

Acceptance criteria:
- Toggle on changes website appearance after load.
- Toggle off restores or stops future injection.
- New pages get Terminal Mode if enabled.
- Browser does not crash on CSP-heavy websites; if injection fails, log it.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 18: CRT Scanline and Glow Effects

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
Add optional CRT-style visual effects to the browser shell and Deck Space.

Requirements:
1. Add settings/toggles for:
   - scanlines
   - glow
   - flicker intensity, default off or very subtle
2. For shell UI, implement effects without making text unreadable.
3. For Deck Space, implement shader-based or overlay-based scanlines/glow if practical.
4. Do not apply heavy effects to normal web pages unless Terminal Mode is enabled and user wants it.
5. Make effects easy to disable.

Acceptance criteria:
- Effects look cool but do not destroy usability.
- Effects can be toggled.
- Performance remains smooth.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 19: Settings Persistence

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
Persist user settings.

Requirements:
1. Store settings under `%APPDATA%/CyberDeckBrowser/settings.json`.
2. Settings fields:
   - terminalModeEnabled
   - scanlinesEnabled
   - glowEnabled
   - flickerIntensity
   - deckLayoutMode
   - homepage
   - searchEngineUrl
3. Use atomic JSON writes.
4. Handle corrupted settings safely.
5. Load settings at startup before creating initial UI where practical.
6. Add a small debug/settings panel or simple internal page to inspect current settings.

Acceptance criteria:
- Terminal Mode setting survives restart.
- Scanline/glow settings survive restart.
- Corrupted settings file does not crash app.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 20: OpenGL Deck Renderer Setup

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
Add the OpenGL renderer used for Deck Space.

Requirements:
1. Use native Win32 + WGL or another clean Windows-compatible OpenGL setup.
2. Prefer OpenGL 4.x core profile if practical.
3. Keep OpenGL code separate:
   - `/src/deck`
   - `/src/render`
4. Add a Deck Space mode:
   - hides or covers the CEF browser area
   - shows an OpenGL-rendered scene
   - can exit back to browser mode
5. Initial scene:
   - black background
   - green grid floor
   - rotating test cube or hex prism
   - basic camera
6. Log OpenGL vendor, renderer, version.

Acceptance criteria:
- User can enter Deck Space and see real OpenGL graphics.
- User can exit back to browsing.
- OpenGL info is logged.
- App does not crash if OpenGL initialization fails; show clear error.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 21: High Performance GPU Preference

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
Prefer NVIDIA/high-performance GPU on hybrid Windows laptops while keeping compatibility.

Requirements:
1. Add exported symbols commonly used to request high-performance GPUs:
   - NvOptimusEnablement
   - AmdPowerXpressRequestHighPerformance
2. Log active OpenGL renderer string.
3. Do not hard-code NVIDIA-only behavior.
4. App must still run on Intel/AMD GPUs if OpenGL requirements are met.
5. Add README note:
   - Windows Graphics Settings can force high performance GPU manually
   - NVIDIA Control Panel can force app to use NVIDIA GPU

Acceptance criteria:
- On NVIDIA laptop, renderer log should help confirm whether NVIDIA GPU is used.
- App remains portable to non-NVIDIA systems.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 22: Deck Camera and Input Controls

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
Add camera and input controls for Deck Space.

Requirements:
1. Mouse:
   - drag to rotate/orbit
   - wheel to zoom
   - left click reserved for selection/picking later
2. Keyboard:
   - Esc exits Deck Space
   - arrow keys rotate/select
   - Enter opens selected node later
   - WASD optional for camera movement
3. Smooth camera:
   - damped rotation
   - zoom limits
4. Add on-screen help overlay:
   - green/yellow terminal text
   - can be hidden later

Acceptance criteria:
- User can move around the 3D scene intuitively.
- Camera cannot zoom into broken/inverted state.
- Esc returns to browser.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 23: Neon Material and Shader System

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
Create the CyberDeck neon material system.

Requirements:
1. Implement shader/material support for:
   - neon green
   - yellow highlight
   - red danger/action
   - dim inactive green
2. Add emissive/glow-like look:
   - simple bloom optional if easy
   - otherwise use bright materials and halo geometry
3. Add black void background.
4. Add optional grid floor with fading lines.
5. Add basic text label approach:
   - bitmap font, texture atlas, or simple 2D overlay
   - must support bookmark titles later
6. Keep performance smooth with dozens of nodes.

Acceptance criteria:
- Deck Space looks cyberpunk/retro, not like default OpenGL demo.
- Materials are centralized and reusable.
- The scene supports labels or has a clear plan for labels.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


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



---


# Prompt 25: Animation System for Deck Nodes

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
Add lightweight animation for Deck Space objects.

Requirements:
1. Support:
   - idle rotation
   - hover pulse
   - selected glow pulse
   - carousel/orbit motion
   - smooth transitions between layouts later
2. Use delta-time based animation.
3. Avoid hardcoded frame-rate assumptions.
4. Add simple animation parameters to each renderable Node.
5. Keep math readable.

Acceptance criteria:
- Objects animate smoothly.
- Idle scene feels alive but not chaotic.
- Animation can be disabled or reduced later if needed.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 26: Bookmark Node Data Model

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
Define the Bookmark Node model.

Requirements:
1. Create a model for Deck Space Nodes:
   - id
   - title
   - url
   - faviconPath optional
   - shapeType: hex, cube, panel
   - colorTheme: green, yellow, red, mixed
   - createdUtc
   - updatedUtc
   - lastVisitedUtc optional
   - visitCount
   - deckPosition optional
   - tags optional
2. Keep model serializable to JSON.
3. Add validation:
   - title not empty
   - URL normalized/safe
   - valid shape type
4. Add helper functions to create a Node from the active tab.

Acceptance criteria:
- Node model is clear and future-proof.
- JSON representation is stable and documented.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 27: Bookmark JSON Storage

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
Implement persistent Bookmark Node storage.

Requirements:
1. Store bookmarks under `%APPDATA%/CyberDeckBrowser/bookmarks.json`.
2. Use atomic writes.
3. On corrupted JSON:
   - rename file with timestamp
   - create new empty bookmark file
   - log warning
4. Add import/export-ready structure:
   - version field
   - nodes array
5. Add functions:
   - loadBookmarks
   - saveBookmarks
   - addBookmark
   - updateBookmark
   - deleteBookmark
   - findBookmarkById
6. Avoid data loss on app crash.

Acceptance criteria:
- Bookmarks survive restart.
- Adding/updating/deleting persists to disk.
- Corrupted file does not crash app.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 28: Add Node from Current Tab

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
Implement the Add Node workflow.

Requirements:
1. Add toolbar button: `ADD NODE`.
2. When clicked:
   - capture active tab URL
   - capture active tab title
   - create Bookmark Node
   - default shape: hex prism
   - default color: green/yellow mix
   - save to bookmarks.json
3. If current URL is empty/internal/error page, show a friendly warning.
4. If URL already exists, offer:
   - update existing Node title
   - create duplicate anyway
   - cancel
5. Show a small success notification:
   - `NODE ADDED TO DECK`

Acceptance criteria:
- Current website can be saved as a Node in one click.
- Node appears after entering Deck Space.
- Duplicate behavior is safe.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 29: Deck Space Bookmark View

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
Render saved bookmarks as 3D objects in Deck Space.

Requirements:
1. Load Bookmark Nodes into the OpenGL Deck scene.
2. Display each Node as:
   - hex prism by default
   - cube/panel if shapeType says so
   - neon material based on colorTheme
   - title label
3. Create initial layout:
   - Hex Ring layout: Nodes arranged in a ring around center
   - selected/front Node is largest/brightest
4. Add empty state:
   - `NO NODES FOUND`
   - `VISIT A SITE AND PRESS ADD NODE`
5. Keep rendering stable with at least 100 Nodes.

Acceptance criteria:
- Saved bookmarks appear as 3D objects.
- Empty state is clear.
- Layout looks like a cyberpunk command deck, not a normal menu.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 30: 3D Node Picking and Open URL

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
Allow users to click/select 3D Nodes and open websites.

Requirements:
1. Implement ray picking or another reliable selection method.
2. Mouse hover:
   - highlight Node
   - show URL/title info panel
3. Mouse click:
   - select Node
4. Double click or Enter:
   - open Node URL in a new browser tab
   - exit Deck Space or keep Deck Space based on a simple setting, default exit to new tab
5. Keyboard selection:
   - left/right arrows move selected Node in ring
   - Enter opens selected Node
6. Update lastVisitedUtc and visitCount.

Acceptance criteria:
- User can open websites from 3D Nodes.
- Selection is reliable enough for normal use.
- Opening a Node creates/switches to a browser tab.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 31: Edit and Delete Nodes

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
Add edit/delete actions for Bookmark Nodes.

Requirements:
1. In Deck Space, selected Node should expose actions:
   - OPEN
   - EDIT
   - DELETE
2. Edit panel:
   - title
   - URL
   - shape type
   - color theme
3. Delete:
   - confirmation required
   - red warning styling
4. Persist changes immediately.
5. Validate edited URLs.
6. Do not break if user deletes the selected Node.

Acceptance criteria:
- User can edit title/URL/shape/color.
- User can delete a Node.
- JSON file updates correctly.
- Deck scene updates without restart.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 32: Deck Layout Modes

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
Add multiple visual layouts for Deck Space.

Requirements:
1. Implement at least three layouts:
   - Hex Ring: default carousel ring
   - Cube Orbit: Nodes orbit around center in layered rings
   - Grid Deck: easier fallback layout with 3D tiles
2. Add layout switch control in Deck Space.
3. Persist selected layout in settings.
4. Animate transitions if practical.
5. Ensure all layouts support selection and opening Nodes.
6. Keep the visual theme consistent:
   - black background
   - green/yellow/red neon
   - terminal labels

Acceptance criteria:
- User can switch layouts.
- Each layout works with 20+ Nodes.
- Grid fallback is usable if 3D ring/orbit is overwhelming.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 33: Favicon and Optional Thumbnail Support

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
Add visual identity to Nodes with favicons and optional thumbnails.

Requirements:
1. Capture favicon from CEF if practical.
2. Store favicon under `%APPDATA%/CyberDeckBrowser/favicons`.
3. Reference faviconPath in Bookmark Node.
4. Render favicon on/near 3D Node:
   - as small texture on hex/cube face, or
   - as 2D overlay label if simpler
5. If favicon capture is too time-consuming, create a clean placeholder system and document TODO.
6. Optional future hook:
   - page screenshot thumbnails, not required for v1

Acceptance criteria:
- Nodes can show favicons or clean placeholders.
- Missing favicon does not break rendering.
- Storage is local and organized.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 34: Logging, Crash Handling, and Diagnostics

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
Improve reliability and diagnostics.

Requirements:
1. Store logs under `%APPDATA%/CyberDeckBrowser/logs`.
2. Add rotating log files or size limit.
3. Log:
   - app startup/shutdown
   - CEF init/shutdown
   - navigation failures
   - bookmark storage errors
   - OpenGL renderer info
   - settings/history corruption recovery
4. Add a diagnostic internal page or menu action:
   - app version
   - CEF version
   - OpenGL vendor/renderer/version
   - data directory
   - log file path
5. Add top-level exception guards where appropriate.
6. Avoid showing raw scary errors to normal users unless useful.

Acceptance criteria:
- Problems can be diagnosed from logs.
- App handles common failures cleanly.
- Diagnostics page/menu helps support future users.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 35: Windows Installer Packaging

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
Create a Windows 11 installer.

Requirements:
1. Choose Inno Setup or NSIS.
2. Package:
   - CyberDeckBrowser.exe
   - required CEF runtime files
   - assets
   - license files
   - third-party notices
3. Installer should:
   - install to Program Files or user-local app folder
   - create Start Menu shortcut
   - optionally create Desktop shortcut
   - support uninstall
4. Add build script:
   - `/scripts/build_release.ps1`
   - `/scripts/package_installer.ps1`
5. Add README packaging instructions.
6. Do not require developer tools on the end user machine.

Acceptance criteria:
- Fresh Windows 11 machine/install VM can install and launch app.
- Uninstaller removes program files.
- User data under AppData is not deleted unless explicitly asked.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.



---


# Prompt 36: README, QA Checklist, and Release Candidate

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
Prepare the v1 release candidate documentation and test checklist.

Requirements:
1. Update `README.md` with:
   - project description
   - screenshots placeholders
   - dependencies
   - CEF setup
   - build steps
   - run steps
   - packaging steps
   - user data locations
   - known limitations
2. Add `/docs/QA_CHECKLIST.md`.
3. QA checklist must test:
   - fresh install
   - launch
   - browsing HTTPS site
   - URL normalization
   - tabs new/close/switch
   - back/forward/reload
   - Terminal Mode on/off
   - Add Node
   - enter Deck Space
   - open Node
   - edit Node
   - delete Node
   - settings persist after restart
   - corrupted JSON recovery
   - installer uninstall
4. Add `/docs/RELEASE_NOTES_TEMPLATE.md`.
5. Mark known limitations honestly.

Acceptance criteria:
- Another human can build, run, package, and test the project from docs.
- Release checklist is specific enough to catch broken core features.
- No false claim of production readiness.

Output required:
- Brief summary of what you changed.
- List of files added/changed.
- Exact build commands.
- Exact run commands.
- Verification results.
- Any known issues or TODOs.

Do not continue to the next stage until this stage builds or you clearly explain what blocks it.
