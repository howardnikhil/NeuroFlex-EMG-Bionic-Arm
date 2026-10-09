# Wiring reference — verify before connecting

The default firmware profile is an ESP32 board with an analog envelope-output EMG module and a hobby servo. These pin assignments are examples, not verified wiring for an existing prototype.

| Signal | Example ESP32 pin | Notes |
|---|---|---|
| EMG analog envelope output | GPIO34 | Classic ESP32 example; ADC1 input-only. Check your board. |
| Emergency-stop input | GPIO27 | Active LOW. Use a properly wired switch and pull-up arrangement. |
| Servo signal | GPIO18 | Signal only; servo needs a suitable power rail. |
| Sensor ground/reference | Per sensor documentation | Confirm grounding/isolation design. |
| Servo supply | External suitable supply | Do not power the servo from a GPIO or undersized board regulator. |

Before energizing: check every voltage, connector polarity, common-reference requirements, ADC limits, motor current, and the emergency-stop behavior. Never connect a person's electrodes to non-isolated mains-powered electronics.
