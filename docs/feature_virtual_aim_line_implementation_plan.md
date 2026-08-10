# Feature Plan: Virtual Aim Point and Jog on Camera Line

## Objective

Implement `MSG_VIRT_AIM_POINT` and `MSG_VIRT_JOG_ON_LINE` incrementally,
with a passing test suite and a discrete regression commit after each stage.

Suggested branch: `virtuals-aim-line`

## Confirmed behavior

- Public virtual axes use the Kuper convention: vEW is X, vNS is Y, and
  vTrack is Z. Negative Z / negative vTrack is forward, toward the set.
- Camera-local `-Z` is the optical forward direction.
- Jog on camera line captures the camera orientation when the jog begins and
  follows that fixed line until stopped.
- There is one persistent aim point.
- When aim is active, vPAN and vTILT are offsets relative to the direction from
  the target nodal point to the aim point. Zero vPAN/vTILT means point directly
  at the aim point.
- vROLL remains independent.
- Aim safe distance is a vertical cylinder around the aim point, not a sphere.
- `MSG_VIRT_STOP` identifies a normal virtual axis. A line jog maps its axis to
  a stop axis: X -> EW, Y -> NS, Z -> Track, PAN -> PAN, TILT -> TILT.

## Stage 1: Align the feature documentation

Update the existing aim-point and jog-on-line feature documents plus the shared
considerations and test-plan documents to reflect the confirmed behavior above.

Verification:

- Review all four documents for a consistent axis convention, aim offset
  semantics, cylinder safety model, and stop-axis mapping.

Commit:

`docs: align virtual aim and line-jog behavior with Dragonframe`

## Stage 2: Add shared camera-line coordinate helpers

Add a pure shared-library module in `common/LimnmocoIK/src/` for public Kuper
translation deltas and camera-line orientation.

The module must:

- use the existing virtual rotation order:
  `Rz(pan) * Rx(tilt) * Ry(roll)`;
- return public virtual translation deltas, not solver-private coordinates;
- keep Kuper track-sign conversion in one defined boundary;
- transform camera-local X/Y/Z into a normalized fixed world direction.

Write host tests before implementation:

- identity: X -> EW, Y -> NS, Z -> Track;
- negative Z -> forward / negative vTrack;
- positive and negative 90-degree pan and tilt;
- roll and combined rotations;
- normalized result and reversed direction.

Commit:

`feat(ik): add tested camera-line coordinate helpers`

## Stage 3: Add shared aim-point geometry

Add pure shared helpers for:

- deriving aim-base pan/tilt from target nodal point and fixed aim point;
- applying vPAN/vTILT relative offsets;
- preserving vROLL;
- validating the vertical aim safe cylinder;
- choosing/documenting pan-angle wrapping.

Write host tests before implementation:

- cardinal aim directions, including Kuper forward and away;
- aim point above and below the nodal point;
- zero PAN/TILT points directly at aim;
- nonzero PAN/TILT offsets from aim direction;
- unchanged roll;
- cylinder boundary, inside, and outside;
- invalid/degenerate aim direction and wrap-boundary cases.

Commit:

`feat(ik): add tested aim-point geometry`

## Stage 4: Build transactional aim-aware virtual targets

Add a pure target builder. It receives current public virtual pose, a requested
translation or rotation, optional aim state, and geometry. It returns a fully
validated target pose without mutating firmware state.

When aiming is active, it derives aim-base pan/tilt from the target nodal
position, then applies requested vPAN/vTILT offsets.

Write host tests before implementation:

- normal targets are unchanged with aim disabled;
- vTrack, EW, and NS targets retain aim;
- PAN/TILT act as relative offsets;
- invalid targets return failure without a partial pose;
- absent-roll configuration stays at zero roll.

Commit:

`feat(ik): build transactional aim-aware virtual targets`

## Stage 5: Add M7 aim-point protocol state

Replace the aim-point stub with typed, validated state.

Implementation requirements:

- parse AIM-X/Y/Z as signed `int32_t / 1000.0f`;
- validate aim state against current FK before enabling;
- echo exact signed fixed-point values in the response;
- accept an unsolicited aim-point packet as a state confirmation/update;
- clear aim atomically during a new virtual configuration;
- append aim state to `MSG_VIRT_GET_POSITION` according to the protocol;
- advertise Aim Point capability only after the full behavior is complete.

Write firmware/parser tests before implementation:

- positive and negative parse/echo fixtures;
- enable, disable, and unsolicited update;
- failed enable leaves prior aim and active motion unchanged;
- absent-roll configuration.

Commit:

`feat(m7): add validated virtual aim-point state`

## Stage 6: Apply aim to normal virtual moves and jogs

Refactor `msg_virt_move()` and `msg_virt_jog()` to:

1. refresh FK;
2. construct a temporary aim-aware target;
3. solve IK, BCT conversion, and motor-limit validation;
4. schedule coordinated motion;
5. commit virtual target state only on success.

This preserves the current physical motion if target validation fails.

Write tests before implementation:

- aimed vTrack, EW, and NS moves;
- relative PAN/TILT offsets while aiming;
- independent roll where installed;
- BCT and no-roll regression;
- failed IK/range/limit does not alter virtual reporting or motor targets.

Commit:

`feat(m7): apply aim point to virtual moves and jogs`

## Stage 7: Add velocity-preserving coordinated replanning

Continuous line jogging must not repeatedly call the existing
`coordinated_motion_start()` while active, because that resets velocities.

Add a coordinated-motion handoff/replan API that starts from actual current
position and velocity and creates the next finite horizon without a velocity
discontinuity.

Write trajectory tests before implementation:

- profile handoff preserves position and velocity;
- repeated handoffs do not create zero-velocity pulses;
- reversal decelerates and reverses safely;
- soft limit is rejected before scheduling;
- cancellation stops all participating motors.

Commit:

`feat(motion): support continuous coordinated replanning`

## Stage 8: Add pure jog-on-line target generation

Add a pure helper that:

- captures effective camera orientation at jog start;
- converts requested X/Y/Z into a fixed public virtual-world direction;
- creates finite look-ahead nodal targets;
- when aim is active, recalculates aim-base pan/tilt for each target while
  retaining vPAN/vTILT offsets;
- handles PAN/TILT jog-on-line axes as relative rotational offsets during aim.

Write host tests before implementation:

- repeated targets follow one fixed line;
- aimed movement preserves relative PAN/TILT offsets;
- aim compensation does not rotate the captured translation line;
- speed direction reverses target direction;
- target reaching the aim cylinder is rejected.

Commit:

`feat(ik): add tested fixed-orientation line-jog targets`

## Stage 9: Add M7 jog-on-line command state

Replace `msg_virt_jog_on_line()` with persistent state containing:

- active flag;
- requested local axis;
- signed `int16_t` speed;
- captured camera orientation;
- mapped stop axis;
- current horizon target;
- participating physical motor set.

Protocol requirements:

- parse SPEED as `int16_t`;
- accept magnitudes `1..10000`;
- reject zero and `INT16_MIN`;
- use the maximum feasible coordinated speed constrained by the limiting motor.

Write parser/state tests before implementation:

- signed speed values and invalid edge cases;
- X/EW, Y/NS, Z/Track, PAN/PAN, and TILT/TILT stop mapping;
- aim-aware versus non-aim-aware startup;
- no-roll configuration.

Commit:

`feat(m7): add virtual jog-on-line command state`

## Stage 10: Execute continuous jog-on-line and arbitrate commands

Run line-jog planning from the M7 motion update path using the
velocity-preserving coordinated handoff.

For every horizon:

1. refresh FK;
2. create and validate the next target;
3. validate aim cylinder, IK, BCT, and motor limits;
4. hand off to the next coordinated profile.

On an asynchronous failure after the initial ACK:

- clear line-jog state;
- stop every participating physical motor;
- emit a defined asynchronous fault/status event.

Arbitration requirements:

- mapped `MSG_VIRT_STOP` cancels the active line jog;
- normal virtual move/jog supersedes line jog;
- virtual reconfiguration clears aim and line jog;
- RT playback supersedes line jog but does not reinterpret its uploaded physical
  targets through aim geometry.

Write firmware integration tests before implementation:

- matching and non-matching STOP axes;
- aim/no-aim line jog;
- physical limit, IK, BCT range, and aim-cylinder failure;
- normal virtual move/jog and RT supersession;
- long simulated jog with no material orthogonal nodal drift.

Commit:

`feat(m7): execute continuous virtual jog-on-line`

## Stage 11: Hardware acceptance

Run and record the following on the crane:

- identity, pan-rotated, and tilt-rotated camera-line jogs;
- positive and negative direction;
- laser-verified straight nodal travel;
- long jog continuity across profile handoffs;
- aimed vTrack, EW, and NS translations;
- relative PAN/TILT offsets while aiming;
- safe-cylinder rejection with no physical movement;
- mapped STOP behavior and reversal;
- current no-roll crane configuration;
- direct real-axis motion unchanged after virtual feature use.

Commit:

`test: record aim point and camera-line hardware acceptance`
