#pragma once
#include <Arduino.h>

// IMPORTANT: example values only. Confirm pin availability for your ESP32 variant.
namespace Config {
constexpr uint32_t SERIAL_BAUD = 115200;

// Sensor profile: conditioned analog envelope output.
// Ensure EMG output never exceeds the ESP32 ADC input range.
constexpr int EMG_ADC_PIN = 34;       // input-only on classic ESP32; may differ on other boards
constexpr int ESTOP_PIN = 27;         // active LOW, external pull-up / circuit required
constexpr int SERVO_PIN = 18;

// ADC assumptions are for classic ESP32 Arduino core; verify your board/core version.
constexpr uint16_t ADC_MAX_COUNTS = 4095;
constexpr uint16_t ADC_VALID_MAX = 4000;
constexpr uint16_t ADC_RAIL_LOW = 5;
constexpr uint16_t ADC_RAIL_HIGH = 4090;

// Sample and filtering settings
constexpr uint32_t SAMPLE_PERIOD_MS = 10;
constexpr uint32_t TELEMETRY_PERIOD_MS = 250;
constexpr uint32_t SENSOR_STALE_TIMEOUT_MS = 500;
constexpr uint16_t FILTER_DIVISOR = 4; // IIR alpha = 1 / FILTER_DIVISOR

// Calibration / activation thresholds. Tune from real baseline/contraction data.
constexpr uint16_t CALIBRATION_SAMPLES = 150;
constexpr uint32_t CALIBRATION_TIMEOUT_MS = 10000;
constexpr uint16_t MIN_ACTIVATION_MARGIN = 80;
constexpr uint16_t RELEASE_HYSTERESIS = 35;

// Servo positions and rate limit; calibrate to the printed mechanism's safe travel.
constexpr int SERVO_OPEN_DEG = 20;
constexpr int SERVO_CLOSE_DEG = 85;
constexpr int SERVO_MIN_SAFE_DEG = 10;
constexpr int SERVO_MAX_SAFE_DEG = 100;
constexpr uint32_t SERVO_STEP_PERIOD_MS = 20;

// Optional boot behavior. System starts DISARMED after calibration.
constexpr bool REQUIRE_SERIAL_ARM = true;
}
