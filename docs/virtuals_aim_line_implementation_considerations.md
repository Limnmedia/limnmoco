# Virtual Aim Point and Jog-on-Line: Implementation Checklist

This document consolidates the decisions and constraints shared by
`MSG_VIRT_AIM_POINT` and `MSG_VIRT_JOG_ON_LINE`.

## Scope and command ownership

- [ ] Keep aim point as persistent virtual configuration state: enabled flag and
  signed, scaled Kuper-world point.
- [ ] Treat an active line jog as persistent motion state: local axis, signed
  requested speed, and participating physical motors.
- [ ] A normal virtual move or virtual jog supersedes and cancels a line jog.
- [ ] A virtual reconfiguration clears aim and cancels line jog atomically.
- [ ] Starting RT playback supersedes active virtual motion. It must not cause
  M7 to re-aim Dragonframe's already physical RT trajectory. Preserve the aim
  setting for the next virtual command unless the protocol establishes a
  different session rule.
- [ ] `MSG_VIRT_STOP` already identifies the logical virtual axis with
  `DMC_VIRT_TRACK`, `DMC_VIRT_EW`, `DMC_VIRT_NS`, `DMC_VIRT_PAN`,
  `DMC_VIRT_TILT`, or `DMC_VIRT_ROLL`. Store the corresponding stop axis when
  line jog begins: local X -> EW, local Y -> NS, local Z -> track, PAN -> pan,
  and TILT -> tilt. A STOP for that axis must cancel the line jog and stop every
  participating physical motor. Other virtual STOP commands retain their
  normal axis-specific behavior.

## Coordinate and rotation contract

- [ ] Define one public-coordinate helper for Kuper-world translation deltas.
  It must be the only owner of Kuper track-sign conversion.
- [ ] Use the jog-on-line axis contract directly: Kuper X is EW, Kuper Y is
  NS, and Kuper Z is track. Negative Kuper Z is forward.
- [ ] Preserve the public convention: forward is negative Kuper Z and negative
  `vTrack`; away from subject is positive Kuper Z and positive `vTrack`.
- [ ] Use camera-local `-Z` as the confirmed optical forward direction.
- [ ] Use the solver's existing rotation order exactly:
  `Rz(virtual pan) * Rx(virtual tilt) * Ry(virtual roll)`.
- [ ] Keep degrees at the shared-library API boundary; convert to radians only
  inside matrix calculations.
- [ ] Make the shared camera-line helper return a **public virtual translation
  delta**, not an ambiguously named internal-world vector.
- [ ] Document the mapping between solver axes (`X=EW`, `Y=track`, `Z=NS`) and
  public virtual coordinates. Do not duplicate a sign conversion in M7.

## Aim-point behavior

- [ ] Parse `AIM-X/Y/Z` as signed `int32_t / 1000.0f`; echo the same signed,
  scaled values.
- [ ] Aim from the virtual nodal point, not merely the pan-axis center.
- [ ] For each requested translation target, calculate
  `aimPoint - targetNodalPosition`, then derive target virtual pan and tilt.
- [ ] Preserve virtual roll while aiming.
- [ ] While aim is enabled, treat virtual PAN and TILT moves/jogs as relative
  offsets from the aim direction. Apply the same policy to jog-on-line PAN/TILT
  axes; virtual roll remains independent.
- [ ] When aim is disabled, first refresh FK and retain the currently achieved
  pan/tilt orientation.
- [ ] Define pan-angle wrapping and choose the reachable/shortest physical
  solution consistently.
- [ ] Validate that derived pan and tilt are finite and inside physical soft
  limits before scheduling motion.

## Safe-distance and failure behavior

- [ ] Reject an aim point when the nodal point is at or inside the vertical
  `SafeDistance` cylinder around it.
- [ ] Repeat that cylinder test for every target pose, including each line-jog
  look-ahead target. Do not permit a move to enter or cross the cylinder.
- [ ] Evaluate aim geometry, IK, BCT conversion, physical limits, and
  coordinated scheduling using temporary target state.
- [ ] Commit `_virtual` target state only after all validation and scheduling
  succeeds. A rejected command must not change reported virtual intent or an
  already active move.
- [ ] For failures discovered after a continuous line-jog ACK, cancel line-jog
  state, stop all participating motors, and emit a defined asynchronous
  fault/status event. The original ACK cannot report a later failure.

## Jog-on-line protocol and motion

- [ ] Read line-jog SPEED as signed `int16_t`, not `uint16_t`.
- [ ] Accept signed magnitudes `1..10000`; define zero as either rejected or a
  stop command, and reject `INT16_MIN` without taking its absolute value.
- [ ] Interpret `+/-10000` as the maximum feasible coordinated velocity,
  constrained by the limiting participating physical motor.
- [ ] Interpret jog-on-line X/Y/Z as Kuper X/EW, Y/NS, and Z/track before
  rotating that requested camera-local direction into its public virtual delta.
- [ ] Capture the effective orientation at line-jog start. Keep its translation
  direction fixed until STOP, even when aim compensation changes pan/tilt.
- [ ] A rotated camera line legitimately changes multiple public translation
  components. The invariant is straight nodal travel along the requested
  camera line, not that only one public coordinate changes.
- [ ] Do not repeatedly call the current `coordinated_motion_start()` while it
  is active: it resets velocities to zero and replaces the profile.
- [ ] Add a velocity-preserving coordinated replan/handoff API before
  continuous line jog. Replan from the current physical position and velocity
  into the next finite horizon without discontinuity.

## Reporting and configuration

- [ ] Continue deriving public virtual position from FK. Physical compensation
  may move several motors; FK remains the source of truth for nodal pose and
  effective orientation.
- [ ] Append aim state and point to `MSG_VIRT_GET_POSITION` only according to
  the protocol's optional-field/capability rules; retain compatibility with
  clients that parse only the base response.
- [ ] Ensure aim works with an absent roll motor by treating roll as fixed
  zero and omitting it from physical coordinated plans.
- [ ] Keep raw real-axis commands unchanged; virtual coordinate conversions
  are confined to virtual command paths.

## Primary implementation locations

| Concern | Location |
|---|---|
| Shared public-coordinate, line, and aim geometry | New module in `common/LimnmocoIK/src/` |
| Existing orientation math and FK/IK | `common/LimnmocoIK/src/LimnmocoIK.cpp` |
| Track-sign conversion | `common/LimnmocoIK/src/KuperTrackConvention.*` |
| Host tests | `tests/ik/main.cpp` |
| Protocol parsing and virtual state | `m7/m7.ino`, `m7/dfx.h`, `m7/dmc_msg.h` |
| Virtual FK/IK target path | `virt_kinematics()`, `virtualPoseForIk()`, `virt_inverse_kinematics()` |
| Virtual move/jog/stop arbitration | `msg_virt_move()`, `msg_virt_jog()`, `msg_virt_stop()` |
| Line-jog state and dispatch | `msg_virt_jog_on_line()` and main packet dispatch |
| Coordinated replan support | `m7/coordinated_motion.*` and `common/LimnmocoIK/src/CoordinatedTrajectory.*` |
