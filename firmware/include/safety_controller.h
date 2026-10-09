#pragma once
#include <stdint.h>
#include "types.h"
#include "activation_detector.h"
#include "calibrator.h"

class SafetyController {
 public:
  SafetyController();
  void begin(uint32_t nowMs);
  void sample(const SensorSample& sample, bool estopActive, uint32_t nowMs);
  bool requestArm();
  void requestStop(FaultCode reason = FaultCode::None);
  void requestCalibration(uint32_t nowMs);
  bool requestManualPosition(bool close);
  SystemState state() const { return state_; }
  FaultCode fault() const { return fault_; }
  bool activation() const { return detector_.active(); }
  int targetPosition() const { return targetPosition_; }
  uint16_t baseline() const { return calibrator_.baseline(); }
  uint16_t activationThreshold() const { return detector_.activationThreshold(); }
  uint16_t releaseThreshold() const { return detector_.releaseThreshold(); }
  const SensorSample& lastSample() const { return lastSample_; }

 private:
  void enterStopped(FaultCode reason);
  SystemState state_;
  FaultCode fault_;
  BaselineCalibrator calibrator_;
  ActivationDetector detector_;
  SensorSample lastSample_;
  uint32_t lastValidSampleMs_;
  int targetPosition_;
  bool haveValidSample_;
};
