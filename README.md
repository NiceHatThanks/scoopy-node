# Scoopy Node

Tiny, local-first smart-home hardware for Home Assistant and ESPHome.

Scoopy Node is a compact USB-C powered room interface built around an ESP32-C3. The current hardware combines two physical buttons, three status LEDs, optional LD2410C mmWave presence sensing, and an I²C expansion connection in a small 3D-printed enclosure.

This repository contains the open hardware, printable enclosure files, ESPHome firmware, captive-portal UI, manufacturing outputs, and project documentation.

## Repository structure

```text
hardware/
  kicad/             Editable KiCad source files
  gerbers/           Fabrication-ready Gerbers and drill files
  bom/               Bill of materials and manufacturing exports

enclosure/
  compact/           Printable files for the compact enclosure
  presence/          Printable files for the mmWave presence enclosure

firmware/
  esphome/           Unified ESPHome configuration and custom components
  captive-portal/    Source for the branded Wi-Fi setup page

docs/
  setup.md           First-time setup and Home Assistant pairing
  assembly.md        Hardware and enclosure assembly notes
  troubleshooting.md Common setup and hardware issues
```

Each enclosure variant contains STL exports and a location for slicer-ready 3MF projects.

## Current firmware

`firmware/esphome/scoopy.yaml` is designed to run on both Scoopy Compact and Scoopy Presence using the same firmware image.

Current firmware features include:

- Unique `scoopy-XXXXXX` device naming using the ESP32 MAC address
- Wi-Fi provisioning through a Scoopy-branded captive portal
- ESPHome native API for Home Assistant
- OTA firmware updates
- USB serial provisioning through Improv
- Two button inputs
- Three controllable status LEDs
- Optional LD2410C mmWave presence sensing
- I²C expansion bus
- Wi-Fi recovery by holding both buttons for 10 seconds

If the optional LD2410C is not fitted, the rest of Scoopy continues to operate normally.

## Status

V1 is currently in development and validation. Hardware, firmware, enclosure files, documentation, and manufacturing outputs may change while testing continues.

## Design goals

- Small enough to place around the home without becoming visual clutter
- Local-first operation with Home Assistant and ESPHome
- Simple USB-C power with no batteries to replace
- Useful physical controls and visible feedback
- Optional room presence sensing
- Open files that are practical to inspect, modify, and reproduce

## Hardware overview

Current V1 hardware includes:

- ESP32-C3
- 2 × tactile buttons
- 3 × configurable LEDs
- Optional HLK-LD2410C mmWave presence sensor
- I²C expansion header
- USB-C power
- 3D-printed enclosure

## Documentation

Start with [`docs/setup.md`](docs/setup.md) for first-time setup.

For hardware files, see [`hardware/README.md`](hardware/README.md). For printable enclosure files, see [`enclosure/README.md`](enclosure/README.md). Firmware details are in [`firmware/esphome/README.md`](firmware/esphome/README.md).

## Website

Project updates and product information: https://nicehatthanks.com

## Licence

A hardware/software/documentation licence will be added before the first formal public release.
