# Scoopy captive portal override

This local ESPHome component keeps ESPHome's captive-portal backend and replaces only the customer-facing setup page.

Upstream base: ESPHome 2026.7.4 `esphome/components/captive_portal`.

Preserved from upstream:

- wildcard DNS captive-portal behaviour
- `/config.json` Wi-Fi scan/device identity endpoint
- `/wifisave` Wi-Fi credential storage
- ESPHome Wi-Fi preference handling
- captive-portal lifecycle and Home Assistant/ESPHome integration

Scoopy-specific change:

- all normal captive-portal page requests serve the Scoopy-branded frontend from `scoopy_index.h`

The human-editable browser prototype is `firmware/captive-portal/index.html`. Keep the embedded copy in `scoopy_index.h` in sync when changing the UI.

During hardware validation use `firmware/esphome/scoopy-onboarding.yaml`. The production `scoopy.yaml` remains untouched until the branded portal has passed the full fresh-device and factory-reset onboarding test.
