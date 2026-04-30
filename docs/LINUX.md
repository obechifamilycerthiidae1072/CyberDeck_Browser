# Linux Support Notes

For Linux installation and day-to-day usage, start with
[README_LINUX.md](../README_LINUX.md).

This document records the Linux architecture boundaries so the Linux and Windows
versions stay cleanly separated.

## Targets

- `CyberDeckBrowser`: Windows application target.
- `CyberDeckBrowserLinux`: Linux diagnostics launcher.
- `CyberDeckBrowserLinuxCef`: separated Linux CEF browser host.
- `CyberDeckWsl2OpenGLProbe`: Linux/WSL2 OpenGL probe.
- `CyberDeckCore`: shared non-UI logic used by both platforms.

## Source Ownership

Linux-specific code belongs under:

```text
src/platform/linux/
```

Windows-specific code belongs under:

```text
src/platform/windows/
src/main/
src/app/
src/ui/
```

Shared logic belongs in platform-neutral modules such as:

```text
src/browser/
src/common/
src/deck/
src/history/
src/render/
src/settings/
```

## Current Linux Browser

The Linux CEF host uses:

- X11 windowing.
- CEF off-screen rendering for web content.
- Built-in bitmap glyph rendering for the native Linux chrome.
- Linux-only software 3D Deck Space projection using shared Deck geometry and
  layout data.
- XDG-compatible user-data paths.

The Linux Deck view is intentionally implemented inside the Linux shell. It does
not reuse the Windows WGL view and does not require Windows headers.

## Build Scripts

- `scripts/build_linux.sh`: builds the Linux diagnostics target.
- `scripts/install_linux.sh`: downloads official Linux CEF, builds the Linux CEF
  host, runs tests, and optionally installs a local launcher.
- `scripts/run_linux.sh`: runs the fresh build-tree CEF browser first, then
  falls back to the installed app.
- `scripts/debug_wsl2.sh`: validates WSL2 display/GPU setup and runs Linux smoke
  checks.

## User Data

Linux runtime data is stored under:

```text
${XDG_DATA_HOME}/cyberdeck-browser
```

or:

```text
~/.local/share/cyberdeck-browser
```

This is separate from the Windows `%APPDATA%\CyberDeckBrowser` path.

## Rules For Future Work

- Keep Linux changes out of Windows UI files unless the change is truly shared
  core behavior.
- Keep Windows installer changes out of Linux packaging work.
- Keep CEF SDK downloads out of git.
- Keep local AI/session memory files out of git.
- Keep real source tests in git; ignore generated test outputs and capture
  artifacts.
