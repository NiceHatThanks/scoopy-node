# Enclosure

Printable enclosure files for Scoopy Node.

Scoopy has two enclosure variants:

- `compact/` — the smaller enclosure without presence sensing
- `presence/` — the larger enclosure designed for the mmWave presence version

Each variant is organised the same way:

```text
<variant>/
  stl/
    v1/
      full/      complete enclosure STL export
      parts/     individual printable components
```

## Bambu Studio / 3MF project

The slicer-ready Bambu Studio / 3MF project is intentionally **not** tracked in this repository.

MakerWorld is the single maintained source for the Bambu project and print profile:

**[Scoopy on MakerWorld](https://makerworld.com/en/models/3231250-scoopy-home-assistant-esphome-room-controller#profileId-3659818)**

This avoids having separate GitHub and MakerWorld copies drift apart over time. GitHub remains the source for the versioned raw STL exports; MakerWorld is the source for the maintained Bambu slicer project/profile.

Fusion source files are intentionally not published. The public enclosure files are focused on making it straightforward to print a complete enclosure or replace individual parts.

Keep printable STL exports grouped by enclosure revision so a known release can always be reproduced later.

## V1 filenames

The committed V1 exports use these names:

```text
scoopy-compact-v1-complete.stl
scoopy-compact-v1-base.stl
scoopy-compact-v1-lid.stl
scoopy-compact-v1-button1.stl
scoopy-compact-v1-button2.stl
scoopy-compact-v1-lightpipe-1.stl
scoopy-compact-v1-lightpipe-2.stl
scoopy-compact-v1-lightpipe-3.stl

scoopy-presence-v1-complete.stl
scoopy-presence-v1-base.stl
scoopy-presence-v1-lid.stl
scoopy-presence-v1-button1.stl
scoopy-presence-v1-button2.stl
scoopy-presence-v1-lightpipe-1.stl
scoopy-presence-v1-lightpipe-2.stl
scoopy-presence-v1-lightpipe-3.stl
```

For assembly, see [`../docs/assembly.md`](../docs/assembly.md).
