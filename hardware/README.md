# Hardware

Electrical design and manufacturing files for Scoopy Node.

## Structure

- `kicad/scoopy/` — editable KiCad project source
- `gerbers/v1/` — V1 fabrication-ready Gerber and drill files
- `gerbers/V1-Gerbers.zip` — upload-ready V1 Gerber archive
- `bom/v1/` — V1 bill of materials and manufacturing exports

The KiCad directory contains the editable design source. Generated fabrication outputs live separately so the project stays clean and manufacturing files are easy to identify.

Exported schematic PDFs are not currently committed as a separate directory. The editable schematic is part of the KiCad project and release-specific documentation can be added as V1 is frozen.

Third-party STEP/GLB models and standalone library components with redistribution restrictions are intentionally not included.

## Revisions

Manufacturing outputs should remain grouped by hardware revision (`v1/`, `v2/`, and so on). Releases and Git tags can then identify the exact set of files used for a known hardware build.

V1 is still under development and validation. Do not assume the latest files are production-frozen until a formal release/tag says so.
