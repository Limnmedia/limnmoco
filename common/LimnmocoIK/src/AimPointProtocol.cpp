// SPDX-License-Identifier: BSD-3-Clause

#include "AimPointProtocol.h"

#include <cmath>
#include <limits>

namespace limnmoco {
namespace {

constexpr float kAimPointProtocolScale = 1000.0f;

bool encode_coordinate(float value, int32_t *output) {
  if (!output || !std::isfinite(value)) {
    return false;
  }
  const double scaled = std::round(
      static_cast<double>(value) * kAimPointProtocolScale);
  if (scaled < static_cast<double>(std::numeric_limits<int32_t>::min()) ||
      scaled > static_cast<double>(std::numeric_limits<int32_t>::max())) {
    return false;
  }
  *output = static_cast<int32_t>(scaled);
  return true;
}

} // namespace

bool aim_point_from_protocol(uint8_t enabled, int32_t rawX, int32_t rawY,
                             int32_t rawZ, AimPointConfiguration *output) {
  if (!output || enabled > 1) {
    return false;
  }
  *output = AimPointConfiguration{
      enabled != 0,
      KuperPoint{rawX / kAimPointProtocolScale, rawY / kAimPointProtocolScale,
                 rawZ / kAimPointProtocolScale}};
  return true;
}

bool aim_point_to_protocol(const AimPointConfiguration &configuration,
                           uint8_t *enabled, int32_t *rawX, int32_t *rawY,
                           int32_t *rawZ) {
  if (!enabled || !rawX || !rawY || !rawZ ||
      !encode_coordinate(configuration.point.x, rawX) ||
      !encode_coordinate(configuration.point.y, rawY) ||
      !encode_coordinate(configuration.point.z, rawZ)) {
    return false;
  }
  *enabled = configuration.enabled ? 1 : 0;
  return true;
}

} // namespace limnmoco
