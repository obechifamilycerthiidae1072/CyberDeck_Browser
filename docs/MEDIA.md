# Promo Media

This repository includes screenshots and compact x265 promotional videos
captured from the CEF-enabled portable build.

## Videos

- `docs/media/cyberdeck-browser-promo-withtext-x265.mp4`
  - 1080p x265 promotional cut with text overlays.
  - Shows Terminal Mode, Hacker News search, Reddit, Deck Space Nodes, and a
    Node opening ChatGPT.
- `docs/media/cyberdeck-browser-promo-notext-x265.mp4`
  - Same cut without captions for reuse in edits, posts, or alternate overlays.

The original h264 working clips were kept in the local portable package folder
instead of being committed to keep the public repository lighter.

## Screenshots

- `docs/screenshots/google-terminal-mode.jpg`
- `docs/screenshots/hacker-news-search.jpg`
- `docs/screenshots/reddit-terminal-mode.jpg`
- `docs/screenshots/deck-space-nodes.jpg`
- `docs/screenshots/chatgpt-node-opened.jpg`

## Recreate Media Locally

The current media was captured with `ffmpeg` from the portable browser build.
Use the portable app, let each page finish loading, then capture the desktop or
the browser window.

Example x265 encode command:

```powershell
ffmpeg -y -i .\CyberDeckBrowser_Final_WithText.mp4 `
  -c:v libx265 -preset medium -crf 28 -tag:v hvc1 -pix_fmt yuv420p -an `
  .\docs\media\cyberdeck-browser-promo-withtext-x265.mp4
```

For OpenGL Deck Space, desktop-region capture is more reliable than
window-title capture because OpenGL child windows may not appear in all capture
modes.
