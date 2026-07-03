// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <algorithm>
#include <cmath>

namespace limnmoco {
namespace {

constexpr double kPi = 3.141592653589793238462643383279502884;

struct Mat3 {
  double m[3][3];
};

double clamp(double value, double lo, double hi) {
  return std::max(lo, std::min(hi, value));
}

Vec3 add(Vec3 a, Vec3 b) {
  return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 sub(Vec3 a, Vec3 b) {
  return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

double length(Vec3 v) {
  return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Mat3 multiply(Mat3 a, Mat3 b) {
  Mat3 out{};
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      out.m[row][col] = a.m[row][0] * b.m[0][col] + a.m[row][1] * b.m[1][col] +
                        a.m[row][2] * b.m[2][col];
    }
  }
  return out;
}

Vec3 multiply(Mat3 matrix, Vec3 v) {
  return Vec3{
      matrix.m[0][0] * v.x + matrix.m[0][1] * v.y + matrix.m[0][2] * v.z,
      matrix.m[1][0] * v.x + matrix.m[1][1] * v.y + matrix.m[1][2] * v.z,
      matrix.m[2][0] * v.x + matrix.m[2][1] * v.y + matrix.m[2][2] * v.z,
  };
}

Mat3 rotationZ(double angle) {
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return Mat3{{{c, -s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}}};
}

Mat3 rotationX(double angle) {
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return Mat3{{{1.0, 0.0, 0.0}, {0.0, c, -s}, {0.0, s, c}}};
}

Mat3 rotationY(double angle) {
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return Mat3{{{c, 0.0, s}, {0.0, 1.0, 0.0}, {-s, 0.0, c}}};
}

Mat3 rotationMatrixFromVirtualAxes(double vpanDeg, double vtiltDeg, double vrollDeg) {
  return multiply(multiply(rotationZ(radians(vpanDeg)), rotationX(radians(vtiltDeg))),
                  rotationY(radians(vrollDeg)));
}

} // namespace

double degrees(double radiansValue) {
  return radiansValue * 180.0 / kPi;
}

double radians(double degreesValue) {
  return degreesValue * kPi / 180.0;
}

CraneSolveResult solveLimnmocoCrane(const VirtualPose &pose, const CraneGeometry &geometry) {
  CraneSolveResult result{};

  result.target = Vec3{
      pose.vew,
      pose.vtrack,
      pose.vheight,
  };

  const Mat3 rotation = rotationMatrixFromVirtualAxes(pose.vpanDeg, pose.vtiltDeg, pose.vrollDeg);
  const Vec3 offsetLocal = Vec3{
      geometry.offsetX,
      geometry.offsetY,
      geometry.offsetZ,
  };

  result.offsetWorld = multiply(rotation, offsetLocal);
  result.panTarget = sub(result.target, result.offsetWorld);

  const double boomLength = std::max(0.001, geometry.boomLength);
  const double boomRawRatio = result.panTarget.z / boomLength;
  const double boomRatio = clamp(boomRawRatio, -1.0, 1.0);
  const double boomRad = std::asin(boomRatio);
  result.boomClamped = boomRawRatio != boomRatio;

  double horizontalReach = boomLength * std::cos(boomRad) + geometry.extensionLength;
  if (std::abs(horizontalReach) < 0.000001) {
    horizontalReach = 0.000001;
  }

  const double swingRawRatio = result.panTarget.x / horizontalReach;
  const double swingRatio = clamp(swingRawRatio, -1.0, 1.0);
  const double swingRad = std::asin(swingRatio);
  result.swingClamped = swingRawRatio != swingRatio;

  result.track = result.panTarget.y - horizontalReach * std::cos(swingRad);
  result.swingDeg = degrees(swingRad);
  result.boomDeg = degrees(boomRad);

  result.base = Vec3{0.0, result.track, 0.0};
  result.armTip = Vec3{
      boomLength * std::sin(swingRad) * std::cos(boomRad),
      result.track + boomLength * std::cos(swingRad) * std::cos(boomRad),
      boomLength * std::sin(boomRad),
  };

  const Vec3 levelExtensionVector = Vec3{
      geometry.extensionLength * std::sin(swingRad),
      geometry.extensionLength * std::cos(swingRad),
      0.0,
  };

  result.panCenter = add(result.armTip, levelExtensionVector);
  result.nodal = add(result.panCenter, result.offsetWorld);
  result.error = sub(result.target, result.nodal);
  result.errorLength = length(result.error);

  return result;
}

} // namespace limnmoco
