# Setup

This guide covers first-time setup of Scoopy with Wi-Fi, ESPHome, and Home Assistant.

Scoopy and Scoopy Compact use the same setup process and the same firmware image.

## What you need

- A Scoopy flashed with the current firmware
- A USB-C power supply and cable
- A 2.4 GHz Wi-Fi network available to the device
- Home Assistant with the ESPHome integration available

Scoopy setup stays local. Your Wi-Fi details are sent directly to the Scoopy you are connected to; there is no Scoopy cloud account involved in onboarding.

## First-time Wi-Fi setup

1. Power Scoopy over USB-C.
2. If it has no saved Wi-Fi network, the middle red LED begins pulsing. This means Scoopy is ready for setup.
3. On your phone, tablet, or computer, open Wi-Fi settings and connect to a network named `scoopy-XXXXXX`.
4. Enter the setup password `scoopy123`.
5. The Scoopy setup page should open automatically.
6. Select your home Wi-Fi network, enter its password, then press **Connect Scoopy**.
7. Once the connection succeeds, the setup page shows **Connected**. It is also normal for the page to disappear as Scoopy leaves setup mode.
8. The middle red setup LED turns off. Scoopy gives a brief green confirmation flash after joining Wi-Fi, then leaves the LEDs off for normal operation.

The final six characters in `scoopy-XXXXXX` come from the ESP32 MAC address, so each Scoopy has a unique setup network and device name.

### If the setup page does not open

Captive portals behave slightly differently between phones, tablets, and computers.

- Make sure your device is still connected to `scoopy-XXXXXX`, not your normal home Wi-Fi.
- A **No internet** warning is expected. Scoopy's temporary setup network is local only.
- On some tablets, open the Wi-Fi network details and choose **Manage router** or **Sign in to network**.
- If a computer opens a Microsoft connectivity page, gets stuck, or does not show the Scoopy page correctly, use a phone or tablet for this part of setup.

For more checks, see [`troubleshooting.md`](troubleshooting.md).

## Add Scoopy to Home Assistant

Once Scoopy has joined the same network as Home Assistant, ESPHome discovery should find it automatically.

1. Open Home Assistant.
2. Look for the discovered Scoopy device, either on a discovery card or under **Settings → Devices & services**.
3. Select **Add** on the discovered ESPHome device.
4. Confirm the prompt with **Submit**.
5. Give the device a useful name, such as **Scoopy - Office**, and assign it to a Home Assistant area if you want to.
6. Open the new Scoopy device page and confirm the expected entities are available.

Home Assistant may include your chosen device name and area in generated Entity IDs. That is normal and is why the examples guide asks you to check the actual Entity ID used by your own Scoopy.

## What should appear in Home Assistant

All Scoopies expose:

- **Button 1** and **Button 2** event entities with `single`, `double`, and `hold` events
- **Left LED**, **Middle LED**, and **Right LED** light entities
- **LED Pattern** for coordinated three-LED effects
- **Bluetooth Proxy** under Configuration, switched off by default
- **Firmware Update** for released OTA updates
- Wi-Fi, IP, uptime, ESPHome version, and Scoopy firmware diagnostic entities

Scoopy Presence also exposes LD2410C mmWave entities including:

- Presence
- Moving Target
- Still Target

Scoopy Compact uses the same firmware without the LD2410C fitted. Radar entities may therefore show as unavailable or Unknown; this is expected and the rest of the device continues to work normally.

## Bluetooth Proxy

Bluetooth Proxy is optional and starts switched off.

To enable it, open your Scoopy in Home Assistant and turn on **Bluetooth Proxy** under Configuration. The setting is remembered across reboots.

Scoopy uses passive BLE scanning and provides one active proxy connection slot. If Home Assistant disconnects, BLE scanning stops automatically and resumes after Home Assistant reconnects when the switch is still enabled.

More detail is available at https://nicehatthanks.com/docs/#bluetooth.

## Firmware updates

When a newer released firmware version is available, Home Assistant shows it through the **Firmware Update** entity on the Scoopy device page.

Open the entity and choose **Install**. Scoopy downloads the firmware over Wi-Fi and reboots automatically.

Normal OTA updates preserve:

- Wi-Fi credentials
- Home Assistant connection
- Device identity
- Existing entity IDs and automations

## USB provisioning

ESPHome Improv over serial is enabled as an alternative way to provide Wi-Fi credentials while Scoopy is connected over USB.

## Reset Wi-Fi credentials

You do not need to reflash Scoopy to change Wi-Fi networks.

1. Press and hold both physical buttons together for 10 seconds.
2. During the final part of the hold, the middle red LED flashes rapidly to warn that a factory reset is about to happen.
3. Scoopy clears its saved Wi-Fi credentials and reboots into setup mode.
4. The middle red LED begins pulsing again and the `scoopy-XXXXXX` setup network returns.

Holding both buttons is reserved for recovery, so avoid holding both together for 10 seconds during normal use.

## After setup

Ready-made Home Assistant automations are in [`examples.md`](examples.md).

For the easiest version, use https://nicehatthanks.com/docs/. Its interactive examples can take one of your Scoopy Entity IDs and fill the matching Scoopy prefix into the examples automatically.
