# Power design

Power rails cannot be finalized until the exact sensor, board and actuator are known.

- Confirm the EMG module supply voltage and output range from its datasheet.
- Confirm the ESP32 variant and ADC input maximum; do not assume all ESP32-family boards have the same ADC behavior.
- Size actuator power for startup/stall current, not only unloaded current.
- Keep motor/servo current off the MCU regulator and GPIO pins.
- Add appropriate over-current protection and an accessible power disconnect.
- Route motor wiring away from the analog EMG signal.
- Document voltage/current measurements before first full-motion tests.
