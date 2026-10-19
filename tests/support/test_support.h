// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_TEST_SUPPORT_H_
#define LIMNMOCO_TEST_SUPPORT_H_

#include <string>

#ifndef LIMNMOCO_TEST_FIXTURE_DIR
#error "LIMNMOCO_TEST_FIXTURE_DIR must be set by the host test target"
#endif

namespace limnmoco::test {

inline constexpr float kTranslationToleranceMm = 0.0001f;
inline constexpr float kRotationToleranceDegrees = 0.0002f;
inline constexpr float kTrajectoryTolerance = 0.00001f;

inline std::string fixture_path(const std::string &name) {
  return std::string(LIMNMOCO_TEST_FIXTURE_DIR) + "/" + name;
}

} // namespace limnmoco::test

#endif // LIMNMOCO_TEST_SUPPORT_H_
