# Safety and bring-up checklist

This firmware is not a medical-device controller and is not safety certified.

## Bring-up sequence
1. Inspect wiring with all power disconnected.
2. Verify board, sensor output, ADC limits and actuator supply against datasheets.
3. Keep the actuator mechanically disconnected while checking sensor telemetry.
4. Check the E-stop input polarity and confirm `STOP` behavior.
5. Calibrate while relaxed; keep the mechanism unloaded.
6. Verify threshold behavior without actuator power.
7. Enable actuator power only after checking mechanical travel, supply current, physical guarding and a power disconnect.
8. Test slowly and unloaded; stop immediately if motion is unexpected.

## Limits
- The software stop cannot replace a physical power disconnect.
- Servo position commands do not guarantee force/torque limits.
- The example EMG envelope threshold is not validated for your sensor or user.
- Do not wear or use the mechanism for daily activities based only on these tests.
