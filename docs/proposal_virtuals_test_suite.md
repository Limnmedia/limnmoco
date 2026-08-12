# Proposal: Virtuals Test Suite

## Objective

Create a layered, repeatable test framework for virtual-crane behavior.  The
framework must validate pure kinematics first, then firmware protocol and
motion integration, then retain a concise hardware acceptance procedure for
the physical crane.

The existing `tests/` directory is the first completed layer. Its former
`ik_test/` name no longer reflected its scope: the current suite covers boom
compensation, Kuper coordinate conventions, aim-point geometry, camera-line
targets, and coordinated trajectory primitives. Future M7 parser and motion
simulation coverage belongs alongside—not inside—these IK-only tests.

## Current and target layout

The completed mechanical rename to `tests/` preserved existing test behavior
and organizes the suite by test boundary:

```text
tests/
  ik/                 Shared-library IK/FK, BCT, coordinate, aim, and line math
  fixtures/           Versioned CSV fixtures shared by host tests
  trajectory/         Pure coordinated-profile and handoff tests
  m7_protocol/        Planned: DMC packet parser/serializer and virtual-state tests
  m7_simulation/      Planned: firmware-facing virtual motion simulation tests
  hardware/           Planned: repeatable real-crane acceptance procedures and records
  support/            Host adapters, assertions, fixture readers, tolerances
```

Each new automated case belongs in a subject-specific `*_test.cpp` within its
boundary directory. Reusable fixture and assertion utilities belong in
`support/`; no aggregate test implementation file is retained.

## Test layers

### 1. Pure shared-library tests

These host tests compile only code from `common/LimnmocoIK`.  They must remain
fast, deterministic, and independent of Arduino APIs.

They cover:

- vector/matrix and coordinate-boundary math;
- FK and IK reconstruction;
- Kuper track convention and FK-origin normalization;
- boom compensation table validation, interpolation, and raw-step mapping;
- aim-point geometry and safe-cylinder validation;
- aim-aware virtual targets;
- fixed camera-line target generation;
- coordinated trajectory shape and handoff math.

### 2. Basic virtual-move regression suite

Add a table-driven fixture of named, small, understandable moves.  Each test
performs both directions of the kinematic round trip:

```text
physical pose -> FK -> IK -> physical pose
virtual target -> IK -> FK -> virtual target
```

Every virtual-target case verifies the requested coordinate reaches its target
and non-requested virtual coordinates remain fixed within tolerance.  Repeat
each round trip several times to detect drift or conversion asymmetry.

Initial fixture groups:

- physical reference/zero poses;
- positive and negative physical Track, Swing, Boom, Pan, Tilt, and Roll;
- single-axis virtual vTrack, vEW, vNS, vPAN, vTilt, and vRoll moves;
- diagonal and three-axis translations;
- translation plus camera rotation;
- nonzero nodal-offset cases;
- no-roll cases;
- representative BCT and linear-boom-SPU variants.

The existing broad CSV displacement and rotation sweeps remain regression
coverage alongside these named basics.

### 3. M7 protocol/state tests

Extract or adapt firmware message handling behind host-buildable seams.  These
tests use raw DMC packet fixtures and assert response packets plus state
changes without hardware.

Priority coverage:

- signed fixed-point virtual positions, aim coordinates, and signed line-jog
  speed;
- virtual configuration validation and atomic replacement of state;
- virtual move/jog target validation and transactional failure behavior;
- aim enable, update, disable, reporting, and relative PAN/TILT behavior;
- line-jog axis mapping, stop-axis mapping, speed validation, and command
  arbitration;
- absent-roll configurations;
- capability and `VIRT_GET_POSITION` protocol compatibility.

### 4. M7 motion simulation tests

Provide a host-side motor model sufficient to exercise the M7 coordinated
motion boundary.  It should advance scheduled positions at the firmware data
rate and record directions/velocity commands; it is not a mechanical model.

Priority coverage:

- multi-axis virtual moves reach their scheduled motor targets together;
- horizons replan without position jumps or zero-velocity pulses;
- reversals decelerate before changing direction;
- soft-limit, IK, BCT, and safe-cylinder failures create no partial target;
- line-jog stop and command supersession stop every participating axis;
- longer simulated moves retain bounded orthogonal virtual drift.

### 5. Hardware acceptance tests

Hardware testing verifies the real mechanics, calibration, controller timing,
and motor stepping that software simulation cannot prove.  Keep procedures and
captured traces in `tests/hardware/`, separate from automated host tests.

Required acceptance categories:

- calibration and real-axis position accuracy;
- virtual vTrack/vEW/vNS compensation using a laser/plumb reference;
- aim retention and safe-cylinder rejection;
- camera-line direction at identity and rotated camera orientations;
- continuous line-jog behavior across horizons;
- stop-all behavior for line jog;
- no-roll crane operation;
- confirmation that virtual feature use does not affect direct real-axis
  commands.

## Fixture design

Use CSV for stable, reviewable numeric fixtures.  Each basic move should have:

- unique case name and category;
- geometry and nodal offset;
- roll-present and BCT-mode flags;
- starting physical pose or starting virtual pose;
- requested virtual axis/value when applicable;
- expected final pose values;
- repeated round-trip count;
- optional per-case tolerance override.

Do not duplicate derived values unnecessarily.  Fixtures should state inputs
and independently verified expectations; test code owns the FK/IK round trip.

## Shared tolerance policy

Define named tolerances in one host-test support header rather than scattering
literals:

- virtual translation, in mm;
- virtual rotation, in degrees;
- physical geometric translation/rotation;
- BCT raw motor steps;
- repeated-round-trip drift.

Tolerances should represent floating-point/model precision, not mechanical
calibration error.  Mechanical tolerance belongs exclusively to hardware
acceptance records.

## Implementation checklist

- [x] Add this proposal document.
  - Commit: `docs: plan layered virtuals test suite`
- [x] Add a root CMake build that runs host tests through CTest and provides
  opt-in `firmware-m7`, `firmware-m4`, and aggregate `verify` targets which
  delegate to Arduino CLI.
  - Commit: `build: add cmake host and firmware targets`
- [x] Add pinned GoogleTest through CMake `FetchContent`, a smoke target, and
  CTest discovery. Convert the existing IK/FK, CSV regression, and trajectory
  runners into individual GoogleTest cases without changing their numeric
  coverage.
  - Commits: `test: add googletest host dependency`,
    `test: add googletest test support`,
    `test: convert trajectory tests to googletest`, and
    `test: convert ik unit coverage to googletest`
- [x] Mechanically rename `ik_test/` to `tests/`, fix paths and build docs.
  - Commit: `test: rename ik harness to tests framework`
- [x] Split the former aggregate IK source into subject-specific test files.
  - Commits: `test: split ik coverage by subject` and
    `test: colocate ik test implementations`
- [x] Add a reusable, table-driven named-basic-move fixture reader.
  - Commit: `test: add virtual move fixture framework`
- [x] Add reference, single-axis, and repeated IK/FK round-trip basic move
  cases.
  - Commit: `test: cover basic virtual IK FK moves`
- [ ] Add combined, no-roll, nodal-offset, and BCT basic-move cases.
  - Commit: `test: expand virtual move regression coverage`
- [ ] Add a host-buildable M7 protocol/state seam and raw-packet fixtures.
  - Commit: `test(m7): add virtual protocol state coverage`
- [ ] Add coordinated-motion simulation and continuous line-jog tests.
  - Commit: `test(m7): simulate coordinated virtual motion`
- [ ] Move the current manual procedures into versioned hardware acceptance
  documents and add a trace-recording template.
  - Commit: `test(hardware): document virtuals acceptance procedure`

Every functional commit runs all prior host suites. Firmware targets are also
compiled when `arduino-cli` and the Giga board core are available. Hardware
tests are recorded separately and are not a gate for pure-math or parser
commits.

## Open design questions before the M7 layers

- Which M7 code should be extracted into an Arduino-independent module versus
  exercised through a thin host shim?
- Should the simulator model generated step accumulation or only scheduled
  floating-point motor positions?  Start with the latter; add step-level
  verification only where it reveals firmware/M4 integration defects.
- Which CMake targets should CI run by default: host tests only, or the full
  `verify` target when Arduino CLI and the Giga board core are available?
- Which current captured hardware traces are stable enough to become static
  protocol fixtures?

## Typed-packet and controller migration

The `typed-packet-rewrite` branch demonstrates a useful architectural
direction: typed DMC packet structures and dispatch callbacks.  It is not a
viable base branch for this work, however.  It diverged before the current
virtuals implementation, virtual command handlers remain stubs, and its CM7
rewrite is not presently buildable.  Bringing it to feature parity would
combine a broad M7/M4 rewrite with reimplementation of the already working
virtual-crane behavior.

Therefore, `virtuals-test-suite` remains the behavior baseline.  Reuse the
typed-rewrite concepts selectively, extracting them from the current working
firmware in independently verifiable steps.

### Target boundary

```text
raw DMC bytes
  -> typed packet codec and validation
  -> VirtualMotionController
  -> MotorPort and response port

Arduino M7 loop
  -> transport/clock/step-output adapters around the controller
```

`VirtualMotionController` owns virtual configuration, aim state, line-jog
state, target construction, command arbitration, and scheduling decisions. It
uses the real shared IK/FK and coordinated-trajectory libraries.  It must not
own Arduino serial, RPC, GPIO, or M4 shared-memory details.

`MotorPort` provides only the state and actions the controller needs: motor
configuration/limits/current position and velocity, coordinated target
scheduling, and stopping participating axes.  A response port emits ACKs and
position/feature packets.  Arduino adapters bind these ports to current M7
state; host fakes make the controller deterministic under test.

Use fakes for stateful simulation (motor bank, clock, and captured response
bytes).  Use GoogleMock only for narrow interaction contracts: no motor write
after rejected validation, one expected ACK, stop requests for every
participating motor, or command supersession.  Do not mock IK, FK, BCT, or
trajectory math; execute that shared production code in tests.

### Incremental migration

- [x] Adopt GoogleTest and create host test targets without changing firmware
  behavior.
- [ ] Extract typed packet definitions/codecs from the current DMC behavior,
   using the typed-rewrite layouts only as a reference.  Validate exact raw
   packet fixtures, signed fields, lengths, and checksums.
- [ ] Introduce a host-buildable `VirtualMotionController` with fake motor and
   response ports.  Start with virtual configuration and get-position.
- [ ] Port normal virtual move, jog, and stop one command at a time, retaining
   current M7 behavior and adding packet/state tests before each port.
- [ ] Port aim point and jog-on-line state, then validate coordinated handoff,
   cancellation, limits, and error behavior in the simulator.
- [ ] Reduce `m7.ino` to an adapter that decodes packets, calls the controller,
   ticks it at the existing data rate, and forwards scheduled output to the
   current M4 path.
- [ ] After parity and hardware acceptance, evaluate whether non-virtual motion
   and CM4 code should adopt the broader typed-rewrite architecture.

This sequence preserves known working crane behavior, produces a test seam as
early as possible, and avoids treating an incomplete rewrite as a prerequisite
for virtuals testing.
