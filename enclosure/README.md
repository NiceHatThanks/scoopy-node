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

Slicer-ready 3MF/Bambu Studio project files are intentionally not tracked in this repository. When available, they will be published through Bambu MakerWorld so there is one maintained copy rather than separate GitHub and Bambu versions.

Fusion source files are intentionally not published. The public enclosure files are focused on making it straightforward to print a complete enclosure or replace individual parts.

Keep printable STL exports grouped by enclosure revision so a known release can always be reproduced later.

Suggested filenames:

```text
scoopy-compact-v1-complete.stl
scoopy-compact-v1-base.stl
scoopy-compact-v1-lid.stl
scoopy-compact-v1-button-1.stl
scoopy-compact-v1-button-2.stl
scoopy-compact-v1-light-pipes.stl

scoopy-presence-v1-complete.stl
scoopy-presence-v1-base.stl
scoopy-presence-v1-lid.stl
scoopy-presence-v1-button-1.stl
scoopy-presence-v1-button-2.stl
scoopy-presence-v1-light-pipes.stl
```
