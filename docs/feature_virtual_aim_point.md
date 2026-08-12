# Feature Plan: Virtual Aim Point

## Purpose

Implement `MSG_VIRT_AIM_POINT` so Dragonframe can configure a fixed
world-space point that the camera continues to face while the crane moves in
virtual space.

The implementation must preserve Kuper coordinates:

```text
Forward             = negative Z / negative vTrack
Away from subject   = positive Z / positive vTrack
```

## Required behavior

When aim point is enabled, the vector from the camera nodal point to the aim
point defines the base camera direction. vPAN and vTILT are relative offsets
from that direction; virtual roll remains independently controlled.

The feature must work with virtual moves and virtual jogs. It must also be
compatible with the planned `MSG_VIRT_JOG_ON_LINE` feature.

## Protocol contract

`MSG_VIRT_AIM_POINT [0x0207]` contains:

```text
ENABLE  BYTE
AIM-X   signed DWORD, X * 1000
AIM-Y   signed DWORD, Y * 1000
AIM-Z   signed DWORD, Z * 1000
```

The response echoes the same data. Once aim support is complete,
`MSG_VIRT_GET_POSITION` must append the optional aim enabled and aim point
fields specified by the protocol.

## Confirmed convention and operator behavior

- Camera-local `-Z` points through the lens.
- Aim direction originates at the configured optical/nodal point.
- Kuper X is vEW, Kuper Y is vNS, and Kuper Z is vTrack; negative Z is forward.
- While aim is enabled, zero vPAN/vTILT points directly at aim. Nonzero values
  are relative pan/tilt offsets from the aim direction.
- Only one aim point is active. Dragonframe can send updated aim points over
  time as aim keyframes change.
- `SafeDistance` defines the radius of a vertical cylinder around the aim
  point. The nodal point must remain outside that cylinder.

## Coordinate pipeline

```text
Dragonframe aim point, Kuper world coordinates
  -> convert Kuper Z to solver-local boom-forward coordinate
  -> vector from target camera nodal point to aim point
  -> derive aim-base virtual pan and tilt
  -> apply requested relative vPAN/vTILT offsets
  -> preserve virtual roll
  -> existing IK
  -> coordinated physical targets
```

Because the virtual pose represents the nodal point, aiming can use:

```text
direction = aimPoint - virtualNodalPosition
```

This avoids a circular dependency from pan/tilt-axis offsets.

## Implementation plan

### 1. Shared aim-geometry helpers

Add shared library functions for:

- Kuper-world point to solver-local point conversion;
- normalized direction from nodal point to aim point;
- aim-base pan/tilt extraction from the direction;
- relative pan/tilt offset application;
- vertical safe-cylinder validation.

Tests must cover cardinal directions, Kuper forward, roll preservation, and
invalid zero-length vectors.

Commit point: pure shared math and tests.

### 2. Correct M7 protocol parsing and storage

Replace the current raw aim-point storage with signed fixed-point conversion:

```text
signed int32 / 1000.0
```

Store typed floating-point Kuper-world coordinates and enable state. Return the
same properly scaled signed values in the aim-point response.

Commit point: protocol parse/echo regression coverage.

### 3. Add aim-point state transitions

- Enable: validate the point is outside the safe cylinder at the current nodal
  position.
- Disable: preserve the current virtual orientation and clear active aiming.
- New virtual configuration: initially clear aim state for safety, unless the
  protocol/session behavior establishes a different requirement.

### 4. Apply aim orientation to virtual targets

Before IK, when aim is enabled:

1. Start with requested virtual translation.
2. Calculate target nodal position.
3. Derive aim-base virtual pan/tilt toward the aim point.
4. Apply requested vPAN/vTILT relative offsets and preserve virtual roll.
5. Pass the resulting pose through `virtualPoseForIk()` and existing IK.

This must be used consistently by `MSG_VIRT_MOVE` and `MSG_VIRT_JOG`.

Commit point: shared and M7 integration tests showing a translating camera
continues to face a fixed aim point.

### 5. Define manual rotation interaction

Direct virtual pan and tilt commands remain available while aim is active; they
modify relative offsets from the aim direction. Virtual roll remains available
and independent while aiming is active.

### 6. Integrate camera-line jogging

For planned `MSG_VIRT_JOG_ON_LINE`, capture the effective camera orientation
when jogging begins. Recompute aim-base pan/tilt for each look-ahead target,
but retain vPAN/vTILT offsets and do not bend the captured translation line.

Aim-point support can be implemented and validated with normal virtual moves
before line jogging exists.

### 7. Report aim state and capability

Append optional aim fields to `MSG_VIRT_GET_POSITION`. Advertise aim-point
capability only after all behavior, reporting, and safety requirements are
complete.

### 8. Safety and failure behavior

- Reject aim points or target nodal positions at or inside the safe cylinder.
- Reject unreachable derived pan/tilt or IK targets.
- Do not alter an active physical move if a new aim configuration fails.
- `MSG_VIRT_STOP` stops motion but does not necessarily disable aim; aim remains
  configured until explicitly disabled or virtuals are reconfigured.

## Code locations

| Area | Files / functions |
|---|---|
| Shared aiming math | new helper in `common/LimnmocoIK/src/` |
| Shared tests | `tests/ik/main.cpp` |
| Kuper Z conversion | `common/LimnmocoIK/src/KuperTrackConvention.*` |
| Protocol constants/scaling | `m7/dfx.h` |
| Aim packet parser/response | `m7/m7.ino` `MSG_VIRT_AIM_POINT` handler |
| Current aim stub | `m7/m7.ino` `msg_virt_aim_point()` |
| Virtual position response | `m7/m7.ino` `msg_virt_get_position()` |
| Target orientation injection | `virtualPoseForIk()`, `virt_inverse_kinematics()`, `msg_virt_jog()` |
| Future line-jog interaction | `msg_virt_jog_on_line()` |

## Hardware acceptance tests

- Enable an aim point ahead of the crane; translate virtual track, EW, and NS
  while the camera remains aimed at it.
- Verify pan/tilt compensation is smooth and roll remains unchanged.
- Verify virtual PAN/TILT act as relative offsets while aiming.
- Test positive and negative Kuper Z aim points.
- Test an aim point or target inside the safe cylinder and confirm no motion
  begins.
- Verify `VIRT_GET_POSITION` and aim-point echo use correct signed `* 1000`
  values.
