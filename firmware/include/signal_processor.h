#pragma once
#include <stdint.h>
#include "types.h"

class SignalProcessor {
 public:
  explicit SignalProcessor(uint16_t filterDivisor);
  SensorSample update(uint16_t raw, uint32_t nowMs);
  void reset();
  uint16_t filtered() const { return filtered_; }

 private:
  uint16_t filterDivisor_;
  uint32_t accumulator_;
  uint16_t filtered_;
  bool initialized_;
};
