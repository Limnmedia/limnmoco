// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_AIM_GEOMETRY_H_
#define LIMNMOCO_AIM_GEOMETRY_H_

namespace limnmoco {

// Kuper world coordinates: X=vEW, Y=vNS (vertical), and Z=vTrack.
struct KuperPoint {
  float x;
  float y;
  float z;
};

// Virtual camera orientation. vPAN and vTILT may be aim-relative offsets;
// vROLL remains independently controlled.
struct AimOrientation {
  float vpanDeg;
  float vtiltDeg;
  float vrollDeg;
};

// Derives the zero-offset camera orientation that points from nodalPoint toward
// aimPoint. The result uses the existing virtual rotation convention and keeps
// vrollDeg unchanged. Returns false for a non-finite point or a direction with
// no horizontal component, whose pan is undefined.
bool aim_base_orientation(const KuperPoint &nodalPoint,
                          const KuperPoint &aimPoint, float vrollDeg,
                          AimOrientation *output);

// Applies virtual PAN/TILT offsets relative to an aim-base orientation. Pan is
// normalized to [-180, 180); roll is preserved unchanged.
bool aim_apply_relative_offsets(const AimOrientation &base, float panOffsetDeg,
                                float tiltOffsetDeg, AimOrientation *output);

// Returns true only when nodalPoint is strictly outside the vertical cylinder
// centered on aimPoint with the given radius. Kuper Y is vertical, so only X
// and Z contribute to the radial distance.
bool aim_point_outside_safe_cylinder(const KuperPoint &nodalPoint,
                                    const KuperPoint &aimPoint, float radius);

} // namespace limnmoco

#endif // LIMNMOCO_AIM_GEOMETRY_H_
