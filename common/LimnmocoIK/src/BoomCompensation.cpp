// SPDX-License-Identifier: BSD-3-Clause

#include "BoomCompensation.h"

#include <math.h>

namespace limnmoco {

bool boom_compensation_table_is_valid(const BoomCompensationTable &table) {
  const float firstDelta = table.motorUnits[1] - table.motorUnits[0];
  if (firstDelta == 0.0f) {
    return false;
  }

  const bool increasing = firstDelta > 0.0f;
  for (int index = 1; index < kBoomCompensationEntryCount; ++index) {
    const float delta = table.motorUnits[index] - table.motorUnits[index - 1];
    if ((increasing && delta <= 0.0f) || (!increasing && delta >= 0.0f)) {
      return false;
    }
  }
  return true;
}

bool boom_angle_to_motor_units(const BoomCompensationTable &table,
                               float boomDegrees, float *motorUnits) {
  if (!motorUnits || boomDegrees < kBoomCompensationMinDegrees ||
      boomDegrees > kBoomCompensationMaxDegrees) {
    return false;
  }

  const float tableIndex = boomDegrees - kBoomCompensationMinDegrees;
  const int lowerIndex = static_cast<int>(floorf(tableIndex));
  if (lowerIndex == kBoomCompensationEntryCount - 1) {
    *motorUnits = table.motorUnits[lowerIndex];
    return true;
  }

  const float fraction = tableIndex - lowerIndex;
  *motorUnits = table.motorUnits[lowerIndex] +
      fraction * (table.motorUnits[lowerIndex + 1] - table.motorUnits[lowerIndex]);
  return true;
}

bool boom_motor_units_to_angle(const BoomCompensationTable &table,
                               float motorUnits, float *boomDegrees) {
  if (!boomDegrees || !boom_compensation_table_is_valid(table)) {
    return false;
  }

  const bool increasing = table.motorUnits[1] > table.motorUnits[0];
  const float first = table.motorUnits[0];
  const float last = table.motorUnits[kBoomCompensationEntryCount - 1];
  if ((increasing && (motorUnits < first || motorUnits > last)) ||
      (!increasing && (motorUnits > first || motorUnits < last))) {
    return false;
  }

  for (int lowerIndex = 0; lowerIndex < kBoomCompensationEntryCount - 1;
       ++lowerIndex) {
    const float lower = table.motorUnits[lowerIndex];
    const float upper = table.motorUnits[lowerIndex + 1];
    if ((increasing && motorUnits >= lower && motorUnits <= upper) ||
        (!increasing && motorUnits <= lower && motorUnits >= upper)) {
      const float fraction = (motorUnits - lower) / (upper - lower);
      *boomDegrees = kBoomCompensationMinDegrees + lowerIndex + fraction;
      return true;
    }
  }

  return false;
}

} // namespace limnmoco
