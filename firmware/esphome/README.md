# ESPHome firmware

This directory contains the unified ESPHome firmware for Scoopy Node.

`Scoopy Compact` and `Scoopy Presence` use the same `scoopy.yaml` configuration. Presence-specific entities are provided by the optional LD2410C radar; if no radar is fitted, the rest of the device continues to operate normally.

## Files

- `scoopy.yaml` — main ESPHome configuration
- `components/` — local ESPHome component overrides used by Scoopy
- `.gitignore` — prevents local ESPHome build output and secrets from being committed

The branded captive-portal HTML source is kept separately in `../captive-portal/`.

## Pinout

| GPIO | Function |
| --- | --- |
| GPIO0 | LD2410C UART TX |
| GPIO1 | LD2410C UART RX |
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
4. Enter the home Wi-Fi credentials in the captive portal.
5. Once connected, Home Assistant should discover Scoopy through the ESPHome integration.

Provisioning over USB serial using ESPHome Improv is also enabled.

## Recovery

Hold both physical buttons for 10 seconds to clear stored Wi-Fi credentials and return Scoopy to setup mode.

## Firmware behaviour

The current configuration provides:

- MAC-suffixed device names so the same firmware image can be flashed to multiple units
- ESPHome native API integration with Home Assistant
- OTA updates
- Two physical button inputs
- Three PWM-controlled LEDs
- A dim red pulse while Scoopy is waiting for Wi-Fi or disconnected
- LD2410C presence, motion, still-target and distance entities when radar is fitted
- A generic I²C expansion bus on GPIO5/GPIO7

## Building and flashing

The production configuration intentionally does not contain a home Wi-Fi SSID or password. Credentials are provisioned after flashing.

From an ESPHome environment, build or flash `scoopy.yaml` in the usual way. Local ESPHome build artefacts and secrets should remain untracked.

Firmware is still under V1 validation, so behaviour and entity names may change before the first formal release.
