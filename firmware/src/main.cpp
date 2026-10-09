#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"
#include "types.h"
#include "signal_processor.h"
#include "safety_controller.h"

namespace {
SignalProcessor signalProcessor(Config::FILTER_DIVISOR);
SafetyController safety;
Servo handServo;
uint32_t lastSampleMs = 0;
uint32_t lastTelemetryMs = 0;
uint32_t lastServoStepMs = 0;
int currentServoPosition = Config::SERVO_OPEN_DEG;
bool servoAttached = false;
String commandBuffer;

bool estopActive() {
  // Active-low input. Configure an external pull-up appropriate to the circuit.
  return digitalRead(Config::ESTOP_PIN) == LOW;
}

int clampServoPosition(int requested) {
  if (requested < Config::SERVO_MIN_SAFE_DEG) return Config::SERVO_MIN_SAFE_DEG;
  if (requested > Config::SERVO_MAX_SAFE_DEG) return Config::SERVO_MAX_SAFE_DEG;
  return requested;
}

void applyServoTarget(uint32_t nowMs) {
  const int target = clampServoPosition(safety.targetPosition());
  if (!servoAttached) return;
  if (static_cast<uint32_t>(nowMs - lastServoStepMs) < Config::SERVO_STEP_PERIOD_MS) return;
  lastServoStepMs = nowMs;
  if (currentServoPosition < target) ++currentServoPosition;
  else if (currentServoPosition > target) --currentServoPosition;
  handServo.write(currentServoPosition);
}

void printStatus() {
  const auto& s = safety.lastSample();
  Serial.print("state="); Serial.print(stateName(safety.state()));
  Serial.print(",fault="); Serial.print(faultName(safety.fault()));
  Serial.print(",raw="); Serial.print(s.raw);
  Serial.print(",filtered="); Serial.print(s.filtered);
  Serial.print(",baseline="); Serial.print(safety.baseline());
  Serial.print(",threshold="); Serial.print(safety.activationThreshold());
  Serial.print(",active="); Serial.print(safety.activation() ? 1 : 0);
  Serial.print(",target_deg="); Serial.println(safety.targetPosition());
}

void printHelp() {
  Serial.println("Commands: HELP STATUS STOP ARM CALIBRATE OPEN CLOSE");
}

void handleCommand(String cmd, uint32_t nowMs) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd == "HELP") printHelp();
  else if (cmd == "STATUS") printStatus();
  else if (cmd == "STOP") {
    safety.requestStop(FaultCode::None);
    Serial.println("OK: software stop latched; use CALIBRATE then ARM.");
  } else if (cmd == "ARM") {
    Serial.println(safety.requestArm() ? "OK: armed" : "DENIED: not calibrated, invalid sensor, or fault active");
  } else if (cmd == "CALIBRATE") {
    safety.requestCalibration(nowMs);
    Serial.println("Calibration request processed. Keep muscle relaxed.");
  } else if (cmd == "OPEN") {
    Serial.println(safety.requestManualPosition(false) ? "OK: open requested" : "DENIED: system not armed");
  } else if (cmd == "CLOSE") {
    Serial.println(safety.requestManualPosition(true) ? "OK: close requested" : "DENIED: system not armed");
  } else if (cmd.length() > 0) {
    Serial.println("ERR: unknown command; send HELP");
  }
}

void readSerialCommands(uint32_t nowMs) {
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());
    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        handleCommand(commandBuffer, nowMs);
        commandBuffer = "";
      }
    } else if (commandBuffer.length() < 48) {
      commandBuffer += c;
    } else {
      commandBuffer = "";
      Serial.println("ERR: command too long; discarded");
    }
  }
}
} // namespace

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  pinMode(Config::EMG_ADC_PIN, INPUT);
  pinMode(Config::ESTOP_PIN, INPUT_PULLUP);

  analogReadResolution(12);
  analogSetPinAttenuation(Config::EMG_ADC_PIN, ADC_11db);

  handServo.setPeriodHertz(50);
  handServo.attach(Config::SERVO_PIN);
  servoAttached = handServo.attached();
  currentServoPosition = clampServoPosition(Config::SERVO_OPEN_DEG);
  if (servoAttached) handServo.write(currentServoPosition);

  safety.begin(millis());
  Serial.println();
  Serial.println("NeuroFlex firmware starting");
  Serial.println("IMPORTANT: research prototype; verify hardware profile before use.");
  Serial.printf("EMG ADC pin=%d, E-stop pin=%d, servo pin=%d\n",
                Config::EMG_ADC_PIN, Config::ESTOP_PIN, Config::SERVO_PIN);
  printHelp();
}

void loop() {
  const uint32_t nowMs = millis();
  readSerialCommands(nowMs);

  // Sampling uses elapsed-time scheduling; no long delay blocks command processing.
  if (static_cast<uint32_t>(nowMs - lastSampleMs) >= Config::SAMPLE_PERIOD_MS) {
    lastSampleMs = nowMs;
    const uint16_t raw = static_cast<uint16_t>(analogRead(Config::EMG_ADC_PIN));
    SensorSample sample = signalProcessor.update(raw, nowMs);
    // Reject invalid/saturated samples before they can activate the hand.
    sample.valid = raw > Config::ADC_RAIL_LOW && raw < Config::ADC_RAIL_HIGH && raw <= Config::ADC_VALID_MAX;
    safety.sample(sample, estopActive(), nowMs);
  }

  // E-stop is checked continuously, not only when a new sample is processed.
  if (estopActive()) safety.requestStop(FaultCode::EmergencyStop);

  applyServoTarget(nowMs);

  if (static_cast<uint32_t>(nowMs - lastTelemetryMs) >= Config::TELEMETRY_PERIOD_MS) {
    lastTelemetryMs = nowMs;
    printStatus();
  }
}
