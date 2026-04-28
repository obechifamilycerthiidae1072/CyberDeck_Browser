# CyberDeck Browser Licensing Plan

## Project License

CyberDeck Browser uses the MIT License for original project code unless a
future business or legal review chooses a different permissive license before
public release.

MIT is suitable for the current goal because it allows private use,
commercial use, modification, distribution, and sublicensing while keeping the
project's own license obligations simple and readable.

## Commercial Reuse Policy

- Keep all third-party licenses documented before code or binaries are added.
- Do not copy random CSS, shaders, 3D code, UI code, images, fonts, icons, or
  snippets from the internet unless the license is verified first.
- Any reused code must include attribution when the source license requires it.
- Do not add GPL-only dependencies unless the project owner explicitly accepts
  the license impact in writing.
- Prefer permissive dependencies: MIT, BSD, Apache-2.0, zlib/libpng, CC0, or
  public domain equivalents where legally usable.
- Store full third-party notices in `THIRD_PARTY_NOTICES.md` before packaging.
- Treat generated code as third-party material until its generator and input
  licenses have been checked.
- Recheck dependency licenses before selling, distributing, or publishing an
  installer.

## Planned Dependencies

| Dependency | Purpose | Expected license | Official source URL | Bundled or external |
| --- | --- | --- | --- | --- |
| Chromium Embedded Framework (CEF) | Embed Chromium browser views for normal web page rendering. | BSD-style CEF license plus Chromium third-party notices. Must verify exact notices for the chosen binary build. | https://bitbucket.org/chromiumembedded/cef | Bundled runtime binaries in app package; source not vendored by default. |
| CMake | Configure and generate Visual Studio/MSVC builds. | BSD 3-Clause. | https://cmake.org | Installed externally as a developer build tool. |
| nlohmann/json | JSON persistence for settings, history, and Deck Space Nodes. | MIT. | https://github.com/nlohmann/json | Prefer external package manager or single-header vendoring with license notice. |
| GLAD or equivalent OpenGL loader | Load OpenGL 4.x function pointers for Deck Space rendering. | Generated GLAD output may be Public Domain, WTFPL, CC0, or affected by Khronos Apache-2.0 specification terms; verify the exact generated files. | https://github.com/Dav1dde/glad | Generated and bundled if used. |
| stb_image | Optional image loading for favicons, thumbnails, or texture assets. | Public domain or MIT dual license. | https://github.com/nothings/stb | Optional single-header vendoring if used. |
| Inno Setup | Optional Windows installer builder. | Inno Setup license; commercial users are requested to purchase a commercial license. Verify before release. | https://jrsoftware.org/isinfo.php | Installed externally as a packaging tool; generated installer distributed. |
| NSIS | Optional Windows installer builder alternative. | Primarily zlib/libpng license, with compression module licenses also documented. | https://nsis.sourceforge.io | Installed externally as a packaging tool; generated installer distributed. |

## Dependency Handling Notes

### CEF / Chromium Embedded Framework

CEF is the central browser dependency. It must be downloaded from official CEF
distribution channels or built from official sources. The selected CEF version,
Chromium version, binary package URL, and included license files must be
recorded before CEF is committed or packaged.

CEF and Chromium include many third-party notices. The installer must ship the
required notices alongside the application.

### CMake

CMake is a developer tool dependency and should not be bundled into the app.
The README should document the minimum tested CMake version once the scaffold
exists.

### nlohmann/json

nlohmann/json is acceptable for v1 JSON persistence because it is permissively
licensed, widely used, and easy to integrate. If vendored as a single header,
the repository must include its copyright and MIT license notice.

### OpenGL Loader

OpenGL on Windows requires loading modern function pointers beyond the system
OpenGL 1.1 exports. If GLAD is used, generated files must be produced from the
official generator and checked into a clear third-party or generated source
location with license notes.

### stb_image

stb_image is optional. Use it only if image loading is needed for favicons,
thumbnails, or Deck Space textures. Its security posture should be considered
before accepting untrusted image inputs from the web.

### Installer Tool

Choose either Inno Setup or NSIS later, not both by default. The installer
stage must include notices for the chosen tool and for every bundled runtime
component.

## Pre-Release License Checklist

- Project `LICENSE` is present.
- `THIRD_PARTY_NOTICES.md` lists every bundled dependency.
- CEF binary package and Chromium notices are recorded.
- Any vendored header-only libraries include license text.
- Any generated OpenGL loader files include generator/source license notes.
- Any icons, fonts, images, shaders, or CSS adapted from outside sources have
  verified licenses and attribution.
- Installer output includes required notices.
- A fresh install can show or provide access to license notices.

## Known Licensing TODOs

- Select the exact CEF binary version during the CEF scaffold stage.
- Decide whether JSON support comes from a package manager or a vendored
  single header.
- Decide whether GLAD is needed or whether another permissive OpenGL loader is
  preferable.
- Choose Inno Setup or NSIS during installer packaging.
- Run a full license review before any commercial sale or public binary
  distribution.
