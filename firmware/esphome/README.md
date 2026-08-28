# ESPHome firmware

This directory contains the unified ESPHome production firmware for Scoopy.

Scoopy Presence and Scoopy Compact use the same `scoopy.yaml` configuration. Scoopy Presence includes the LD2410C mmWave radar; Scoopy Compact omits it. When no LD2410C is fitted, the rest of the device continues to operate normally.

## Files

- `scoopy.yaml` — unified production ESPHome configuration
- `components/` — local ESPHome component overrides used by Scoopy
- `.gitignore` — prevents local ESPHome build output and secrets from being committed

The human-readable branded captive-portal HTML source is kept separately in `../captive-portal/`.

## Pinout

| GPIO | Function |
| --- | --- |
| GPIO0 | ESP RX ← LD2410C TX |
| GPIO1 | ESP TX → LD2410C RX |
| GPIO2 | Button 1 |
| GPIO3 | Button 2 |
| GPIO4 | Left green LED |
| GPIO5 | I²C SDA |
| GPIO6 | Middle red LED |
| GPIO7 | I²C SCL |
| GPIO10 | Right green LED |

## First-time provisioning

A fresh or factory-reset Scoopy starts its own Wi-Fi access point named `scoopy-XXXXXX`, where the suffix comes from the ESP32 MAC address.

1. Power Scoopy over USB-C.
2. Connect to the `scoopy-XXXXXX` Wi-Fi network.
3. Use the setup password `scoopy123`.
4. Enter the home Wi-Fi credentials in the Scoopy captive portal.
5. After Scoopy joins Wi-Fi, the red setup indication stops and both green LEDs give a brief confirmation flash.
6. Home Assistant should then discover Scoopy through the ESPHome integration.

Provisioning over USB serial using ESPHome Improv is also enabled.

## Recovery

Hold both physical buttons together for 10 seconds to clear the saved Wi-Fi credentials and return Scoopy to setup mode. The middle red LED flashes rapidly near the end of the hold before the reset is performed.

## Firmware behaviour

The V1 production configuration provides:

- MAC-suffixed `scoopy-XXXXXX` device names so one firmware image can be flashed to multiple units
- ESPHome native API integration and Home Assistant discovery
- native ESPHome OTA support for local/development updates
- website-hosted HTTP firmware updates through the Home Assistant **Firmware Update** entity
- two physical button entities with `single`, `double`, and `hold` events
- three individually dimmable PWM LED entities
- individual LED effects plus coordinated three-LED patterns including Circular Pulse, Comet, Breathe All, Heartbeat, Ping Pong, and Ripple
- a bright pulsing middle red LED while Scoopy is waiting for Wi-Fi or disconnected
- a brief two-green-LED confirmation after Wi-Fi connects
- LD2410C Presence, Moving Target, and Still Target entities when the radar is fitted
- LD2410 distance entities intentionally disabled by default to avoid unnecessary high-frequency Home Assistant Recorder history
- graceful operation without radar on Scoopy Compact
- optional Home Assistant Bluetooth Proxy, disabled by default and remembered across reboots
- passive BLE scanning with one active proxy connection slot when Bluetooth Proxy is enabled
- Wi-Fi signal, Wi-Fi percentage, IP address, uptime, ESPHome version, and Scoopy firmware diagnostics
- USB serial provisioning through Improv
- a generic I²C expansion bus on GPIO5/GPIO7
- physical Wi-Fi/factory recovery by holding both buttons for 10 seconds

## Building and flashing

The production configuration intentionally does not contain a home Wi-Fi SSID or password. Credentials are provisioned after flashing.

From an ESPHome environment, build or flash `scoopy.yaml` in the usual way. Local ESPHome build artefacts, compiled release binaries, production tooling, and secrets should remain untracked.

`v1.0.0` establishes the first formal Scoopy firmware baseline. Future released firmware should increment the Scoopy project version using semantic versioning, with the corresponding Git tag preserving the exact source used for that release.

## Licence

Scoopy-specific firmware configuration and project material are covered by the repository's source-available, non-commercial [`LICENSE`](../../LICENSE). Personal builds and modifications are welcome; commercial use requires prior permission from Nice Hat Thanks.

The local `components/captive_portal/` override includes code derived from ESPHome and therefore retains the applicable upstream ESPHome licence terms. See [`components/captive_portal/THIRD_PARTY_NOTICE.md`](components/captive_portal/THIRD_PARTY_NOTICE.md).
