// SPDX-License-Identifier: BSD-3-Clause

#include "coordinated_motion.h"

#include <cmath>

namespace {

constexpr uint8_t kMaxAxes = MOTOR_COUNT;

struct CoordinatedMotionState {
  CoordinatedMotionAxis axes[kMaxAxes];
  uint8_t axisCount;
  limnmoco::CoordinatedTrajectoryProfile profile;
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

  for (uint8_t i = 0; i < axisCount; ++i) {
    const CoordinatedMotionAxis &axis = axes[i];
    if (axis.motorIndex >= MOTOR_COUNT ||
        !targetWithinLimits(motors[axis.motorIndex], axis.plan.target)) {
      return false;
    }
  }

  state = CoordinatedMotionState{};
  state.axisCount = axisCount;
  limnmoco::CoordinatedAxisPlan plans[kMaxAxes];
  for (uint8_t i = 0; i < axisCount; ++i) {
    plans[i] = axes[i].plan;
  }
  state.profile = limnmoco::coordinated_make_profile(
      plans, axisCount);
  state.active = state.profile.duration > 0.0f;

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

bool coordinated_motion_handoff(Motor *motors,
                                const CoordinatedMotionAxis *axes,
                                uint8_t axisCount) {
  if (!state.active || axisCount == 0 || axisCount > kMaxAxes) {
    return false;
  }

  CoordinatedMotionAxis replanned[kMaxAxes];
  float sharedVelocity = -1.0f;
  for (uint8_t i = 0; i < axisCount; ++i) {
    if (axes[i].motorIndex >= MOTOR_COUNT ||
        !targetWithinLimits(motors[axes[i].motorIndex], axes[i].plan.target)) {
      return false;
    }
    Motor &motor = motors[axes[i].motorIndex];
    replanned[i] = axes[i];
    replanned[i].plan.start = motor.position;
    const float distance = replanned[i].plan.target - motor.position;
    if (fabsf(distance) < 0.001f) {
      if (fabsf(motor.currentVelocity) > 0.001f) {
        return false;
      }
      continue;
    }
    const float normalizedVelocity = motor.currentVelocity / distance;
    if (normalizedVelocity < -0.0001f) {
      return false;
    }
    if (sharedVelocity < 0.0f) {
      sharedVelocity = normalizedVelocity;
    } else if (fabsf(normalizedVelocity - sharedVelocity) >
               fmaxf(0.001f, sharedVelocity * 0.01f)) {
      return false;
    }
  }
  if (sharedVelocity < 0.0f) {
    return false;
  }

  limnmoco::CoordinatedAxisPlan plans[kMaxAxes];
  for (uint8_t i = 0; i < axisCount; ++i) {
    plans[i] = replanned[i].plan;
  }
  const limnmoco::CoordinatedTrajectoryProfile profile =
      limnmoco::coordinated_make_handoff_profile(
          plans, axisCount, sharedVelocity);
  if (profile.duration <= 0.0f) {
    return false;
  }

  state = CoordinatedMotionState{};
  state.axisCount = axisCount;
  state.profile = profile;
  state.active = true;
  for (uint8_t i = 0; i < axisCount; ++i) {
    state.axes[i] = replanned[i];
    Motor &motor = motors[replanned[i].motorIndex];
    motor.moves[0].time = 0.0f;
    motor.currentMove = 0;
    motor.currentMoveTime = 0.0f;
    motor.wasMoving = 0;
    motor.moving = 1;
  }
  return true;
}

bool coordinated_motion_update(Motor *motors, float timeSegment,
                               int32_t *directions) {
  if (!state.active) {
    return false;
  }

  state.elapsed = fminf(
      state.profile.duration, state.elapsed + timeSegment);
  bool complete = state.elapsed >= state.profile.duration;

  for (uint8_t i = 0; i < state.axisCount; ++i) {
    const CoordinatedMotionAxis &axis = state.axes[i];
    Motor &motor = motors[axis.motorIndex];
    const float newPosition = limnmoco::coordinated_position(
        axis.plan, state.profile, state.elapsed);
    motor.position = newPosition;
    motor.currentVelocity = limnmoco::coordinated_velocity(
        axis.plan, state.profile, state.elapsed);
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
