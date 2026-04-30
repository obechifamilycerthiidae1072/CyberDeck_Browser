# CyberDeck Browser for Linux

This is the Linux installation and operation guide for CyberDeck Browser.

The Linux version is intentionally separate from the Windows application. Linux
has its own platform source under `src/platform/linux`, its own CMake targets,
its own runtime paths, and its own install/run scripts. Windows source files and
Windows packaging stay on the Windows path.

## Current Linux Status

The Linux build includes:

- A separated Linux CEF browser host: `cyberdeck-browser-cef`.
- X11 windowing with CEF off-screen rendering.
- CyberDeck toolbar controls: `BACK`, `FWD`, `RELOAD`, `STOP`, `TERM`, `SCAN`,
  `GLOW`, `FLK`, `SET`, `DECK`, `ADD NODE`, URL, and `GO`.
- Terminal Mode CSS injection for dark green/yellow/red website styling.
- Built-in toolbar text rendering, so the Linux chrome does not depend on
  distro-specific system fonts.
- Deck Space with a Linux software 3D projection, perspective grid, animated
  extruded Nodes, layout switching, selection, open, add, and delete.
- Linux user data under the XDG data directory.
- A smaller core diagnostics launcher: `cyberdeck-browser`.

The Windows version still owns the Win32 UI, Windows CEF host, WGL OpenGL Deck
Space, and Windows installer packaging. Linux work should remain Linux-only.

## Requirements

Recommended:

- Ubuntu 24.04, Ubuntu 22.04, or WSL2 Ubuntu with WSLg.
- CMake 3.24 or newer.
- GCC or Clang with C++20 support.
- `curl`, `tar`, `bzip2`, and standard CEF runtime libraries.
- A working X11/WSLg desktop session.

The installer script can install the Ubuntu/Debian package dependencies for you
when run with `--deps`.

## Quick Install

From the repository root:

```bash
./scripts/install_linux.sh --deps
```

This command:

- Installs Ubuntu/Debian build and runtime packages with `apt-get`.
- Downloads the official Linux CEF binary archive.
- Extracts CEF to `third_party/cef/linux64`.
- Configures and builds `build-linux-cef`.
- Runs the CTest suite unless `--skip-tests` is used.
- Installs the app to `~/.local/opt/cyberdeck-browser`.
- Creates a launcher at `~/.local/bin/cyberdeck-browser`.

Run the installed browser:

```bash
~/.local/bin/cyberdeck-browser "https://www.example.com"
```

If `~/.local/bin` is on your `PATH`, this also works:

```bash
cyberdeck-browser "https://www.example.com"
```

## Build Without Installing

To build the Linux CEF browser without copying it into `~/.local`:

```bash
./scripts/install_linux.sh --no-install
```

Run the build-tree binary:

```bash
./build-linux-cef/cyberdeck-browser-cef "https://www.example.com"
```

For the core diagnostics launcher only:

```bash
./scripts/build_linux.sh
./build-linux/cyberdeck-browser
```

The diagnostics launcher is not the full browser. It verifies platform paths,
logging, storage initialization, and shared URL normalization.

## Repository Launcher

During development, use:

```bash
./scripts/run_linux.sh "https://www.example.com"
```

The repository launcher prefers the fresh `build-linux-cef` binary first, then
falls back to the installed app if no build-tree CEF binary exists.

## Controls

- `TERM`: toggle terminal-style website CSS.
- `SCAN`: toggle scanlines.
- `GLOW`: toggle shell glow.
- `FLK`: cycle flicker intensity.
- `ADD NODE`: save the current page into Deck Space.
- `DECK`: enter or leave Deck Space.
- `SET`: open diagnostics.
- `Left` / `Right` in Deck Space: select Nodes.
- `Enter` in Deck Space: open the selected Node.
- `Delete` in Deck Space: remove the selected Node.
- `L` in Deck Space: cycle layout mode.
- `Esc` in Deck Space: return to browser mode.

## WSL2 Notes

On WSL2, the scripts set WSLg-friendly Mesa variables when Microsoft WSL is
detected:

```bash
GALLIUM_DRIVER=d3d12
MESA_D3D12_DEFAULT_ADAPTER_NAME=NVIDIA
```

Run the WSL2 debug pass:

```bash
./scripts/debug_wsl2.sh
```

The debug script checks the display environment, NVIDIA visibility, OpenGL
renderer selection, Linux build/tests, and the project OpenGL probe.

## Runtime Paths

Linux user data:

```text
${XDG_DATA_HOME}/cyberdeck-browser
```

Fallback when `XDG_DATA_HOME` is not set:

```text
~/.local/share/cyberdeck-browser
```

Important files:

- `settings.json`
- `history.json`
- `bookmarks.json`
- `favicons/`
- `logs/cyberdeck.log`
- `cef/`

Local install path:

```text
~/.local/opt/cyberdeck-browser
```

Local launcher path:

```text
~/.local/bin/cyberdeck-browser
```

## Custom CEF Archive

The default CEF archive is defined in `scripts/install_linux.sh` and points to
the official CEF Automated Builds CDN.

To use another official Linux CEF archive:

```bash
./scripts/install_linux.sh --cef-url "https://cef-builds.spotifycdn.com/<archive>.tar.bz2" --force-cef
```

Or with an environment variable:

```bash
CEF_URL="https://cef-builds.spotifycdn.com/<archive>.tar.bz2" ./scripts/install_linux.sh --force-cef
```

## Uninstall

Remove the local app and launcher:

```bash
rm -rf ~/.local/opt/cyberdeck-browser
rm -f ~/.local/bin/cyberdeck-browser
```

Optional user data removal:

```bash
rm -rf ~/.local/share/cyberdeck-browser
```

Do not remove user data if you want to keep settings, history, and Deck Space
Nodes.

## Git Hygiene

Installer and browser downloads should contain only the application, runtime
libraries, resources, notices, and launchers. They do not need source tests,
prompt packs, screenshots, local captures, AI assistant memory files, logs,
runtime state, or build trees.

The repository still keeps real source tests under `tests/` because they protect
URL normalization, storage, settings, history, bookmarks, favicon handling, Deck
geometry, and layout behavior. The Windows prompt material under `Promts/` and
`Global Read For coder/` is also intentionally kept. GitHub-generated source
archives use `.gitattributes` to exclude those development-only folders from
release downloads without deleting them from the repository.

## Separation Rules

- Keep Linux code under `src/platform/linux`.
- Keep Windows code under `src/platform/windows`, `src/main`, `src/app`, and
  `src/ui`.
- Keep reusable cross-platform behavior in `CyberDeckCore`.
- Keep Linux scripts in `scripts/build_linux.sh`, `scripts/install_linux.sh`,
  `scripts/run_linux.sh`, and `scripts/debug_wsl2.sh`.
- Do not make Windows files include Linux headers.
- Do not make Linux files include `windows.h`.
