# Scoopy captive portal override

This local ESPHome component keeps ESPHome's captive-portal backend and replaces the customer-facing setup page with the Scoopy-branded UI.

Upstream base: ESPHome 2026.7.4 `esphome/components/captive_portal`.

Preserved from upstream:

- wildcard DNS captive-portal behaviour
- `/config.json` Wi-Fi scan/device identity endpoint
- `/wifisave` Wi-Fi credential storage
- ESPHome Wi-Fi preference handling
- captive-portal lifecycle and Home Assistant/ESPHome integration

Scoopy-specific change:

- normal captive-portal page requests serve the Scoopy-branded frontend embedded in `scoopy_index.h`

The human-editable browser source is `firmware/captive-portal/index.html`. Keep the embedded copy in `scoopy_index.h` in sync when changing the UI.

The production `firmware/esphome/scoopy.yaml` uses this component directly. The V1 onboarding and 10-second factory-reset/reprovisioning flows have been validated on hardware and form part of the `v1.0.0` baseline.

## Third-party licensing

This directory contains code derived from ESPHome's captive-portal implementation. Those upstream-derived portions retain the applicable ESPHome licence terms and are not relicensed by the Scoopy source-available licence.

See [`THIRD_PARTY_NOTICE.md`](THIRD_PARTY_NOTICE.md) for the attribution and licensing details.
