// SPDX-License-Identifier: BSD-3-Clause

#ifndef M7_COORDINATED_MOTION_H_
#define M7_COORDINATED_MOTION_H_

#include "dfx.h"

#include <stdint.h>

#include <CoordinatedTrajectory.h>

struct CoordinatedMotionAxis {
  uint8_t motorIndex;
  limnmoco::CoordinatedAxisPlan plan;
};

void coordinated_motion_reset();
bool coordinated_motion_start(Motor *motors,
                              const CoordinatedMotionAxis *axes,
                              uint8_t axisCount);
bool coordinated_motion_update(Motor *motors, float timeSegment,
                               int32_t *directions);
bool coordinated_motion_active();

#endif // M7_COORDINATED_MOTION_H_
