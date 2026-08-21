# Setup

This guide covers first-time setup of Scoopy Node with Wi-Fi, ESPHome, and Home Assistant.

## What you need

- A Scoopy Node flashed with the current firmware
- A USB-C power supply and cable
- A 2.4 GHz Wi-Fi network available to the device
- Home Assistant with the ESPHome integration available

## First-time Wi-Fi setup

1. Power Scoopy over USB-C.
2. Wait for the middle red LED to begin its dim setup pulse.
3. On your phone or computer, open the available Wi-Fi networks and connect to `scoopy-XXXXXX`.
4. Enter the setup password `scoopy123`.
5. The Scoopy setup page should open automatically. If it does not, open the captive portal from the Wi-Fi network's sign-in notification.
6. Select your home Wi-Fi network and enter its password.
7. Scoopy will leave setup mode and connect to the home network. The red setup pulse stops after Wi-Fi connects.

The six-character suffix in `scoopy-XXXXXX` is generated from the ESP32 MAC address, so each device has a unique network and device name.

## Add Scoopy to Home Assistant

Once Scoopy has joined the same network as Home Assistant, Home Assistant should discover it automatically through ESPHome.

1. Open **Settings → Devices & services** in Home Assistant.
2. Look for the discovered Scoopy ESPHome device.
3. Select **Add** and complete the ESPHome setup flow.
4. Open the new Scoopy device page and confirm the expected buttons, LEDs, and any fitted presence entities are available.

Scoopy Compact and Scoopy Presence run the same firmware. If the optional LD2410C radar is not fitted, its entities may remain unavailable while the rest of the device works normally.

## USB provisioning

ESPHome Improv over serial is enabled as an alternative way to provide Wi-Fi credentials while Scoopy is connected over USB.

## Reset Wi-Fi credentials

Hold both Scoopy buttons together for 10 seconds to clear the stored Wi-Fi credentials and return the device to setup mode.

Scoopy will then recreate its `scoopy-XXXXXX` setup network so Wi-Fi can be configured again.

## After setup

The two buttons and three LEDs are exposed through ESPHome so they can be used in Home Assistant automations, scripts, and dashboards. Presence models also expose LD2410C presence information.

The current firmware is still under V1 validation, so entity names and setup behaviour may change before the first formal release.
