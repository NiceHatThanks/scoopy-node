# Scoopy experimental Matter firmware

This directory contains the experimental ESP-Matter firmware for Scoopy. It is completely separate from `../esphome/` and does not modify or depend on the existing ESPHome implementation.

## Toolchain

- ESP-IDF 5.5.5
- ESP-Matter 1.6.0
- ESP32-C3
- 4 MB flash

## Current milestone

The first build is intentionally small. It creates a Matter node with a single Generic Switch endpoint reserved for Button 1, starts Matter-over-Wi-Fi commissioning over BLE, and exposes the development Matter console over serial.

No Scoopy GPIO is driven in this first commissioning-only build.

## Build

From a WSL ESP-IDF terminal:

```bash
cd firmware/matter
idf.py set-target esp32c3
idf.py build
idf.py size
```

The first successful build generates `dependencies.lock`; keep that file committed so the managed-component dependency graph remains reproducible.

## First flash

When replacing an existing ESPHome image with Matter for the first time, clear the old flash state once:

```bash
idf.py -p /dev/ttyACM0 erase-flash
idf.py -p /dev/ttyACM0 flash monitor
```

For later Matter firmware updates, do not erase flash unless deliberately testing recovery/reset behaviour.

## Commissioning

This development firmware uses ESP-Matter's development/test commissioning data. The serial Matter console can be used to inspect onboarding codes and network state. Home Assistant commissioning should be performed from the Home Assistant Companion app using Matter's BLE rendezvous and Wi-Fi provisioning flow.

Matter OTA is deliberately disabled for this milestone, although the partition table already reserves dual OTA application slots for later work.
