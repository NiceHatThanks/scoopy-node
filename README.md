# Scoopy Node

Tiny, local-first smart-home hardware for Home Assistant and ESPHome.

Scoopy Node is a compact USB-C powered room interface built around an ESP32-C3. The first hardware revision combines two physical buttons, three status LEDs, optional mmWave presence sensing, and an I²C expansion connection in a small 3D-printed enclosure.

This repository is the home for the open hardware, printable enclosure files, ESPHome firmware, manufacturing outputs, and documentation.

## Repository structure

```text
hardware/
  kicad/         KiCad source files
  gerbers/       Fabrication-ready Gerbers and drill files
  bom/           Bill of materials and manufacturing exports
  schematics/    Schematics and exported PDFs

enclosure/
  stl/           Complete and individual printable STL files
  3mf/           Slicer-ready project files

firmware/
  esphome/       ESPHome configuration and supporting files

docs/
  setup.md       First-time setup
  assembly.md    Hardware and enclosure assembly
  troubleshooting.md

images/          Product and documentation images
```

## Status

The first revision is currently in development and validation. Files may change as hardware testing continues.

## Design goals

- Small enough to place around the home without becoming visual clutter
- Local-first operation with Home Assistant and ESPHome
- Simple USB-C power with no batteries to replace
- Useful physical controls and visible feedback
- Optional room presence sensing
- Open files that are practical to inspect, modify, and reproduce

## Hardware overview

Current V1 hardware includes:

- ESP32-C3 module
- 2 × tactile buttons
- 3 × configurable LEDs
- Optional HLK-LD2410C mmWave presence sensor
- I²C expansion header
- USB-C power
- 3D-printed enclosure

## Website

Project updates and product information: https://nicehatthanks.uk

## Licence

A hardware/software/documentation licence will be added before the first formal public release.
