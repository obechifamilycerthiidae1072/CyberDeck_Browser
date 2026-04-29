# Bookmark Node Model

Deck Space bookmarks are called Nodes. The v1 JSON representation is a single
object with fixed key order so later storage code can write stable diffs.

```json
{
  "version": 1,
  "id": "node-20260429t034500z-000001",
  "title": "Example Domain",
  "url": "https://www.example.com/",
  "faviconPath": null,
  "shapeType": "hex",
  "colorTheme": "green",
  "createdUtc": "2026-04-29T03:45:00Z",
  "updatedUtc": "2026-04-29T03:45:00Z",
  "lastVisitedUtc": null,
  "visitCount": 0,
  "deckPosition": null,
  "tags": []
}
```

## Fields

- `id`: stable string identifier owned by the bookmark store.
- `title`: display label; must not be empty.
- `url`: normalized safe `http` or `https` URL.
- `faviconPath`: optional local favicon asset path, or `null`.
- `shapeType`: `hex`, `cube`, or `panel`.
- `colorTheme`: `green`, `yellow`, `red`, or `mixed`.
- `createdUtc` / `updatedUtc`: UTC timestamps in `YYYY-MM-DDTHH:MM:SSZ` form.
- `lastVisitedUtc`: optional UTC timestamp updated when a Node is opened.
- `visitCount`: non-negative count of opens through Deck Space.
- `deckPosition`: optional object with numeric `x`, `y`, and `z` coordinates.
- `tags`: optional labels represented as a stable array; empty when unused.

URLs are normalized with the same policy as the address bar, then restricted to
safe web schemes for Deck Nodes. Local files, script/data URLs, and external
application protocols are rejected.

## Storage

Persistent Nodes live in `%APPDATA%\CyberDeckBrowser\bookmarks.json`:

```json
{
  "version": 1,
  "nodes": []
}
```

The bookmark store writes atomically through a temporary file and replaces the
main file with `MoveFileExW` using write-through semantics. If storage JSON is
corrupted or contains invalid Nodes, the file is renamed with a timestamped
`.corrupt.*.bak` suffix and a clean empty `bookmarks.json` is created.

The `ADD NODE` toolbar action creates a v1 Node from the active tab title and
URL, using a hex shape and mixed green/yellow color by default. Duplicate URLs
prompt before changing storage: update the existing Node title, create a
separate duplicate Node, or cancel.

Deck Space renders saved Nodes with the v1 Hex Ring layout. The first Node is
the selected/front Node, scaled larger and rendered brightest. Empty storage
shows `NO NODES FOUND` and `VISIT A SITE AND PRESS ADD NODE`.

Users can hover a Node to highlight it and show its title/URL panel, click to
select it, use left/right arrows to move selection through the ring, and open
the selected Node with Enter or double click. Opening a Node increments
`visitCount`, sets `lastVisitedUtc`, and creates a new browser tab. The default
setting exits Deck Space after opening; `keepDeckOpenAfterNodeOpen` can keep
the Deck visible for future workflows.

The selected Node action panel exposes `OPEN`, `EDIT`, and `DELETE` controls.
`EDIT` opens a native panel for title, URL, shape, and color. Edited URLs are
normalized and validated before writing. `DELETE` requires confirmation and
updates the in-memory Deck scene immediately after removing the Node from JSON.

Deck layout mode is persisted in settings as `deckLayoutMode`. Supported values
are `hex-ring`, `cube-orbit`, and `grid-deck`; users cycle them in Deck Space
with `L`. Hex Ring is the default, Cube Orbit uses layered rings around center,
and Grid Deck presents a flatter tile layout for easier scanning.

## Favicons

Node visual identity uses `faviconPath`. In this stage CyberDeck creates a
deterministic local SVG placeholder per host under
`%APPDATA%\CyberDeckBrowser\favicons` and stores that path on the Node. Deck
Space renders these as small `[FAV]` overlay badges and shows local identity
status in the selected Node panel. Missing favicon paths are tolerated and old
Nodes are backfilled lazily when Deck Space loads.

TODO: capture real CEF favicon images from page favicon URLs, download/decode
them safely, and replace the placeholder SVG when a valid icon is available.
Optional screenshot thumbnails remain a future hook and are not required for v1.
