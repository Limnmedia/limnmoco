// SPDX-License-Identifier: BSD-3-Clause

#include "Vec3.h"

#include <cmath>

namespace limnmoco {

Vec3 vec3_add(Vec3 a, Vec3 b) {
  return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

float vec3_length(Vec3 v) {
  return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

} // namespace limnmoco
