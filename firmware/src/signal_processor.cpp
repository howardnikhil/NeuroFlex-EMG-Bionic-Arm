#include "signal_processor.h"

SignalProcessor::SignalProcessor(uint16_t filterDivisor)
    : filterDivisor_(filterDivisor == 0 ? 1 : filterDivisor),
      accumulator_(0), filtered_(0), initialized_(false) {}

SensorSample SignalProcessor::update(uint16_t raw, uint32_t nowMs) {
  if (!initialized_) {
    filtered_ = raw;
    accumulator_ = raw;
    initialized_ = true;
  } else {
    // Integer IIR low-pass filter: y[n] = y[n-1] + (x[n]-y[n-1])/N.
    const int32_t delta = static_cast<int32_t>(raw) - static_cast<int32_t>(filtered_);
    const int32_t next = static_cast<int32_t>(filtered_) + delta / static_cast<int32_t>(filterDivisor_);
    filtered_ = static_cast<uint16_t>(next < 0 ? 0 : next);
    accumulator_ = filtered_;
  }

  SensorSample sample;
  sample.raw = raw;
  sample.filtered = filtered_;
  sample.timestampMs = nowMs;
  sample.valid = true;
  return sample;
}

void SignalProcessor::reset() {
  accumulator_ = 0;
  filtered_ = 0;
  initialized_ = false;
}
