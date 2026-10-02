# Limnmoco Host Test Suite

This directory contains deterministic C++17 tests for the shared virtual-crane
math and motion primitives used by the M7 firmware. It covers IK/FK, boom
compensation, Kuper coordinates, aim-point and camera-line calculations, DMC
fixed-point conversions, and coordinated trajectory profiles.

The suite is organized by boundary: `ik/` contains shared crane math,
`fixtures/` contains versioned numeric data, `trajectory/` contains focused
motion-profile tests, and `support/` contains shared test infrastructure. M7
packet/state, motion simulation, and hardware-acceptance directories will be
added as those layers are implemented.

Add new GoogleTest cases in a subject-specific `*_test.cpp` file in the
appropriate boundary directory. Keep reusable fixture loading and legacy case
construction in helper sources; do not add new test cases to an aggregate test
file.

## Run the suite

Run these commands from the repository root:

```sh
cmake -S . -B build
cmake --build build --target host-tests
```

`host-tests` runs the main virtual-crane suite and the focused coordinated
trajectory suite through CTest. It also fetches the pinned GoogleTest 1.17.0
release through CMake `FetchContent`. All current host tests are discovered
GoogleTest cases; the smoke test remains as a direct dependency integration
check. CMake requires version 3.16+, a C++17 compiler, and network access on
the first configuration.

## Coverage status

This table describes behavioral coverage, not line coverage. Percentages would
be misleading until the project has a defined denominator and coverage
instrumentation; use the status labels to show what is automated today.

| Area | Automated coverage | Known gap |
| --- | --- | --- |
| Shared IK/FK | FK-origin normalization, reconstruction sanity cases, reference, single-axis, combined, no-roll, nodal-offset, and BCT named moves, repeated round trips, linear virtual compensation, rotation, and CSV range sweeps | Additional BCT profiles and wider multi-axis ranges are planned |
| Coordinate conventions | Kuper track sign, NS track compensation, camera-line X/Y/Z mapping, pan/tilt/roll rotation cases | Hardware confirmation of every world-space direction remains required |
| Boom compensation | Table validation, interpolation, signed Dragonframe raw-step encoding, and regression against double scaling | Broader basic-move BCT fixture coverage is planned |
| Aim point | Aim geometry, safe cylinder, relative PAN/TILT offsets, roll retention, and transactional target construction | M7 aim command/state behavior is not host-tested yet |
| Camera-line jog | Fixed-orientation target generation, signed direction, aim compensation, and safe-cylinder target rejection | M7 packet handling, horizon execution, stop behavior, and hardware line accuracy are not host-tested yet |
| Coordinated trajectories | Triangular/trapezoidal profiles, synchronized axes, velocity/acceleration limits, and pure handoff-profile math | M7 coordinated-motion handoff and reversal simulation are not implemented |
| DMC fixed-point data | Aim-point signed encode/decode | Full raw-packet parsing, checksums, response fixtures, and virtual-command state transitions are not implemented |
| Named-move fixtures | Strict CSV schema parsing, finite numeric values, boolean flags, BCT step expectations, repeated-round-trip counts, transactional rejection of malformed records, and reference/single-axis/combined/no-roll/nodal-offset/BCT cases | Additional BCT profiles and wider multi-axis ranges are not implemented yet |
| M7 firmware integration | M7 firmware compiles through the `firmware-m7` CMake target | No parser, controller, or motion-simulation test seam yet |
| M4 firmware integration | M4 firmware compiles through the `firmware-m4` CMake target | No automated step-generation or shared-memory simulation yet |
| Physical crane | Manual hardware procedures and prior trace review inform acceptance expectations | Laser/plumb measurements, calibration, line-jog continuity, stop-all behavior, and absent-roll operation need versioned acceptance records |

The test-suite evolution plan, including typed packet decoding and a
host-testable virtual-motion controller, is in
[`docs/proposal_virtuals_test_suite.md`](../docs/proposal_virtuals_test_suite.md).
