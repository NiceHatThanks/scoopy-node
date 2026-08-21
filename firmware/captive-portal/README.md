# Captive portal

This directory contains the source for Scoopy's branded first-time Wi-Fi setup page.

## Files

- `index.html` — standalone source for the Scoopy captive-portal UI

The live ESPHome firmware uses a local captive-portal component under `../esphome/components/` so the standard ESPHome provisioning behaviour can be retained while presenting the Scoopy setup UI.

## Relationship to the firmware

The main device configuration lives in `../esphome/scoopy.yaml`.

On a fresh or factory-reset device, Scoopy creates a `scoopy-XXXXXX` Wi-Fi access point. After the user connects, the captive portal is used to select the home Wi-Fi network and provide its credentials.

The firmware deliberately does not compile a customer's Wi-Fi credentials into the production image.

## Editing

Treat `index.html` as the human-readable source for the setup page. When changing the captive-portal UI, keep the corresponding ESPHome component implementation in sync so the firmware serves the intended version.

V1 provisioning is still under validation, so the page and flow may change before the first formal release.
