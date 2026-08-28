# Troubleshooting

This guide covers the most common V1 setup, firmware, hardware, and enclosure problems.

If Scoopy is already assembled, start with the simple checks first: known-good USB power, Wi-Fi setup state, Home Assistant discovery, and a reboot. If something is visibly damaged, smells hot, or is getting unusually warm, disconnect power and inspect the hardware before continuing.

## Device does not power on

- Try a known-good USB-C cable and USB power source.
- Check that the ESP32-C3 module is seated and soldered correctly to the Scoopy carrier PCB.
- Inspect the USB-C connector and nearby solder joints for damage or shorts.
- If you assembled Scoopy yourself, remove the enclosure and confirm the PCB is not being mechanically stressed by the case or screw.
- If the board becomes unusually hot, disconnect it immediately and inspect for a short or assembly fault.

A working Scoopy with no saved Wi-Fi should boot into setup mode and pulse the middle red LED.

## Scoopy is not discovered by Home Assistant

First confirm Scoopy has successfully joined your normal Wi-Fi. The middle red setup pulse should stop after connection and Scoopy gives a brief green confirmation flash.

Then:

- Make sure Home Assistant and Scoopy are on networks that can communicate with each other.
- Open **Settings → Devices & services** and check for a discovered ESPHome device.
- Give discovery a short moment after Scoopy joins Wi-Fi or reboots.
- If it is not discovered automatically, find Scoopy's IP address from your router or from ESPHome logs and add the ESPHome integration manually using that address.
- Rebooting Scoopy and Home Assistant discovery can also clear a stale discovery state.

If Scoopy has fallen back into setup mode and the red LED is pulsing, fix Wi-Fi first.

## Wi-Fi provisioning problems

Scoopy only uses 2.4 GHz Wi-Fi.

During first-time setup:

- Connect to `scoopy-XXXXXX` using password `scoopy123`.
- Ignore warnings that the setup network has **No internet**. That is expected.
- Make sure your phone/tablet/computer remains connected to Scoopy rather than automatically switching back to your normal Wi-Fi.
- On some tablets, open the network details and choose **Manage router** or **Sign in to network**.
- If a computer opens a Microsoft connectivity page, gets stuck, or does not display Scoopy's captive portal correctly, use a phone or tablet for setup.
- Re-check the home Wi-Fi password before pressing **Connect Scoopy**.

If Scoopy previously worked on another Wi-Fi network, hold both buttons together for 10 seconds to clear the saved Wi-Fi details and return it to setup mode.

See [`setup.md`](setup.md) for the full onboarding flow.

## Button input problems

Each physical button is exposed to Home Assistant as an event entity supporting:

- `single`
- `double`
- `hold`

If a button appears not to work:

- Open the Scoopy device in Home Assistant and confirm **Button 1** and **Button 2** are present.
- Use the event entity or an automation trace to confirm which event is being received.
- Check that your automation is using the actual Entity ID assigned by Home Assistant, including any area prefix.
- If you built the hardware yourself, inspect the tactile switch soldering and make sure the printed button moves freely and is not permanently pressing the switch.
- Avoid holding both buttons together for 10 seconds unless you intend to factory-reset Wi-Fi.

The GitHub examples in [`examples.md`](examples.md) show the expected Home Assistant event-trigger format.

## LED behaviour

Normal built-in status behaviour is:

- **Middle red LED pulsing:** Scoopy is waiting for Wi-Fi or has lost its Wi-Fi connection.
- **Brief green confirmation:** Scoopy has successfully connected to Wi-Fi.
- **LEDs off after connection:** normal idle state unless Home Assistant or an automation is controlling them.

Home Assistant exposes three individual LED entities plus the coordinated **LED Pattern** entity.

If an effect behaves unexpectedly:

- Check whether an individual LED and **LED Pattern** are being controlled by different automations at the same time.
- Turning on an individual LED stops the coordinated pattern first, and starting a coordinated pattern takes ownership of all three LEDs.
- Check the automation trace for repeated `light.turn_on`, `light.turn_off`, or effect actions.

If one physical LED never lights, inspect its orientation, soldering, series resistor, and light-pipe alignment. V1 uses 2.2 kΩ series resistors for the LEDs.

## mmWave radar connection and detection

### Scoopy Compact

Scoopy Compact does not have an LD2410C fitted. Because Compact and Presence use the same firmware, radar entities may show as unavailable or **Unknown**. This is expected.

### Scoopy Presence

On Scoopy Presence, Home Assistant should expose **Presence**, **Moving Target**, and **Still Target**.

If those entities remain unavailable or detection is poor:

- Give the LD2410C a few seconds after boot to begin reporting normally.
- Check that the radar is fitted securely and correctly connected to the carrier PCB.
- The firmware UART uses GPIO1 as ESP TX to LD2410 RX and GPIO0 as ESP RX from LD2410 TX.
- Check the UART solder joints and wiring if you are building from the source hardware files.
- Use a nylon M2 screw near the radar where possible. A metal screw may affect the LD2410C, which is why the Presence assembly guide recommends nylon.
- Make sure the enclosure is assembled in the correct orientation and nothing metallic has been added directly in front of or beside the radar unnecessarily.

The standard V1 firmware intentionally keeps the core presence entities simple. High-frequency distance entities are disabled by default to avoid filling Home Assistant Recorder history with constantly changing measurements.

## Bluetooth Proxy problems

Bluetooth Proxy is optional and starts switched off.

If Scoopy is not appearing as a Bluetooth Proxy:

- Open the Scoopy device in Home Assistant and turn on **Bluetooth Proxy** under Configuration.
- Check that Home Assistant is currently connected to Scoopy through ESPHome.
- Go to **Settings → Bluetooth → My network → Adapters** and look for Scoopy.
- Scoopy uses passive BLE scanning and provides one active proxy connection slot, so `1/1` connections simply means that slot is currently in use.
- If Home Assistant disconnects from Scoopy, BLE scanning deliberately stops. It resumes when Home Assistant reconnects if the Bluetooth Proxy switch is still enabled.

On Scoopy Presence, Home Assistant may also discover the onboard LD2410C over BLE. That separate LD2410 BLE integration is optional because Scoopy already reads the radar directly over UART for its normal presence entities.

## Firmware update problems

Released firmware updates appear through the **Firmware Update** entity in Home Assistant.

If an update does not complete:

- Confirm Scoopy is online and has a stable Wi-Fi connection.
- Retry the update from the **Firmware Update** entity.
- Avoid removing power while firmware is actively being written.
- After a successful update, Scoopy reboots automatically and should return with the same device identity, Wi-Fi credentials, and Home Assistant entities.
- Check the **Scoopy Firmware** diagnostic entity to confirm the installed version.

Native ESPHome OTA and USB flashing remain available for development or recovery if required.

## I²C expansion problems

The production firmware enables an I²C bus on:

- GPIO5 — SDA
- GPIO7 — SCL

I²C scanning is enabled, so detected addresses should appear in ESPHome logs.

If an expansion device is not detected:

- Re-check SDA, SCL, ground, and power connections against the V1 KiCad schematic before applying power.
- Confirm the peripheral is compatible with the electrical levels provided by your build.
- Check for an address conflict if more than one I²C device is fitted.
- Check whether the peripheral already includes pull-up resistors and whether your overall bus has suitable pull-ups.
- Keep wiring short while diagnosing the problem.
- Watch the ESPHome logs during boot for the I²C scan result.

The I²C connection is intended for user expansion, so any additional ESPHome component configuration for your chosen peripheral is your responsibility.

## USB-C fit or enclosure alignment

If the enclosure does not close cleanly:
- Check that the two printed buttons are seated correctly in the lid.
- Make sure the USB-C connector is correctly seated through the opening in the base before tightening the PCB screw.
- Confirm the three lid light pipes line up with the three PCB LEDs before clipping the base into the lid.
- Check that the heat-set insert is straight and the M2 screw is not pulling the PCB out of position.

For raw geometry, use the versioned STL exports in [`../enclosure/`](../enclosure/).

For Bambu Studio / 3MF printing, use the maintained [Scoopy MakerWorld project](https://makerworld.com/en/models/3231250-scoopy-home-assistant-esphome-room-controller#profileId-3659818). The slicer project is intentionally maintained there rather than duplicated in GitHub.

## Factory reset and start again

If setup is badly confused or you are moving Scoopy to a new Wi-Fi network:

1. Hold both physical buttons together for 10 seconds.
2. The middle red LED flashes rapidly near the end of the hold.
3. Scoopy clears its saved Wi-Fi credentials.
4. After reboot, the red LED pulses and `scoopy-XXXXXX` returns.
5. Follow [`setup.md`](setup.md) again.

A factory reset clears Wi-Fi setup; it does not require you to reflash the firmware.

## Still stuck?

Check the illustrated setup guide at https://nicehatthanks.com/setup/ and the interactive documentation at https://nicehatthanks.com/docs/.

For project files and technical history, use the GitHub repository. For direct help, use https://nicehatthanks.com/contact.html.
