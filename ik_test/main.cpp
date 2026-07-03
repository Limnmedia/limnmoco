// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestCase {
  std::string name;
  limnmoco::VirtualPose pose;
  limnmoco::CraneGeometry geometry;
  double expectedTrack;
  double expectedSwingDeg;
  double expectedBoomDeg;
  bool checkExactAxes;
  bool expectClamp;
};

bool near(double actual, double expected, double tolerance) {
  return std::abs(actual - expected) <= tolerance;
}

void printVec(const char *label, limnmoco::Vec3 v) {
  std::cout << "  " << label << ": (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

bool runTest(const TestCase &test) {
  constexpr double kAxisTolerance = 1e-6;
  constexpr double kErrorTolerance = 1e-6;

  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(test.pose, test.geometry);

  bool ok = true;
  if (test.checkExactAxes) {
    ok = ok && near(result.track, test.expectedTrack, kAxisTolerance);
    ok = ok && near(result.swingDeg, test.expectedSwingDeg, kAxisTolerance);
    ok = ok && near(result.boomDeg, test.expectedBoomDeg, kAxisTolerance);
  }

  ok = ok && near(result.errorLength, 0.0, kErrorTolerance);
  ok = ok && ((result.boomClamped || result.swingClamped) == test.expectClamp);

  std::cout << (ok ? "[PASS] " : "[FAIL] ") << test.name << "\n";
  std::cout << std::fixed << std::setprecision(8);
  std::cout << "  track: " << result.track << "\n";
  std::cout << "  swingDeg: " << result.swingDeg << "\n";
  std::cout << "  boomDeg: " << result.boomDeg << "\n";
  std::cout << "  errorLength: " << result.errorLength << "\n";
  std::cout << "  clamped: " << ((result.boomClamped || result.swingClamped) ? "yes" : "no") << "\n";
  printVec("target", result.target);
  printVec("panTarget", result.panTarget);
  printVec("nodal", result.nodal);
  std::cout << "\n";

  return ok;
}

} // namespace

int main() {
  const limnmoco::CraneGeometry defaultGeometry{
      10.0, // boomLength
      2.8,  // extensionLength
      0.0,  // offsetX
      0.0,  // offsetY
      0.0,  // offsetZ
  };

  const std::vector<TestCase> tests = {
      {
          "Blender default panel values",
          limnmoco::VirtualPose{
              8.0, // vtrack / Y
              2.0, // vew / X
              3.0, // vheight / Z
              0.0, // vpanDeg
              0.0, // vtiltDeg
              0.0, // vrollDeg
          },
          defaultGeometry,
          -4.17623075008637,
          9.32779500976675,
          17.4576031237221,
          true,
          false,
      },
      {
          "Centered target, no offset",
          limnmoco::VirtualPose{
              12.8,
              0.0,
              0.0,
              0.0,
              0.0,
              0.0,
          },
          defaultGeometry,
          0.0,
          0.0,
          0.0,
          true,
          false,
      },
      {
          "Rotated nodal offset",
          limnmoco::VirtualPose{
              8.0,
              2.0,
              3.0,
              20.0,
              10.0,
              -5.0,
          },
          limnmoco::CraneGeometry{
              10.0,
              2.8,
              0.5,
              0.25,
              -0.1,
          },
          0.0,
          0.0,
          0.0,
          false,
          false,
      },
  };

  int failures = 0;
  for (const TestCase &test : tests) {
    if (!runTest(test)) {
      ++failures;
    }
  }

  if (failures) {
    std::cerr << failures << " IK test(s) failed.\n";
    return 1;
  }

  std::cout << "All IK tests passed.\n";
  return 0;
}
