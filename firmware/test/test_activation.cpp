#include <cassert>
#include "../include/activation_detector.h"

int main() {
  ActivationDetector d(100, 80);
  assert(!d.update(50));
  assert(!d.update(99));
  assert(d.update(100));
  assert(d.update(90));  // hysteresis: remains active above release threshold
  assert(!d.update(80));
  d.setThresholds(120, 90);
  assert(!d.active());
  assert(d.update(120));
  assert(!d.update(90));
  return 0;
}
