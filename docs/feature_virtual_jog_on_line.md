# Feature Plan: Virtual Jog on Camera Line

## Purpose

Implement `MSG_VIRT_JOG_ON_LINE` so Dragonframe can jog the crane along the
camera's current local X, Y, or Z direction through the existing virtual IK and
coordinated-motion system.

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

## Questions requiring a decision

### Camera local-axis convention

Confirm the meaning of positive camera-local X, Y, and Z. In particular,
confirm whether positive local Z is forward through the lens or backward toward
the camera body.

### PAN and TILT protocol axes

The protocol does not define whether axes 3 and 4 mean direct virtual pan/tilt
rotation or a translation derived from those rotations. The initial recommended
behavior is direct virtual pan and tilt jogging. Dragonframe packet captures
should confirm this assumption. 

This initial recommendation seems to be the correct interpretation.

as an aside, it seems like it's a double covering of the behavior of VIRT_MOVE 
with the PAN or TILT axis selected

### Speed normalization

Confirm whether a magnitude of 10,000 should use the limiting participating
physical motor's configured maximum velocity. The initial implementation should
reuse the existing two-second look-ahead policy to form each coordinated
segment.

Pretty sure the protocol specifies 10,000 means maximum velocity. which as it is 
already constrained by the slowest physical participating motor for virtual jog,
i think it should be so for jog on line

## Coordinate pipeline

All line-jog translation must pass through the public Kuper virtual-coordinate
boundary. It must not bypass `KuperTrackConvention`.

```text
camera-local direction
  -> rotate by current virtual pan/tilt/roll
  -> Kuper world direction
  -> public virtual target pose
  -> public-to-solver conversion
  -> IK
  -> coordinated physical targets
```

This ensures that a forward camera-line jog reports negative `vTrack` and
commands the matching signed real-track position.

## Implementation plan

### 1. Shared camera-line direction helper

Add a shared helper in `common/LimnmocoIK` that accepts a local X, Y, or Z axis,
applies the existing virtual pan/tilt/roll rotation matrix, and returns a
normalized Kuper-world translation direction.

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

For X/Y/Z translation, orientation and unrelated virtual coordinates must remain
unchanged.

Commit point: target-generation tests only.

### 3. Define PAN and TILT behavior

After the protocol interpretation is confirmed, implement axes 3 and 4. If they
are direct rotational jogs, route them through the existing virtual pan/tilt
target machinery and add focused regression tests.

Commit point: protocol-axis behavior tests.

### 4. Add persistent M7 line-jog state

Replace the current `msg_virt_jog_on_line()` stub with state that records:

- active flag;
- requested camera-local axis;
- signed speed;
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
5. Schedule synchronized physical motion with `coordinated_motion_start()`.
6. Refresh the segment until stopped.

This must not rely on Dragonframe repeatedly sending line-jog packets.

### 6. Integrate stop and safety behavior

`MSG_VIRT_STOP` must stop every physical axis participating in the active line
jog and clear its state. A hard stop, limit, range failure, or IK failure must
also clear line-jog state and return the appropriate error.

A normal virtual move or virtual jog must supersede and cancel an active line
jog.

### 7. Preserve virtual position reporting

Virtual positions continue to come from FK. Physical compensation axes may move
during a line jog, but only the intended camera-line displacement should advance
in public virtual coordinates.

## Code locations

| Area | Files / functions |
|---|---|
| Shared direction math | `common/LimnmocoIK/src/Mat3.*`, new helper beside `KuperTrackConvention.*` |
| Host tests | `ik_test/main.cpp` |
| Coordinated segment tests | `common/LimnmocoIK/src/CoordinatedTrajectory.*`, `ik_test/coordinated_trajectory_test.cpp` |
| Protocol constants/types | `m7/dfx.h` |
| Packet dispatch | `m7/m7.ino` `MSG_VIRT_JOG_ON_LINE` handler |
| Current stub | `m7/m7.ino` `msg_virt_jog_on_line()` |
| Virtual FK/IK | `virt_kinematics()`, `virtualPoseForIk()`, `virt_inverse_kinematics()` |
| Existing virtual jog behavior | `msg_virt_jog()`, `makeCoordinatedJogAxis()`, `msg_virt_stop()` |

## Hardware acceptance tests

- Identity orientation: local X/Y/Z jogs map to expected virtual EW/NS/track
  directions.
- Pan +90 degrees: camera-line jog moves sideways in world space.
- Tilt +90 degrees: camera-line jog moves vertically.
- Positive and negative speed reverse exactly.
- `MSG_VIRT_STOP` halts every participating motor.
- Forward line jog reports negative `vTrack`.
- Direct real-axis commands and raw signed motor-position reports are unchanged.

