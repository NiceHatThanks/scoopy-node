# Scoopy Node

Tiny, local-first room hardware for Home Assistant and ESPHome.

Scoopy is a compact USB-C powered room controller built around an ESP32-C3. V1 combines two physical buttons, three PWM LEDs, optional LD2410C mmWave presence sensing, optional Home Assistant Bluetooth Proxy support, and an I²C expansion connection in a small 3D-printed enclosure.

There are two enclosure variants:

- **Scoopy Presence** — the full-size enclosure with an LD2410C mmWave presence sensor.
- **Scoopy Compact** — the smaller enclosure without the radar.

Both variants use the same firmware image. If an LD2410C is not fitted, the rest of Scoopy continues to work normally.

This repository contains the source-available hardware files, versioned STL enclosure exports, ESPHome firmware, captive-portal UI, manufacturing outputs, and project documentation.

## Quick links

- Website and product information: https://nicehatthanks.com
- First-time setup: https://nicehatthanks.com/setup/
- Interactive Home Assistant docs and examples: https://nicehatthanks.com/docs/
- Assembly guide: [`docs/assembly.md`](docs/assembly.md)
- Troubleshooting: [`docs/troubleshooting.md`](docs/troubleshooting.md)
- Bambu Studio / 3MF project: [Scoopy on MakerWorld](https://makerworld.com/en/models/3231250-scoopy-home-assistant-esphome-room-controller#profileId-3659818)

## Repository structure

```text
hardware/
  kicad/             Editable KiCad source files
  gerbers/           Fabrication-ready Gerbers and drill files
  bom/               Bill of materials and manufacturing exports

enclosure/
  compact/           Versioned STL exports for Scoopy Compact
  presence/          Versioned STL exports for Scoopy Presence
  build/             Assembly photos used by the build guide

firmware/
  esphome/           Unified ESPHome configuration and custom components
  captive-portal/    Source for the branded Wi-Fi setup page

docs/
  setup.md           First-time setup and Home Assistant pairing
  examples.md        Ready-made Home Assistant automations
  assembly.md        V1 enclosure assembly guide
  troubleshooting.md Common setup, firmware and hardware issues
```

## Firmware

`firmware/esphome/scoopy.yaml` is the unified production configuration for both Scoopy variants. The V1 firmware identifies itself as `1.0.0` through the ESPHome project version.

V1 features include:

- Unique `scoopy-XXXXXX` naming using the ESP32 MAC address
- Scoopy-branded local Wi-Fi provisioning with a captive portal
- ESPHome native API and Home Assistant discovery
- Two physical buttons exposed as `single`, `double`, and `hold` event entities
- Three individually dimmable PWM LED entities
- Individual pulse, flash, and alert LED effects
- Coordinated three-LED patterns including Circular Pulse, Comet, Breathe All, Heartbeat, Ping Pong, and Ripple
- Pulsing red Wi-Fi/setup indication and a brief green confirmation after connection
- LD2410C presence, moving-target, and still-target entities when the radar is fitted
- Optional Home Assistant Bluetooth Proxy, disabled by default
- Passive BLE scanning with one active proxy connection slot
- Website-hosted firmware updates through the Home Assistant **Firmware Update** entity
- Native ESPHome OTA support for local/development updates
- USB serial provisioning through Improv
- Wi-Fi signal, Wi-Fi percentage, IP address, uptime, ESPHome version, and Scoopy firmware diagnostics
- I²C expansion on GPIO5 (SDA) and GPIO7 (SCL)
- Physical Wi-Fi/factory recovery by holding both buttons for 10 seconds

The firmware deliberately uses one image for both variants. On Scoopy Compact, LD2410C entities may show as unavailable or Unknown because the radar is not fitted; this does not affect buttons, LEDs, Wi-Fi, OTA, Bluetooth Proxy, or the rest of the device.

## Home Assistant

Once Scoopy has joined the same network as Home Assistant, ESPHome discovery should find it automatically.

Ready-made automations are in [`docs/examples.md`](docs/examples.md). The examples cover:

- LED pattern control
- Button-controlled lighting
- Presence-controlled lighting
- Time-aware lighting
- Alarm reminders
- Status LEDs
- Multi-function room controls for lights, fans, and blinds

The interactive version at https://nicehatthanks.com/docs/ can take one of your Scoopy Entity IDs and fill the matching Scoopy prefix into the examples automatically.

## Setup and recovery

For normal first-time setup, use https://nicehatthanks.com/setup/ or [`docs/setup.md`](docs/setup.md).

A fresh or factory-reset Scoopy creates a Wi-Fi network named `scoopy-XXXXXX` with password `scoopy123`. Wi-Fi credentials are sent directly to Scoopy through its local setup page; no Scoopy cloud account is involved.

To clear saved Wi-Fi credentials and return to setup mode, hold both physical buttons together for 10 seconds. The middle red LED flashes rapidly near the end of the hold before the reset is performed.

## Firmware updates

Released firmware can be installed directly from Home Assistant through Scoopy's **Firmware Update** entity. Scoopy reads the update manifest hosted at:

`https://nicehatthanks.com/firmware/scoopy/manifest.json`

An OTA update preserves the device identity, saved Wi-Fi credentials, Home Assistant connection, and existing entity IDs.

## Enclosure and printing

GitHub contains the versioned raw STL exports for Scoopy Compact and Scoopy Presence.

The slicer-ready Bambu Studio / 3MF project is intentionally **not** duplicated in this repository. MakerWorld is the single maintained source for the Bambu project and print profile:

**[Scoopy on MakerWorld](https://makerworld.com/en/models/3231250-scoopy-home-assistant-esphome-room-controller#profileId-3659818)**

See [`enclosure/README.md`](enclosure/README.md) for the STL layout and [`docs/assembly.md`](docs/assembly.md) for assembly.

## Hardware overview

V1 uses a 28 × 28 mm carrier PCBA with:

- ESP32-C3 Super Mini
- 2 × tactile buttons
- 3 × LEDs with 2.2 kΩ series resistors
- Optional HLK-LD2410C mmWave presence sensor
- I²C expansion connection
- USB-C power through the ESP32-C3 module

Scoopy Presence uses the LD2410C and the larger enclosure. Scoopy Compact uses the same carrier and firmware without the radar.

Electrical design and manufacturing files are documented in [`hardware/README.md`](hardware/README.md).

## Assembly

The enclosure build is the same basic process for Compact and Presence: fit the heat-set insert, secure the PCBA, place the two buttons in the lid, then clip the base and lid together while aligning the three light pipes with the LEDs.

For Scoopy Presence, a nylon M2 screw is recommended near the LD2410C because a metal screw may affect the radar.

See the illustrated [`docs/assembly.md`](docs/assembly.md) guide.

## Release status

`v1.0.0` is the first formal Scoopy release. The V1 hardware, firmware, STL exports, documentation, and manufacturing files in that release tag form the reproducible V1 baseline.

Development on `main` may move ahead after the release. Use a release tag when reproducing a known version.

## Build your own

The hardware, firmware, and printable STL files are available so you can inspect Scoopy, build one for yourself, modify it, and experiment with your own non-commercial versions.

Commercial manufacture or sale requires prior permission from Nice Hat Thanks.

See [`LICENSE`](LICENSE) for the full terms.

## Licence

Scoopy is **source available for personal and non-commercial use**. You may inspect the files, build your own, modify them, and share non-commercial derivatives under the terms in [`LICENSE`](LICENSE).

Commercial manufacture, sale, or other commercial use requires prior written permission from Nice Hat Thanks.

Scoopy is not released under an open-source or open-hardware licence.
