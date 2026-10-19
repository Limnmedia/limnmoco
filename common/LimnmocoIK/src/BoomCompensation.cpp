// SPDX-License-Identifier: BSD-3-Clause

#include "BoomCompensation.h"

#include <math.h>

namespace limnmoco {

bool boom_compensation_table_is_valid(const BoomCompensationTable &table) {
  const float firstDelta = table.motorSteps[1] - table.motorSteps[0];
  if (firstDelta == 0.0f) {
    return false;
  }

  const bool increasing = firstDelta > 0.0f;
  for (int index = 1; index < kBoomCompensationEntryCount; ++index) {
    const float delta = table.motorSteps[index] - table.motorSteps[index - 1];
    if ((increasing && delta <= 0.0f) || (!increasing && delta >= 0.0f)) {
      return false;
    }
  }
  return true;
}

bool boom_angle_to_steps(const BoomCompensationTable &table,
                         float boomDegrees, float *motorSteps) {
  if (!motorSteps || boomDegrees < kBoomCompensationMinDegrees ||
      boomDegrees > kBoomCompensationMaxDegrees) {
    return false;
  }

  const float tableIndex = boomDegrees - kBoomCompensationMinDegrees;
  const int lowerIndex = static_cast<int>(floorf(tableIndex));
  if (lowerIndex == kBoomCompensationEntryCount - 1) {
    *motorSteps = table.motorSteps[lowerIndex];
    return true;
  }

  const float fraction = tableIndex - lowerIndex;
  *motorSteps = table.motorSteps[lowerIndex] +
      fraction * (table.motorSteps[lowerIndex + 1] - table.motorSteps[lowerIndex]);
  return true;
}

bool boom_steps_to_angle(const BoomCompensationTable &table,
                         float motorSteps, float *boomDegrees) {
  if (!boomDegrees || !boom_compensation_table_is_valid(table)) {
    return false;
  }

  const bool increasing = table.motorSteps[1] > table.motorSteps[0];
  const float first = table.motorSteps[0];
  const float last = table.motorSteps[kBoomCompensationEntryCount - 1];
  if ((increasing && (motorSteps < first || motorSteps > last)) ||
      (!increasing && (motorSteps > first || motorSteps < last))) {
    return false;
  }

  for (int lowerIndex = 0; lowerIndex < kBoomCompensationEntryCount - 1;
       ++lowerIndex) {
    const float lower = table.motorSteps[lowerIndex];
    const float upper = table.motorSteps[lowerIndex + 1];
    if ((increasing && motorSteps >= lower && motorSteps <= upper) ||
        (!increasing && motorSteps <= lower && motorSteps >= upper)) {
      const float fraction = (motorSteps - lower) / (upper - lower);
      *boomDegrees = kBoomCompensationMinDegrees + lowerIndex + fraction;
      return true;
    }
  }

  return false;
}

} // namespace limnmoco
