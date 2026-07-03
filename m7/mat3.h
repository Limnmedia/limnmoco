// SPDX-License-Identifier: BSD-3-Clause

#ifndef M7_MAT3_H_
#define M7_MAT3_H_

#include "vec3.h"

struct Mat3 {
  float m[3][3];
};

Mat3 mat3_multiply(Mat3 a, Mat3 b);
Vec3 mat3_multiply_v(Mat3 matrix, Vec3 v);
Mat3 mat3_rotation_z(float angle);
Mat3 mat3_rotation_x(float angle);
Mat3 mat3_rotation_y(float angle);

#endif // M7_MAT3_H_
