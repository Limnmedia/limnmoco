// SPDX-License-Identifier: BSD-3-Clause

#include "vec3.h"

Vec3 vec3_add(Vec3 a, Vec3 b) {
  return limnmoco::vec3_add(a, b);
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  return limnmoco::vec3_sub(a, b);
}

float vec3_length(Vec3 v) {
  return limnmoco::vec3_length(v);
}
