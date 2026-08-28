# Scoopy Node

Tiny, local-first smart-home hardware for Home Assistant and ESPHome.

Scoopy Node is a compact USB-C powered room interface built around an ESP32-C3. The current hardware combines two physical buttons, three status LEDs, LD2410C mmWave presence sensing, and an I²C expansion connection in a small 3D-printed enclosure.

Scoopy Compact uses the same core hardware and firmware without the mmWave presence sensor for rooms where presence sensing is not needed.

This repository contains the source-available hardware files, printable enclosure files, ESPHome firmware, captive-portal UI, manufacturing outputs, and project documentation.

## Repository structure

```text
hardware/
  kicad/             Editable KiCad source files
  gerbers/           Fabrication-ready Gerbers and drill files
  bom/               Bill of materials and manufacturing exports

enclosure/
  compact/           Printable files for Scoopy Compact
  presence/          Printable files for Scoopy with mmWave presence

firmware/
  esphome/           Unified ESPHome configuration and custom components
  captive-portal/    Source for the branded Wi-Fi setup page

docs/
  setup.md           First-time setup and Home Assistant pairing
  examples.md        Ready-made Home Assistant automations
  assembly.md        Hardware and enclosure assembly notes
  troubleshooting.md Common setup and hardware issues
```

Each enclosure variant contains versioned STL exports. Slicer-ready 3MF projects are intentionally not maintained in this repository; when available, Bambu Studio projects and profiles will be published through Bambu MakerWorld so there is a single maintained copy.

## Current firmware

`firmware/esphome/scoopy.yaml` is designed to run on both Scoopy and Scoopy Compact using the same firmware image.

Current firmware features include:

- Unique `scoopy-XXXXXX` device naming using the ESP32 MAC address
- Wi-Fi provisioning through a Scoopy-branded captive portal
- ESPHome native API for Home Assistant
- OTA firmware updates
- USB serial provisioning through Improv
- Two button inputs
- Three controllable status LEDs
- LD2410C mmWave presence sensing when the radar is fitted
- I²C expansion bus
- Wi-Fi recovery by holding both buttons for 10 seconds

The same firmware also runs on Scoopy Compact. If the LD2410C is not fitted, the rest of Scoopy continues to operate normally.

## Home Assistant examples

Ready-made automations are in [`docs/examples.md`](docs/examples.md).

Before copying an example, find the Entity ID Home Assistant is actually using for your Scoopy:

1. Open **Settings → Devices & services**.
2. Open your **Scoopy** device.
3. Open one of its entities, such as **Button 1**.
4. Select the **cog/settings icon**.
5. Copy the full **Entity ID**.

For example, Home Assistant might show:

```text
event.study_scoopy_test_button_1
```

The useful Scoopy part is:

```text
study_scoopy_test
```

Home Assistant may include the assigned **area name** in generated entity IDs. A device called `scoopy_test` assigned to the Study can therefore become `study_scoopy_test`. That is normal.

In the GitHub examples, replace `scoopy_xxxxxx` with that shared Scoopy part. For example:

```yaml
entity_id: event.scoopy_xxxxxx_button_1
```

becomes:

```yaml
entity_id: event.study_scoopy_test_button_1
```

The same prefix is used for the Scoopy buttons, LEDs, LED Pattern and presence entities.

For the easiest version, use the interactive examples at **https://nicehatthanks.com/docs/**. Paste any Scoopy entity ID there and the page fills the matching Scoopy name into every example automatically.

## Status

`v1.0.0` is the first formal Scoopy release. The V1 hardware, unified firmware, enclosure STL exports, and manufacturing files in that release tag form the reproducible V1 baseline. Development on `main` may move ahead of that baseline after release.

## Design goals

- Small enough to place around the home without becoming visual clutter
- Local-first operation with Home Assistant and ESPHome
- Simple USB-C power with no batteries to replace
- Useful physical controls and visible feedback
- mmWave room presence sensing in the standard Scoopy enclosure
- A smaller Compact version for rooms where presence sensing is not needed
- Source files that are practical to inspect, modify, and reproduce for personal projects

## Hardware overview

Current V1 Scoopy hardware includes:

- ESP32-C3
- 2 × tactile buttons
- 3 × configurable LEDs
- HLK-LD2410C mmWave presence sensor
- I²C expansion header
- USB-C power
- 3D-printed enclosure

Scoopy Compact omits the LD2410C and uses the smaller enclosure.

## Build your own

The hardware, firmware, and printable files are available so you can inspect Scoopy, build one for yourself, modify it, and experiment with your own non-commercial versions.

Commercial manufacture or sale requires prior permission from Nice Hat Thanks.

See [`LICENSE`](LICENSE) for the full terms.

## Documentation

Start with [`docs/setup.md`](docs/setup.md) for first-time setup.

Then see [`docs/examples.md`](docs/examples.md) for copy-ready Home Assistant automations.

For hardware files, see [`hardware/README.md`](hardware/README.md). For printable enclosure files, see [`enclosure/README.md`](enclosure/README.md). Firmware details are in [`firmware/esphome/README.md`](firmware/esphome/README.md).

## Website

Project updates and product information: https://nicehatthanks.com

## Licence

Scoopy is **source available for personal and non-commercial use**. You may inspect the files, build your own, modify them, and share non-commercial derivatives under the terms in [`LICENSE`](LICENSE).

Commercial manufacture, sale, or other commercial use requires prior written permission from Nice Hat Thanks.

Scoopy is not released under an open-source or open-hardware licence.
