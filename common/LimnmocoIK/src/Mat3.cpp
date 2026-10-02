// SPDX-License-Identifier: BSD-3-Clause

#include "Mat3.h"

#include <cmath>

namespace limnmoco {

Mat3 mat3_multiply(Mat3 a, Mat3 b) {
  Mat3 out{};
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      out.m[row][col] = a.m[row][0] * b.m[0][col] +
                        a.m[row][1] * b.m[1][col] +
                        a.m[row][2] * b.m[2][col];
    }
  }
  return out;
}

Vec3 mat3_multiply_v(Mat3 matrix, Vec3 v) {
  return Vec3{
      matrix.m[0][0] * v.x + matrix.m[0][1] * v.y + matrix.m[0][2] * v.z,
      matrix.m[1][0] * v.x + matrix.m[1][1] * v.y + matrix.m[1][2] * v.z,
      matrix.m[2][0] * v.x + matrix.m[2][1] * v.y + matrix.m[2][2] * v.z,
  };
}

Mat3 mat3_rotation_z(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{c, -s, 0.0f}, {s, c, 0.0f}, {0.0f, 0.0f, 1.0f}}};
}

Mat3 mat3_rotation_x(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{1.0f, 0.0f, 0.0f}, {0.0f, c, -s}, {0.0f, s, c}}};
}

Mat3 mat3_rotation_y(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{c, 0.0f, s}, {0.0f, 1.0f, 0.0f}, {-s, 0.0f, c}}};
}

} // namespace limnmoco
