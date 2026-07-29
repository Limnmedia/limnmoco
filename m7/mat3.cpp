// SPDX-License-Identifier: BSD-3-Clause

#include "mat3.h"

Mat3 mat3_multiply(Mat3 a, Mat3 b) {
  return limnmoco::mat3_multiply(a, b);
}

Vec3 mat3_multiply_v(Mat3 matrix, Vec3 v) {
  return limnmoco::mat3_multiply_v(matrix, v);
}

Mat3 mat3_rotation_z(float angle) {
  return limnmoco::mat3_rotation_z(angle);
}

Mat3 mat3_rotation_x(float angle) {
  return limnmoco::mat3_rotation_x(angle);
}

Mat3 mat3_rotation_y(float angle) {
  return limnmoco::mat3_rotation_y(angle);
}
