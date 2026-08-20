# Hardware

Electrical design and manufacturing files for Scoopy Node.

## Structure

- `kicad/scoopy/` — editable KiCad project (`.kicad_pro`, `.kicad_sch`, `.kicad_pcb`)
- `gerbers/v1/` — V1 fabrication-ready Gerber and drill files, plus the upload-ready ZIP
- `bom/v1/` — V1 bill of materials / manufacturing export
- `schematics/` — reserved for exported schematic PDFs when releases are prepared

The KiCad directory contains source design files only. Generated fabrication outputs live separately so the editable project stays clean and manufacturing files are easy to find.

Third-party STEP/GLB models and standalone library components with redistribution restrictions are intentionally not included.

As revisions are frozen, manufacturing outputs should be kept in revision-specific folders such as `v1/`, `v2/`, etc., and corresponding releases/tags can identify known production versions.
