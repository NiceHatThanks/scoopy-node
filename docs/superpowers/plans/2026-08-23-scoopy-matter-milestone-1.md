# Scoopy Matter Milestone 1 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Commission Scoopy into Home Assistant with ESP-Matter, then make Button 1 emit standard Matter Generic Switch events.

**Architecture:** A standalone ESP-IDF project lives at `firmware/matter/`. Matter is a thin integration layer above isolated Scoopy button logic; `firmware/esphome/` is never modified.

**Tech Stack:** ESP-IDF 5.5.5, ESP-Matter 1.6.0 managed component, ESP32-C3, 4 MB flash, Matter-over-Wi-Fi with BLE commissioning.

**Spec:** `docs/superpowers/specs/2026-08-23-scoopy-matter-milestone-1-design.md`

## Global Constraints

- Do not modify, rename, refactor or remove anything in `firmware/esphome/`.
- Pin ESP-Matter exactly to `1.6.0`.
- Target ESP32-C3 with 4 MB flash.
- Keep Matter OTA disabled during this milestone while retaining an OTA-capable partition layout.
- Use development/test commissioning credentials only.
- Use standard Matter Generic Switch; no custom clusters.
- Build after each meaningful change and stop on compiler errors.

---

### Task 1: Commissioning-only Matter project

**Files:**
- Create: `firmware/matter/CMakeLists.txt`
- Create: `firmware/matter/.gitignore`
- Create: `firmware/matter/README.md`
- Create: `firmware/matter/partitions.csv`
- Create: `firmware/matter/sdkconfig.defaults`
- Create: `firmware/matter/main/CMakeLists.txt`
- Create: `firmware/matter/main/idf_component.yml`
- Create: `firmware/matter/main/app_main.cpp`
- Generated after build: `firmware/matter/dependencies.lock`

**Interfaces:**
- Consumes: ESP-Matter `node::create`, `generic_switch::create`, Switch feature APIs and `esp_matter::start`.
- Produces: Root Node endpoint 0 and stable Button 1 Generic Switch endpoint 1.

- [ ] **Step 1: Add project configuration and dependency pin**

Create a normal managed-component ESP-IDF project. `main/idf_component.yml` contains only the direct Matter dependency:

```yaml
dependencies:
  espressif/esp_matter:
    version: "1.6.0"
```

Use a 4 MB dual-OTA partition table but set `CONFIG_ENABLE_OTA_REQUESTOR=n`.

- [ ] **Step 2: Add the minimal Matter node**

`app_main.cpp` initializes NVS, creates the Matter root node, creates one Generic Switch endpoint configured as a momentary/action/multi-press switch, starts Matter and enables useful development console commands. It does not initialize any Scoopy GPIO.

- [ ] **Step 3: Build and verify target**

Run:

```bash
cd firmware/matter
idf.py set-target esp32c3
idf.py build
```

Expected: build succeeds and generated esptool commands target `esp32c3`. Run `idf.py size` and confirm the application fits the 0x1E0000 OTA slot.

- [ ] **Step 4: Commit dependency lockfile**

After the first successful build, verify `dependencies.lock` pins ESP-Matter 1.6.0 and commit it. Do not commit `managed_components/`, `build/`, `sdkconfig`, or `sdkconfig.old`.

- [ ] **Step 5: Flash a clean Matter device and commission**

For the first ESPHome-to-Matter transition only:

```bash
idf.py -p /dev/ttyACM0 erase-flash
idf.py -p /dev/ttyACM0 flash monitor
```

Verify serial output shows Matter startup/commissioning. Obtain the development onboarding code from the Matter console/logs, then commission with the Home Assistant Companion app. Do not continue until commissioning is repeatable.

- [ ] **Step 6: Commit commissioning milestone**

```bash
git add firmware/matter
git commit -m "feat: add experimental Matter commissioning firmware"
```

---

### Task 2: Button 1 application input

**Files:**
- Create: `firmware/matter/main/scoopy_buttons.hpp`
- Create: `firmware/matter/main/scoopy_buttons.cpp`
- Create: `firmware/matter/main/scoopy_matter.hpp`
- Create: `firmware/matter/main/scoopy_matter.cpp`
- Modify: `firmware/matter/main/app_main.cpp`
- Modify: `firmware/matter/main/CMakeLists.txt`

**Interfaces:**
- `scoopy_buttons` produces `ButtonAction::{SinglePress, DoublePress, Hold}` for Button 1.
- `scoopy_matter` consumes those actions and emits Matter Switch cluster events for Endpoint 1.

- [ ] **Step 1: Define and test gesture semantics**

Before hardware integration, define the button state-machine contract: active-low GPIO2, internal pull-up, 20 ms debounce, single press after the double-click window expires, double press on two releases within the window, and hold after the chosen hold threshold. Keep timing constants in `scoopy_buttons.cpp`.

- [ ] **Step 2: Implement GPIO2 input in `scoopy_buttons`**

Use ESP-IDF GPIO APIs and a FreeRTOS-safe timer/task mechanism. The module exposes callbacks/actions only; it contains no ESP-Matter includes.

- [ ] **Step 3: Move Matter endpoint ownership into `scoopy_matter`**

Move the Generic Switch creation from `app_main.cpp` into `scoopy_matter`. Preserve endpoint 1 and the same Switch features so a firmware update does not change the endpoint model.

- [ ] **Step 4: Translate actions into Matter events**

Map single, double and hold actions to the standards-defined events supported by the configured Switch cluster. Perform Matter data-model/event operations on the CHIP/Matter work context rather than directly from an arbitrary GPIO ISR.

- [ ] **Step 5: Build, flash and inspect serial logs**

```bash
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

Do not erase flash; verify the existing Matter fabric and Wi-Fi state survive the firmware update.

- [ ] **Step 6: Validate Home Assistant automation triggers**

In Home Assistant, verify Button 1 exposes Matter button/device triggers and that single press, double press and hold can be distinguished in automations.

- [ ] **Step 7: Commit Button 1 milestone**

```bash
git add firmware/matter/main
git commit -m "feat: add Matter events for button 1"
```

Milestone 1 is complete only after this hardware test passes.
