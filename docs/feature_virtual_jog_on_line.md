# Feature Plan: Virtual Jog on Camera Line

## Purpose

Implement `MSG_VIRT_JOG_ON_LINE` so Dragonframe can jog the crane along a
camera-relative X, Y, or Z line through the existing virtual IK and
coordinated-motion system. The effective camera orientation is captured when
the jog begins and retained until the jog stops.

The feature must preserve the Kuper convention:

```text
Forward             = negative Z / negative vTrack
Away from subject   = positive Z / positive vTrack
```

## Protocol contract

`MSG_VIRT_JOG_ON_LINE [0x0206]` contains:

```text
AXIS   BYTE   0=X, 1=Y, 2=Z (camera line), 3=PAN, 4=TILT
SPEED  WORD   signed direction and magnitude; 1=inching, 10000=maximum jog
```

The speed value must be interpreted as signed `int16_t`: its sign chooses
direction and its magnitude selects speed. A line-jog command has no endpoint,
so the intended behavior is continuous motion until `MSG_VIRT_STOP`.

## Confirmed semantics

- Kuper X is vEW, Kuper Y is vNS, and Kuper Z is vTrack.
- Negative Z / negative vTrack is forward, toward the set. Camera-local `-Z`
  is the optical forward direction.
- The X/Y/Z line is captured from the effective camera orientation at jog
  start. Aim compensation may later rotate pan/tilt, but it must not bend that
  captured translation line.
- A magnitude of 10,000 uses the maximum feasible coordinated speed, limited by
  the slowest participating physical motor.
- PAN and TILT jog-on-line axes change virtual pan/tilt. When aim is active,
  those are relative offsets from the aim direction; otherwise they are direct
  virtual camera rotations.

## Coordinate pipeline

All line-jog translation must pass through the public Kuper virtual-coordinate
boundary. It must not bypass `KuperTrackConvention`.

```text
captured camera-local X/Y/Z direction
  -> rotate once by effective virtual pan/tilt/roll
  -> Kuper world direction
  -> public virtual target pose
  -> if aim enabled: derive target aim-base pan/tilt and apply vPAN/vTILT offsets
  -> public-to-solver conversion
  -> IK
  -> coordinated physical targets
```

This ensures that a forward camera-line jog reports negative `vTrack` and
commands the matching signed real-track position.

## Implementation plan

### 1. Shared camera-line direction helper

Add a shared helper in `common/LimnmocoIK` that accepts a Kuper X, Y, or Z
axis, applies the effective virtual pan/tilt/roll rotation matrix once, and
returns a normalized public-world translation direction.

Required host tests:

- identity orientation;
- positive and negative 90-degree pan;
- positive and negative 90-degree tilt;
- roll;
- reversed signed speed;
- Kuper forward/track sign.

Commit point: shared math and tests only.

### 2. Shared virtual target-generation helper

Add a pure helper that receives the current public virtual pose, camera-local
axis, signed normalized speed, and look-ahead distance. It returns a new public
virtual pose along the camera line.

For X/Y/Z translation, the captured line stays fixed. When aim is disabled,
orientation remains unchanged; when aim is enabled, target pan/tilt are
re-derived from the aim point while preserving their relative offsets.

Commit point: target-generation tests only.

### 3. Define PAN and TILT behavior

Implement axes 3 and 4 through the virtual pan/tilt target machinery. With aim
enabled they change relative offsets from aim; without aim they are direct
virtual rotations.

Commit point: protocol-axis behavior tests.

### 4. Add persistent M7 line-jog state

Replace the current `msg_virt_jog_on_line()` stub with state that records:

- active flag;
- requested Kuper camera-local axis;
- signed speed;
- captured effective orientation;
- mapped `MSG_VIRT_STOP` axis (X -> EW, Y -> NS, Z -> Track, PAN -> PAN,
  TILT -> TILT);
- last target or refresh time;
- active physical axes.

The `MSG_VIRT_JOG_ON_LINE` dispatcher must parse the signed speed correctly and
return the handler's real error result rather than unconditional `OK`.

### 5. Schedule continuous coordinated look-ahead segments

While line jog is active:

1. Refresh virtual pose from physical positions using FK.
2. Generate a short public virtual target on the camera line.
3. Convert through `virtualPoseForIk()` and solve IK.
4. Convert solver track with `solver_track_to_kuper()`.
5. Hand off synchronized physical motion through a velocity-preserving
   coordinated replan API.
6. Refresh the horizon until stopped.

This must not rely on Dragonframe repeatedly sending line-jog packets.

### 6. Integrate stop and safety behavior

`MSG_VIRT_STOP` for the mapped logical axis must stop every physical axis
participating in the active line jog and clear its state. A hard stop, limit,
range failure, or IK failure must also clear line-jog state and emit the defined
asynchronous fault/status event.

A normal virtual move or virtual jog must supersede and cancel an active line
jog.

### 7. Preserve virtual position reporting

Virtual positions continue to come from FK. Physical compensation axes may move
during a line jog, and a rotated camera line can legitimately change multiple
public translation components. The invariant is straight nodal travel along the
captured camera line.

## Code locations

| Area | Files / functions |
|---|---|
| Shared direction math | `common/LimnmocoIK/src/Mat3.*`, new helper beside `KuperTrackConvention.*` |
| Host tests | `tests/ik/main.cpp` |
| Coordinated segment tests | `common/LimnmocoIK/src/CoordinatedTrajectory.*`, `tests/trajectory/coordinated_trajectory_test.cpp` |
| Protocol constants/types | `m7/dfx.h` |
| Packet dispatch | `m7/m7.ino` `MSG_VIRT_JOG_ON_LINE` handler |
| Current stub | `m7/m7.ino` `msg_virt_jog_on_line()` |
| Virtual FK/IK | `virt_kinematics()`, `virtualPoseForIk()`, `virt_inverse_kinematics()` |
| Existing virtual jog behavior | `msg_virt_jog()`, `makeCoordinatedJogAxis()`, `msg_virt_stop()` |

## Hardware acceptance tests

- Identity orientation: X/Y/Z jogs map to vEW/vNS/vTrack
  directions.
- Pan +90 degrees: camera-line jog moves sideways in world space.
- Tilt +90 degrees: camera-line jog moves vertically.
- Positive and negative speed reverse exactly.
- `MSG_VIRT_STOP` halts every participating motor.
- Forward line jog reports negative `vTrack`.
- Aim compensation does not bend a captured camera line.
- Direct real-axis commands and raw signed motor-position reports are unchanged.
