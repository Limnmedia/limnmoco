// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_MAT3_H_
#define LIMNMOCO_MAT3_H_

#include "Vec3.h"

namespace limnmoco {

struct Mat3 {
  float m[3][3];
};

Mat3 mat3_multiply(Mat3 a, Mat3 b);
Vec3 mat3_multiply_v(Mat3 matrix, Vec3 v);
Mat3 mat3_rotation_z(float angle);
Mat3 mat3_rotation_x(float angle);
Mat3 mat3_rotation_y(float angle);

} // namespace limnmoco

#endif // LIMNMOCO_MAT3_H_
