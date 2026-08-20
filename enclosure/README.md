# Enclosure

Printable enclosure files for Scoopy Node.

## Structure

- `stl/v1/full/` — complete V1 enclosure STL exports
- `stl/v1/parts/` — individual printable V1 enclosure components
- `3mf/v1/` — slicer-ready V1 project files when available

Fusion source files are intentionally not published. The public enclosure files are focused on making it straightforward to print, replace, or modify individual physical parts.

Keep printable exports grouped by hardware/enclosure revision so a known release can always be reproduced later.

When adding files, use clear names such as:

```text
scoopy-v1-complete.stl
scoopy-v1-base.stl
scoopy-v1-lid.stl
scoopy-v1-button-1.stl
scoopy-v1-button-2.stl
scoopy-v1-light-pipes.stl
scoopy-v1.3mf
```
