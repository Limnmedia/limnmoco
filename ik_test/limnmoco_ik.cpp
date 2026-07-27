// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <algorithm>
#include <cmath>

namespace limnmoco {
namespace {

constexpr float kPi = 3.14159265f;

float clamp(float value, float lo, float hi) {
  return std::max(lo, std::min(hi, value));
}

Vec3 add(Vec3 a, Vec3 b) {
  return Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 sub(Vec3 a, Vec3 b) {
  return Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

float length(Vec3 v) {
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

Mat3 rotationZ(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{c, -s, 0.0f}, {s, c, 0.0f}, {0.0f, 0.0f, 1.0f}}};
}

Mat3 rotationX(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{1.0f, 0.0f, 0.0f}, {0.0f, c, -s}, {0.0f, s, c}}};
}

Mat3 rotationY(float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return Mat3{{{c, 0.0f, s}, {0.0f, 1.0f, 0.0f}, {-s, 0.0f, c}}};
}

Mat3 rotationMatrixFromVirtualAxes(float vpanDeg, float vtiltDeg, float vrollDeg) {
  return multiply(multiply(rotationZ(radians(vpanDeg)), rotationX(radians(vtiltDeg))),
                  rotationY(radians(vrollDeg)));
}

} // namespace

float degrees(float radiansValue) {
  return radiansValue * 180.0f / kPi;
}

float radians(float degreesValue) {
  return degreesValue * kPi / 180.0f;
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

  const float boomLength = std::max(0.001f, geometry.boomLength);
  const float boomRawRatio = result.panTarget.z / boomLength;
  const float boomRatio = clamp(boomRawRatio, -1.0f, 1.0f);
  const float boomRad = std::asin(boomRatio);
  result.boomClamped = boomRawRatio != boomRatio;

  float horizontalReach = boomLength * std::cos(boomRad) + geometry.extensionLength;
  if (std::abs(horizontalReach) < 0.000001f) {
    horizontalReach = 0.000001f;
  }

  const float swingRawRatio = result.panTarget.x / horizontalReach;
  const float swingRatio = clamp(swingRawRatio, -1.0f, 1.0f);
  const float swingRad = std::asin(swingRatio);
  result.swingClamped = swingRawRatio != swingRatio;

  result.track = result.panTarget.y - horizontalReach * std::cos(swingRad);
  result.swingDeg = degrees(swingRad);
  result.boomDeg = degrees(boomRad);

  result.base = Vec3{0.0f, result.track, 0.0f};
  result.armTip = Vec3{
      boomLength * std::sin(swingRad) * std::cos(boomRad),
      result.track + boomLength * std::cos(swingRad) * std::cos(boomRad),
      boomLength * std::sin(boomRad),
  };

  const Vec3 levelExtensionVector = Vec3{
      geometry.extensionLength * std::sin(swingRad),
      geometry.extensionLength * std::cos(swingRad),
      0.0f,
  };

  result.panCenter = add(result.armTip, levelExtensionVector);
  result.nodal = add(result.panCenter, result.offsetWorld);
  result.error = sub(result.target, result.nodal);
  result.errorLength = length(result.error);

  return result;
}

VirtualPose solveForwardKinematics(const CranePositions &positions,
                                   const CraneGeometry &geometry) {
  const float boomRad = radians(positions.boom);
  const float swingRad = radians(positions.swing);

  Vec3 armTip{
      geometry.boomLength * std::sin(swingRad) * std::cos(boomRad),
      positions.track + geometry.boomLength * std::cos(swingRad) * std::cos(boomRad),
      geometry.boomLength * std::sin(boomRad),
  };
  Vec3 levelExtension{
      geometry.extensionLength * std::sin(swingRad),
      geometry.extensionLength * std::cos(swingRad),
      0.0f,
  };

  const Mat3 rotation = rotationMatrixFromVirtualAxes(
      positions.pan, positions.tilt, positions.roll);
  const Vec3 offsetWorld = multiply(
      rotation, Vec3{geometry.offsetX, geometry.offsetY, geometry.offsetZ});
  const Vec3 nodal = add(add(armTip, levelExtension), offsetWorld);

  return VirtualPose{nodal.y, nodal.x, nodal.z,
                     positions.pan, positions.tilt, positions.roll};
}

Vec3 fk(const CranePositions &pos, const CraneGeometry &geo) {
}

} // namespace limnmoco
