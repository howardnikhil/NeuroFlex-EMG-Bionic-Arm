#pragma once
#include <stdint.h>

class ActivationDetector {
 public:
  ActivationDetector(uint16_t activationThreshold, uint16_t releaseThreshold);
  bool update(uint16_t signal);
  void setThresholds(uint16_t activationThreshold, uint16_t releaseThreshold);
  void reset();
  bool active() const { return active_; }
  uint16_t activationThreshold() const { return activationThreshold_; }
  uint16_t releaseThreshold() const { return releaseThreshold_; }

 private:
  uint16_t activationThreshold_;
  uint16_t releaseThreshold_;
  bool active_;
};
