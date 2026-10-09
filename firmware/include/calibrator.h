#pragma once
#include <stdint.h>

enum class CalibrationStatus : uint8_t { Idle, Collecting, Complete, Failed };

class BaselineCalibrator {
 public:
  BaselineCalibrator(uint16_t requiredSamples, uint32_t timeoutMs, uint16_t minMargin);
  void start(uint32_t nowMs);
  CalibrationStatus update(uint16_t sample, uint32_t nowMs);
  CalibrationStatus status() const { return status_; }
  uint16_t baseline() const { return baseline_; }
  uint16_t activationThreshold() const { return activationThreshold_; }
  uint16_t releaseThreshold() const { return releaseThreshold_; }

 private:
  uint16_t requiredSamples_;
  uint32_t timeoutMs_;
  uint16_t minMargin_;
  uint32_t startMs_;
  uint32_t sum_;
  uint16_t count_;
  uint16_t baseline_;
  uint16_t activationThreshold_;
  uint16_t releaseThreshold_;
  CalibrationStatus status_;
};
