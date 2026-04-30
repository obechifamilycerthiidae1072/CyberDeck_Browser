# CyberDeck Browser User Guide

CyberDeck Browser is a Windows desktop browser with two personalities: a normal
CEF web view for real websites, and Deck Space, an OpenGL bookmark world where
bookmarks become 3D Nodes.

## First Launch

Start the app and wait for the first page to finish loading. When CEF is
enabled, the page is rendered by Chromium Embedded Framework. If Terminal Mode
is enabled, the page is restyled after load with the CyberDeck palette: black
backgrounds, neon green text, yellow highlights, red warning/action emphasis,
and monospace typography.

## Toolbar

- `BACK` and `FWD` move through browser history.
- `RELOAD` reloads the active tab.
- `STOP` stops a loading page.
- `TERM` toggles Terminal Mode for the active page and future navigations.
- `SCAN`, `GLOW`, and `FLK` control native CRT shell effects.
- `SET` opens settings and diagnostics.
- `DECK` enters Deck Space. In Deck Space the same button changes to `WEB`.
- `ADD NODE` saves the current page as a Deck Space Node.
- The address bar accepts full URLs, bare domains, and search terms.

## Browsing

Type a URL or search term in the address bar and press `GO` or Enter.

Examples:

```text
google.com
https://www.reddit.com/
hacker news open source browser cef opengl cyberdeck
```

Bare domains are normalized to HTTPS. Search text is sent to the configured
search engine. Unsafe schemes such as script/data URLs are blocked.

## Terminal Mode

Terminal Mode is meant to make the web feel like a retro terminal without
replacing the web engine. The real site still renders through CEF; the app then
injects styling into pages where injection is allowed.

Use `TERM` to toggle it. Some sites may resist or partially override injected
styles because modern pages often ship their own complex CSS.

## Deck Space

Deck Space is the 3D bookmark system. Bookmarks are called Nodes.
Fresh profiles start with default Nodes for Google, Reddit, GitHub, and ChatGPT.

1. Browse to a page.
2. Wait for the real page title to appear.
3. Press `ADD NODE`.
4. Press `DECK`.
5. Select Nodes with the mouse or left/right arrow keys.
6. Press Enter or double-click to open the selected Node.

Deck Space controls:

- Drag mouse: orbit camera.
- Mouse wheel: zoom.
- Left/right arrows: select Nodes.
- `L`: cycle layouts: Hex Ring, Cube Orbit, Grid Deck.
- `E`: edit selected Node.
- `Delete`: delete selected Node after confirmation.
- `Esc` or `WEB`: return to the browser.

## Node Editing

The edit panel supports title, URL, shape, and color changes. URLs are
normalized and validated before saving. Supported Node shapes are `hex`, `cube`,
and `panel`. Supported color themes are `green`, `yellow`, `red`, and `mixed`.

## Data Locations

Normal builds store user data under:

```text
%APPDATA%\CyberDeckBrowser
```

Important files:

- `settings.json`
- `history.json`
- `bookmarks.json`
- `favicons\`
- `logs\cyberdeck.log`

The portable package launcher redirects app-level data into its own
`CyberdeckPortable\Data` folder. CEF internal profile/cache portability still
needs a future source-level CEF settings change.

## Diagnostics

Press `SET` to see diagnostics:

- app version
- CEF state/version text
- OpenGL vendor, renderer, and version
- data directory
- settings path
- log path

The log is useful when checking whether CEF initialized, pages loaded, Deck
Space entered, Nodes opened, or JSON recovery happened.

## Current Release-Candidate Notes

This is a release-candidate project, not a hardened production browser. It is
best treated as a working prototype and portfolio piece until the QA checklist,
installer validation, signing, and security review are completed.
