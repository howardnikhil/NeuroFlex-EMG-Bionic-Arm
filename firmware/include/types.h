#pragma once
#include <stdint.h>

enum class SystemState : uint8_t {
  Boot,
  Calibrating,
  Disarmed,
  Armed,
  Stopped,
  Fault
};

enum class FaultCode : uint8_t {
  None,
  EmergencyStop,
  CalibrationTimeout,
  SensorRail,
  SensorStale,
  InvalidConfiguration
};

struct SensorSample {
  uint16_t raw = 0;
  uint16_t filtered = 0;
  uint32_t timestampMs = 0;
  bool valid = false;
};

inline const char* stateName(SystemState state) {
  switch (state) {
    case SystemState::Boot: return "BOOT";
    case SystemState::Calibrating: return "CALIBRATING";
    case SystemState::Disarmed: return "DISARMED";
    case SystemState::Armed: return "ARMED";
    case SystemState::Stopped: return "STOPPED";
    case SystemState::Fault: return "FAULT";
    default: return "UNKNOWN";
  }
}

inline const char* faultName(FaultCode fault) {
  switch (fault) {
    case FaultCode::None: return "NONE";
    case FaultCode::EmergencyStop: return "ESTOP";
    case FaultCode::CalibrationTimeout: return "CAL_TIMEOUT";
    case FaultCode::SensorRail: return "SENSOR_RAIL";
    case FaultCode::SensorStale: return "SENSOR_STALE";
    case FaultCode::InvalidConfiguration: return "INVALID_CONFIG";
    default: return "UNKNOWN";
  }
}
