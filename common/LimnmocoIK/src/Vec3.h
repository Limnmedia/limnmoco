// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_VEC3_H_
#define LIMNMOCO_VEC3_H_

namespace limnmoco {

struct Vec3 {
  float x;
  float y;
  float z;
};

Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
float vec3_length(Vec3 v);

} // namespace limnmoco

#endif // LIMNMOCO_VEC3_H_
