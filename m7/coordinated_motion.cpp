// SPDX-License-Identifier: BSD-3-Clause

#include "coordinated_motion.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr uint8_t kMaxAxes = MOTOR_COUNT;

struct CoordinatedMotionState {
  CoordinatedMotionAxis axes[kMaxAxes];
  uint8_t axisCount;
  float duration;
  float elapsed;
  bool active;
};

CoordinatedMotionState state{};

bool targetWithinLimits(const Motor &motor, float target) {
  if (motor.limitLowEnabled && target < motor.limitLow) {
    return false;
  }
  if (motor.limitHighEnabled && target > motor.limitHigh) {
    return false;
  }
  return true;
}

} // namespace

void coordinated_motion_reset() {
  state = CoordinatedMotionState{};
}

bool coordinated_motion_start(Motor *motors,
                              const CoordinatedMotionAxis *axes,
                              uint8_t axisCount) {
  if (axisCount == 0 || axisCount > kMaxAxes) {
    return false;
  }

  float duration = 0.0f;
  for (uint8_t i = 0; i < axisCount; ++i) {
    const CoordinatedMotionAxis &axis = axes[i];
    if (axis.motorIndex >= MOTOR_COUNT ||
        !targetWithinLimits(motors[axis.motorIndex], axis.plan.target)) {
      return false;
    }
    duration = std::max(duration, limnmoco::coordinated_minimum_duration(
        axis.plan.target - axis.plan.start,
        axis.plan.maxVelocity, axis.plan.maxAcceleration));
  }

  state = CoordinatedMotionState{};
  state.axisCount = axisCount;
  state.duration = duration;
  state.active = duration > 0.0f;

  for (uint8_t i = 0; i < axisCount; ++i) {
    state.axes[i] = axes[i];
    Motor &motor = motors[axes[i].motorIndex];
    motor.moves[0].time = 0.0f;
    motor.currentMove = 0;
    motor.currentMoveTime = 0.0f;
    motor.currentVelocity = 0.0f;
    motor.wasMoving = 0;
    motor.moving = state.active ? 1 : 0;
  }

  if (!state.active) {
    for (uint8_t i = 0; i < axisCount; ++i) {
      motors[axes[i].motorIndex].position = axes[i].plan.target;
    }
  }

  return true;
}

bool coordinated_motion_update(Motor *motors, float timeSegment,
                               int32_t *directions) {
  if (!state.active) {
    return false;
  }

  state.elapsed = std::min(state.duration, state.elapsed + timeSegment);
  bool complete = state.elapsed >= state.duration;

  for (uint8_t i = 0; i < state.axisCount; ++i) {
    const CoordinatedMotionAxis &axis = state.axes[i];
    Motor &motor = motors[axis.motorIndex];
    const float oldPosition = motor.position;
    const float newPosition = limnmoco::coordinated_position(
        axis.plan, state.duration, state.elapsed);
    motor.position = newPosition;
    motor.currentVelocity = (newPosition - oldPosition) / timeSegment;
    directions[axis.motorIndex] = motor.currentVelocity > 0.0001f ? 1 :
                                   motor.currentVelocity < -0.0001f ? -1 : 0;
    motor.moving = complete ? 0 : 1;
    if (complete) {
      motor.position = axis.plan.target;
      motor.currentVelocity = 0.0f;
      directions[axis.motorIndex] = 0;
    }
  }

  if (complete) {
    state.active = false;
  }
  return true;
}

bool coordinated_motion_active() {
  return state.active;
}
