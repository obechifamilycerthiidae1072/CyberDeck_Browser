# Windows Packaging

CyberDeck Browser uses Inno Setup for Windows installer packaging.
Linux packaging is intentionally separate and is not implemented yet.

## Build Release Binaries

```powershell
.\scripts\build_release.ps1
```

For a CEF-enabled release build, pass the extracted CEF binary distribution:

```powershell
.\scripts\build_release.ps1 -CefRoot "C:\path\to\cef_binary" -RequireCef
```

The CMake CEF integration copies required CEF runtime files into the build
output when CEF is configured with a compatible MSVC generator. The installer
script stages everything next to `CyberDeckBrowser.exe`, so CEF runtime files
must already be present in the release build output before packaging.

## Package Installer

Install Inno Setup 6 on the developer machine, then run:

```powershell
.\scripts\package_installer.ps1
```

If `ISCC.exe` is not on `PATH`, pass it explicitly:

```powershell
.\scripts\package_installer.ps1 -IsccPath "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
```

The generated installer is written to `dist\CyberDeckBrowserSetup-0.1.0.exe`.
Use `-SkipCompile` to prepare and inspect `dist\installer-staging\app` without
requiring Inno Setup:

```powershell
.\scripts\package_installer.ps1 -SkipCompile
```

## Installer Behavior

- Installs program files under Program Files when elevated, with Inno Setup's
  user-level install fallback available.
- Creates a Start Menu shortcut.
- Offers an optional Desktop shortcut.
- Supports normal Windows uninstall.
- Does not delete user data under `%APPDATA%\CyberDeckBrowser`; bookmarks,
  settings, history, favicons, and logs remain unless a future explicit cleanup
  option is added.

## Release Checklist

- Build with the intended CEF runtime and verify `libcef.dll` is staged.
- Confirm `LICENSE`, `THIRD_PARTY_NOTICES.md`, and `README.md` are in the
  installer staging directory.
- Recheck CEF/Chromium notices before public distribution.
- Test install, launch, and uninstall on a clean Windows 11 VM.

## Linux Packaging Status

The Linux build currently produces the separated `cyberdeck-browser` core
launcher only. Future AppImage, deb, rpm, or Flatpak packaging should live in a
Linux-specific packaging path and should not modify the Inno Setup workflow.
