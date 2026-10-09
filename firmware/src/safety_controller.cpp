#include "safety_controller.h"
#include "config.h"

SafetyController::SafetyController()
    : state_(SystemState::Boot), fault_(FaultCode::None),
      calibrator_(Config::CALIBRATION_SAMPLES, Config::CALIBRATION_TIMEOUT_MS, Config::MIN_ACTIVATION_MARGIN),
      detector_(500, 465), lastSample_(), lastValidSampleMs_(0),
      targetPosition_(Config::SERVO_OPEN_DEG), haveValidSample_(false) {}

void SafetyController::begin(uint32_t nowMs) {
  state_ = SystemState::Calibrating;
  fault_ = FaultCode::None;
  targetPosition_ = Config::SERVO_OPEN_DEG;
  calibrator_.start(nowMs);
  detector_.reset();
}

void SafetyController::enterStopped(FaultCode reason) {
  state_ = SystemState::Stopped;
  fault_ = reason;
  targetPosition_ = Config::SERVO_OPEN_DEG;
  detector_.reset();
}

void SafetyController::sample(const SensorSample& sample, bool estopActive, uint32_t nowMs) {
  lastSample_ = sample;
  if (sample.valid) {
    haveValidSample_ = true;
    lastValidSampleMs_ = sample.timestampMs;
  }

  if (estopActive) {
    enterStopped(FaultCode::EmergencyStop);
    return;
  }

  if (!sample.valid) return;

  if (sample.raw <= Config::ADC_RAIL_LOW || sample.raw >= Config::ADC_RAIL_HIGH) {
    enterStopped(FaultCode::SensorRail);
    return;
  }

  if (state_ == SystemState::Calibrating) {
    const auto status = calibrator_.update(sample.filtered, nowMs);
    if (status == CalibrationStatus::Complete) {
      detector_.setThresholds(calibrator_.activationThreshold(), calibrator_.releaseThreshold());
      state_ = SystemState::Disarmed;
    } else if (status == CalibrationStatus::Failed) {
      state_ = SystemState::Fault;
      fault_ = FaultCode::CalibrationTimeout;
    }
    return;
  }

  if (state_ == SystemState::Armed) {
    if (static_cast<uint32_t>(nowMs - lastValidSampleMs_) > Config::SENSOR_STALE_TIMEOUT_MS) {
      enterStopped(FaultCode::SensorStale);
      return;
    }
    const bool active = detector_.update(sample.filtered);
    targetPosition_ = active ? Config::SERVO_CLOSE_DEG : Config::SERVO_OPEN_DEG;
  }
}

bool SafetyController::requestArm() {
  if (state_ != SystemState::Disarmed || fault_ != FaultCode::None || !haveValidSample_) return false;
  state_ = SystemState::Armed;
  return true;
}

void SafetyController::requestStop(FaultCode reason) {
  enterStopped(reason);
}

void SafetyController::requestCalibration(uint32_t nowMs) {
  if (state_ == SystemState::Stopped || state_ == SystemState::Fault) {
    fault_ = FaultCode::None;
    state_ = SystemState::Calibrating;
    targetPosition_ = Config::SERVO_OPEN_DEG;
    calibrator_.start(nowMs);
    detector_.reset();
  }
}

bool SafetyController::requestManualPosition(bool close) {
  if (state_ != SystemState::Armed) return false;
  targetPosition_ = close ? Config::SERVO_CLOSE_DEG : Config::SERVO_OPEN_DEG;
  return true;
}
