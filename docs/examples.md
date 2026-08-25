# Home Assistant examples

These examples are starting points for Scoopy automations in Home Assistant.

For an interactive version that can fill your Scoopy and other entity IDs into the YAML automatically, use **https://nicehatthanks.com/docs/**.

## Before you copy

### Find your Scoopy Entity ID

In Home Assistant:

1. Go to **Settings → Devices & services**.
2. Open your **Scoopy**.
3. Open an entity such as **Button 1**.
4. Select the **cog/settings icon**.
5. Copy the full **Entity ID**.

You may see something like:

```text
event.study_scoopy_test_button_1
```

Home Assistant may include your assigned **area** in the Entity ID. For example, a Scoopy called `scoopy_test` assigned to the Study may use `study_scoopy_test` as the shared entity prefix.

For these examples, replace:

```text
scoopy_xxxxxx
```

with the shared Scoopy part:

```text
study_scoopy_test
```

So:

```text
event.scoopy_xxxxxx_button_1
```

becomes:

```text
event.study_scoopy_test_button_1
```

You can use the same shared prefix for the Scoopy buttons, LEDs, LED Pattern and presence entities.

> Tip: You can also paste an example into **Edit in YAML** in Home Assistant, then switch back to the visual editor and use Home Assistant's normal entity pickers to choose your lights, fans, blinds, sensors and other devices.

---

## 1. Try Scoopy's LEDs

**Button 1** starts the Circular Pulse effect. **Button 2** stops it.

Change only `scoopy_xxxxxx`.

```yaml
alias: Scoopy - Circular Pulse Demo
description: Button 1 starts the circular pulse, Button 2 stops it

triggers:
  # Scoopy Button 1 - single press
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_1
    options:
      event_type:
        - single
    id: pulse_on

  # Scoopy Button 2 - single press
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_2
    options:
      event_type:
        - single
    id: pulse_off

conditions: []

actions:
  - choose:
      # Button 1 -> start the coordinated Circular Pulse effect
      - conditions:
          - condition: trigger
            id: pulse_on
        sequence:
          - action: light.turn_on
            target:
              entity_id: light.scoopy_xxxxxx_led_pattern
            data:
              effect: Circular Pulse

      # Button 2 -> stop the pattern and turn the LEDs off
      - conditions:
          - condition: trigger
            id: pulse_off
        sequence:
          - action: light.turn_off
            target:
              entity_id: light.scoopy_xxxxxx_led_pattern

mode: restart
```

## 2. Press a button to turn on a light

Press **Button 1** to turn a light on. Press it again to turn the same light off.

Change `scoopy_xxxxxx` and `light.your_light`.

```yaml
alias: Scoopy - Button 1 Toggle Light
description: Button 1 toggles a Home Assistant light on and off

triggers:
  # Scoopy Button 1 - single press
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_1
    options:
      event_type:
        - single

conditions: []

actions:
  # Works with Hue or any other light exposed to Home Assistant
  - action: light.toggle
    target:
      entity_id: light.your_light

mode: restart
```

## 3. Turn the room light on when you're there

Presence turns the light on immediately. Leave the room for **5 seconds** and it turns off.

Change `scoopy_xxxxxx` and `light.your_light`. Increase the 5-second delay if you want a slower switch-off.

```yaml
alias: Scoopy - Presence Controlled Light
description: Presence turns the light on, 5 seconds with no presence turns it off

triggers:
  # Someone is detected -> turn the light on
  - trigger: state
    entity_id: binary_sensor.scoopy_xxxxxx_presence
    from: "off"
    to: "on"
    id: presence_on

  # Nobody detected for 5 seconds -> turn the light off
  - trigger: state
    entity_id: binary_sensor.scoopy_xxxxxx_presence
    from: "on"
    to: "off"
    for:
      seconds: 5
    id: presence_off

conditions: []

actions:
  - choose:
      - conditions:
          - condition: trigger
            id: presence_on
        sequence:
          - action: light.turn_on
            target:
              entity_id: light.your_light

      - conditions:
          - condition: trigger
            id: presence_off
        sequence:
          - action: light.turn_off
            target:
              entity_id: light.your_light

mode: restart
```

## 4. Make the lights softer at night

If the light is off, **Button 1** turns it on at **100%** during the day or **25%** from 9pm to 7am. If the light is already on, Button 1 turns it off.

Change `scoopy_xxxxxx` and `light.your_light`. Change the times and brightness percentages to suit your room.

```yaml
alias: Scoopy - Time Aware Light Button
description: Button 1 toggles a light, using a dimmer brightness at night

triggers:
  # Scoopy Button 1 - single press
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_1
    options:
      event_type:
        - single

conditions: []

actions:
  - choose:
      # If the light is already on, turn it off
      - conditions:
          - condition: state
            entity_id: light.your_light
            state: "on"
        sequence:
          - action: light.turn_off
            target:
              entity_id: light.your_light

      # If it is 9pm-7am, turn it on dimly
      - conditions:
          - condition: or
            conditions:
              - condition: time
                after: "21:00:00"
              - condition: time
                before: "07:00:00"
        sequence:
          - action: light.turn_on
            target:
              entity_id: light.your_light
            data:
              brightness_pct: 25

    # Otherwise it is daytime -> turn it on at full brightness
    default:
      - action: light.turn_on
        target:
          entity_id: light.your_light
        data:
          brightness_pct: 100

mode: restart
```

## 5. Remind me if the alarm isn't set

At **10pm**, if your alarm is still disarmed, Scoopy starts a gentle **Heartbeat** pattern. Arm the alarm and the reminder stops.

Change `scoopy_xxxxxx` and `alarm_control_panel.your_alarm`. Adjust the times if you want different reminder hours.

```yaml
alias: Scoopy - Alarm Reminder
description: Heartbeat at night while the alarm is disarmed

triggers:
  # Check automatically at 10pm
  - trigger: time
    at: "22:00:00"
    id: reminder_start

  # Re-check whenever the alarm state changes
  - trigger: state
    entity_id: alarm_control_panel.your_alarm
    id: alarm_changed

  # Stop any reminder at 5am
  - trigger: time
    at: "05:00:00"
    id: reminder_end

conditions: []

actions:
  - choose:
      # During reminder hours, show Heartbeat while the alarm is disarmed
      - conditions:
          - condition: state
            entity_id: alarm_control_panel.your_alarm
            state: "disarmed"
          - condition: or
            conditions:
              - condition: time
                after: "22:00:00"
              - condition: time
                before: "05:00:00"
        sequence:
          - action: light.turn_on
            target:
              entity_id: light.scoopy_xxxxxx_led_pattern
            data:
              effect: Heartbeat

    # Alarm armed, or reminder hours finished -> stop the pattern
    default:
      - action: light.turn_off
        target:
          entity_id: light.scoopy_xxxxxx_led_pattern

mode: restart
```

## 6. Show me if something is still open

When a door, garage or other binary sensor is open, Scoopy's **middle red LED** turns on. Close it and the LED turns off.

Change `scoopy_xxxxxx` and `binary_sensor.your_door`. For most contact sensors, `on` means open.

```yaml
alias: Scoopy - Door Open Status
description: Red LED stays on while a door or other sensor is open

triggers:
  - trigger: state
    entity_id: binary_sensor.your_door
    id: door_changed

conditions: []

actions:
  - if:
      - condition: state
        entity_id: binary_sensor.your_door
        state: "on"
    then:
      - action: light.turn_on
        target:
          entity_id: light.scoopy_xxxxxx_middle_led
        data:
          brightness_pct: 100
    else:
      - action: light.turn_off
        target:
          entity_id: light.scoopy_xxxxxx_middle_led

mode: restart
```

## 7. Make each button do more

**Button 1:** press to toggle a fan, hold to open or close the blinds.  
**Button 2:** press for time-aware lighting, hold for full brightness.

Change `scoopy_xxxxxx`, `switch.your_fan`, `cover.your_blind` and `light.your_light`.

```yaml
alias: Scoopy - Whole Room Controls
description: Fan, blinds and time-aware lighting controlled by Scoopy

triggers:
  # Button 1 - press -> fan
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_1
    options:
      event_type:
        - single
    id: fan_toggle

  # Button 1 - hold -> blinds
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_1
    options:
      event_type:
        - hold
    id: blinds_toggle

  # Button 2 - press -> time-aware light
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_2
    options:
      event_type:
        - single
    id: light_toggle

  # Button 2 - hold -> maximum brightness
  - trigger: event.received
    target:
      entity_id: event.scoopy_xxxxxx_button_2
    options:
      event_type:
        - hold
    id: light_full

conditions: []

actions:
  - choose:
      # Button 1 press -> toggle the fan
      - conditions:
          - condition: trigger
            id: fan_toggle
        sequence:
          - action: switch.toggle
            target:
              entity_id: switch.your_fan

      # Button 1 hold -> open closed blinds, otherwise close them
      - conditions:
          - condition: trigger
            id: blinds_toggle
        sequence:
          - if:
              - condition: template
                value_template: >
                  {{ state_attr('cover.your_blind', 'current_position') | int(0) == 0 }}
            then:
              - action: cover.open_cover
                target:
                  entity_id: cover.your_blind
            else:
              - action: cover.close_cover
                target:
                  entity_id: cover.your_blind

      # Button 2 press -> toggle the room light
      - conditions:
          - condition: trigger
            id: light_toggle
        sequence:
          - if:
              - condition: state
                entity_id: light.your_light
                state: "on"
            then:
              - action: light.turn_off
                target:
                  entity_id: light.your_light
                data:
                  transition: 1
            else:
              - choose:
                  # 5am-6:30pm -> bright
                  - conditions:
                      - condition: time
                        after: "05:00:00"
                        before: "18:30:00"
                    sequence:
                      - action: light.turn_on
                        target:
                          entity_id: light.your_light
                        data:
                          brightness_pct: 100
                          transition: 1

                  # 6:30pm-10pm -> softer
                  - conditions:
                      - condition: time
                        after: "18:30:00"
                        before: "22:00:00"
                    sequence:
                      - action: light.turn_on
                        target:
                          entity_id: light.your_light
                        data:
                          brightness_pct: 35
                          transition: 1

                  # 10pm-5am -> very dim
                  - conditions:
                      - condition: or
                        conditions:
                          - condition: time
                            after: "22:00:00"
                          - condition: time
                            before: "05:00:00"
                    sequence:
                      - action: light.turn_on
                        target:
                          entity_id: light.your_light
                        data:
                          brightness_pct: 5
                          transition: 1

      # Button 2 hold -> force full brightness
      - conditions:
          - condition: trigger
            id: light_full
        sequence:
          - action: light.turn_on
            target:
              entity_id: light.your_light
            data:
              brightness_pct: 100
              transition: 0.5

mode: restart
```
