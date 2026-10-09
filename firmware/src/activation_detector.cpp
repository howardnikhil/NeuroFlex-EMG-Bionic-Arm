#include "activation_detector.h"

ActivationDetector::ActivationDetector(uint16_t activationThreshold, uint16_t releaseThreshold)
    : activationThreshold_(activationThreshold),
      releaseThreshold_(releaseThreshold),
      active_(false) {
  if (releaseThreshold_ >= activationThreshold_) {
    releaseThreshold_ = activationThreshold_ > 0 ? activationThreshold_ - 1 : 0;
  }
}

bool ActivationDetector::update(uint16_t signal) {
  if (!active_ && signal >= activationThreshold_) active_ = true;
  else if (active_ && signal <= releaseThreshold_) active_ = false;
  return active_;
}

void ActivationDetector::setThresholds(uint16_t activationThreshold, uint16_t releaseThreshold) {
  activationThreshold_ = activationThreshold;
  releaseThreshold_ = releaseThreshold < activationThreshold
                          ? releaseThreshold
                          : (activationThreshold > 0 ? activationThreshold - 1 : 0);
  active_ = false;
}

void ActivationDetector::reset() { active_ = false; }
