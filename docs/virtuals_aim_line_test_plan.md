# Virtual Aim Point and Jog-on-Line: Test Plan

Tests are grouped from pure shared math through firmware behavior and hardware
acceptance. Each feature should be committed only after the tests for its layer
pass.

## Shared-library unit tests

### Public coordinates and camera-line direction

- [ ] Identity orientation: Kuper local X, Y, and Z produce EW, NS, and track
  public translation deltas respectively.
- [ ] Positive and negative 90-degree pan rotate each local translation axis
  into the expected world direction.
- [ ] Positive and negative 90-degree tilt cases.
- [ ] Roll cases, including a combined pan/tilt/roll orientation.
- [ ] Positive and negative line-jog speed reverse exactly.
- [ ] Camera-local optical `-Z` maps to Kuper forward / negative `vTrack` at
  identity orientation.
- [ ] Verify a rotated camera line changes the expected combination of public
  `vEW`, `vNS`, and `vTrack` components.

### Aim geometry

- [ ] Cardinal aim directions, including Kuper forward and away.
- [ ] Aim point directly above and below the nodal point.
- [ ] Preserve roll while deriving aim pan/tilt.
- [ ] Reject a zero-length aim vector.
- [ ] Reject nodal positions at and inside the vertical SafeDistance cylinder;
  accept one just outside it.
- [ ] Target motion approaching the aim-point cylinder is rejected before it
  crosses the boundary.
- [ ] Pan wrap-boundary cases select the documented continuous solution.
- [ ] Derived pan/tilt are finite and correctly rejected when unreachable.

### FK/IK and BCT integration

- [ ] Aim-derived target solves through FK/IK to the intended nodal point.
- [ ] With a BCT, geometric boom angle maps to the expected raw physical boom
  steps; no SPU or fixed-point factor is applied twice.
- [ ] No-roll configuration uses zero roll and never creates a roll plan.
- [ ] Existing FK-origin normalization and Kuper-track tests remain unchanged.

## Coordinated-motion tests

- [ ] Replanning an active line-jog horizon preserves current physical
  positions and velocities within numerical tolerance.
- [ ] Repeated replans maintain a continuous velocity direction; no reset to
  zero occurs at each horizon boundary.
- [ ] Direction reversal has the configured deceleration/reversal behavior.
- [ ] A target at a motor soft limit is rejected before a profile begins.
- [ ] A limit/range/IK failure during a continuing jog cancels active state and
  stops every participating motor.
- [ ] Long simulated jogs do not accumulate material orthogonal nodal drift.

## M7 protocol and state tests

### Aim point

- [ ] Signed fixed-point parsing and echo for positive and negative aim
  coordinates.
- [ ] Failed aim enable leaves the previous aim configuration and active motion
  unchanged.
- [ ] Valid aim enable persists across a normal virtual translation and a stop.
- [ ] Disable refreshes FK and retains the achieved pan/tilt orientation.
- [ ] Virtual PAN/TILT moves and jogs apply relative offsets while aim is
  enabled.
- [ ] Virtual roll remains accepted while aim is enabled when roll exists.
- [ ] New virtual configuration clears aim state.
- [ ] `VIRT_GET_POSITION` returns base virtual positions plus aim extensions
  according to the final protocol compatibility rule.

### Jog on line

- [ ] Parse positive and negative signed SPEED values; reject zero, out of
  range magnitudes, and `INT16_MIN` according to the final contract.
- [ ] `+/-10000` uses the feasible coordinated maximum constrained by the
  slowest participating physical motor.
- [ ] PAN/TILT line-jog axes apply relative aim offsets while aim is enabled.
- [ ] A line jog records its stop axis: local X -> EW, local Y -> NS, local Z
  -> track, PAN -> pan, and TILT -> tilt.
- [ ] `VIRT_STOP` for that recorded logical axis cancels the line jog and stops
  every participating physical motor; a STOP for a different virtual axis
  retains normal axis-specific behavior.
- [ ] A normal virtual move, normal virtual jog, and reconfiguration each
  supersede and clear active line-jog state.
- [ ] RT playback supersedes active line motion without re-aiming uploaded
  physical RT targets.
- [ ] Asynchronous jog failure produces the defined status/fault notification.

## Hardware acceptance tests

- [ ] At identity orientation, local X/Y/Z line jogs move the expected virtual
  world directions.
- [ ] At pan +90 degrees, a camera-forward jog moves sideways in world space.
- [ ] At tilt +90 degrees, camera-forward jog moves vertically in world space.
- [ ] Positive and negative speeds reverse physical and reported nodal motion.
- [ ] During a long line jog, motion is continuous across horizons and does not
  visibly pause or pulse.
- [ ] Aim compensation does not bend the camera line captured when the jog
  began.
- [ ] `VIRT_STOP` halts every motor participating in the current line jog.
- [ ] Forward line jog reports negative `vTrack` at identity orientation.
- [ ] With a laser reference, verify the nodal point follows the intended line
  while track/swing/boom/pan compensation motors move as needed.
- [ ] Enable an aim point ahead of the crane; translate in vTrack, EW, and NS
  while the camera remains aimed at the fixed reference.
- [ ] Repeat aimed translation with a nonzero nodal offset.
- [ ] Verify aim-point safe-cylinder rejection causes no physical movement.
- [ ] Confirm virtual PAN/TILT produce relative aim offsets and roll is
  available when installed.
- [ ] Confirm all virtual behavior works on the current crane with no roll
  motor.
- [ ] Confirm direct real-axis commands behave unchanged after exercising both
  virtual features.
