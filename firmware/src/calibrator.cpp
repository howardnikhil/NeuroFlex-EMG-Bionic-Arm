#include "calibrator.h"

BaselineCalibrator::BaselineCalibrator(uint16_t requiredSamples, uint32_t timeoutMs, uint16_t minMargin)
    : requiredSamples_(requiredSamples == 0 ? 1 : requiredSamples),
      timeoutMs_(timeoutMs), minMargin_(minMargin), startMs_(0), sum_(0), count_(0),
      baseline_(0), activationThreshold_(0), releaseThreshold_(0),
      status_(CalibrationStatus::Idle) {}

void BaselineCalibrator::start(uint32_t nowMs) {
  startMs_ = nowMs;
  sum_ = 0;
  count_ = 0;
  baseline_ = 0;
  activationThreshold_ = 0;
  releaseThreshold_ = 0;
  status_ = CalibrationStatus::Collecting;
}

CalibrationStatus BaselineCalibrator::update(uint16_t sample, uint32_t nowMs) {
  if (status_ != CalibrationStatus::Collecting) return status_;
  if (static_cast<uint32_t>(nowMs - startMs_) > timeoutMs_) {
    status_ = CalibrationStatus::Failed;
    return status_;
  }
  sum_ += sample;
  if (count_ < 65535) ++count_;
  if (count_ >= requiredSamples_) {
    baseline_ = static_cast<uint16_t>(sum_ / count_);
    const uint32_t activation = static_cast<uint32_t>(baseline_) + minMargin_;
    activationThreshold_ = static_cast<uint16_t>(activation > 4095 ? 4095 : activation);
    const uint16_t hysteresis = minMargin_ / 2;
    releaseThreshold_ = baseline_ + hysteresis < activationThreshold_
                            ? static_cast<uint16_t>(baseline_ + hysteresis)
                            : static_cast<uint16_t>(activationThreshold_ > 0 ? activationThreshold_ - 1 : 0);
    status_ = CalibrationStatus::Complete;
  }
  return status_;
}
