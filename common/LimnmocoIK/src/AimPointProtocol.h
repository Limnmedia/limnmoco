// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_AIM_POINT_PROTOCOL_H_
#define LIMNMOCO_AIM_POINT_PROTOCOL_H_

#include "AimGeometry.h"

#include <stdint.h>

namespace limnmoco {

// Typed representation of MSG_VIRT_AIM_POINT's signed X/Y/Z * 1000 fields.
struct AimPointConfiguration {
  bool enabled;
  KuperPoint point;
};

bool aim_point_from_protocol(uint8_t enabled, int32_t rawX, int32_t rawY,
                             int32_t rawZ, AimPointConfiguration *output);
bool aim_point_to_protocol(const AimPointConfiguration &configuration,
                           uint8_t *enabled, int32_t *rawX, int32_t *rawY,
                           int32_t *rawZ);

} // namespace limnmoco

#endif // LIMNMOCO_AIM_POINT_PROTOCOL_H_
