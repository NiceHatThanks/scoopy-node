# Captive portal

This directory contains the human-readable source for Scoopy's branded first-time Wi-Fi setup page.

## Files

- `index.html` — standalone source for the Scoopy captive-portal UI

The production ESPHome firmware uses a local captive-portal component under `../esphome/components/` so ESPHome's normal provisioning behaviour can be retained while presenting the Scoopy setup UI.

## Relationship to the firmware

The production device configuration lives in `../esphome/scoopy.yaml`.

On a fresh or factory-reset device, Scoopy creates a `scoopy-XXXXXX` Wi-Fi access point. After the user connects, the captive portal is used to select the home Wi-Fi network and provide its credentials.

The production firmware deliberately does not compile a customer's Wi-Fi credentials into the image.

The V1 provisioning flow has been validated on hardware for first-time setup and recovery and is part of the `v1.0.0` firmware baseline.

## Editing

Treat `index.html` as the human-readable source for the setup page. The copy served by firmware is embedded in `../esphome/components/captive_portal/scoopy_index.h`.

When changing the captive-portal UI, keep the human-readable HTML and embedded firmware copy in sync, then re-test both fresh-device onboarding and the 10-second physical factory-reset flow before releasing the change.
