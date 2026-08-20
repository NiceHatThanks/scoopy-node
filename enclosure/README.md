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
  3mf/
    v1/          slicer-ready project files when available
```

Fusion source files are intentionally not published. The public enclosure files are focused on making it straightforward to print a complete enclosure or replace individual parts.

Keep printable exports grouped by enclosure revision so a known release can always be reproduced later.

Suggested filenames:

```text
scoopy-compact-v1-complete.stl
scoopy-compact-v1-base.stl
scoopy-compact-v1-lid.stl
scoopy-compact-v1-button-1.stl
scoopy-compact-v1-button-2.stl
scoopy-compact-v1-light-pipes.stl
scoopy-compact-v1.3mf

scoopy-presence-v1-complete.stl
scoopy-presence-v1-base.stl
scoopy-presence-v1-lid.stl
scoopy-presence-v1-button-1.stl
scoopy-presence-v1-button-2.stl
scoopy-presence-v1-light-pipes.stl
scoopy-presence-v1.3mf
```
