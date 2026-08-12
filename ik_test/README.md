# Limnmoco Host Test Suite

This directory contains deterministic C++17 tests for the shared virtual-crane
math and motion primitives used by the M7 firmware. It covers IK/FK, boom
compensation, Kuper coordinates, aim-point and camera-line calculations, DMC
fixed-point conversions, and coordinated trajectory profiles.

The directory will be renamed to `tests/` as the suite expands to include M7
packet/state tests, motion simulation, and versioned hardware acceptance
procedures. The current layout and commands remain valid until that mechanical
rename occurs.

## Run the suite

Run these commands from the repository root:

```sh
cmake -S . -B build
cmake --build build --target host-tests
```

`host-tests` runs the main virtual-crane suite and the focused coordinated
trajectory suite through CTest. CMake requires version 3.16+ and a C++17
compiler.

## Coverage status

This table describes behavioral coverage, not line coverage. Percentages would
be misleading until the project has a defined denominator and coverage
instrumentation; use the status labels to show what is automated today.

| Area | Automated coverage | Known gap |
| --- | --- | --- |
| Shared IK/FK | FK-origin normalization, reconstruction sanity cases, linear virtual compensation, rotation, and CSV range sweeps | Basic named move/round-trip fixtures and repeated-drift cases are planned |
| Coordinate conventions | Kuper track sign, NS track compensation, camera-line X/Y/Z mapping, pan/tilt/roll rotation cases | Hardware confirmation of every world-space direction remains required |
| Boom compensation | Table validation, interpolation, signed Dragonframe raw-step encoding, and regression against double scaling | Broader basic-move BCT fixture coverage is planned |
| Aim point | Aim geometry, safe cylinder, relative PAN/TILT offsets, roll retention, and transactional target construction | M7 aim command/state behavior is not host-tested yet |
| Camera-line jog | Fixed-orientation target generation, signed direction, aim compensation, and safe-cylinder target rejection | M7 packet handling, horizon execution, stop behavior, and hardware line accuracy are not host-tested yet |
| Coordinated trajectories | Triangular/trapezoidal profiles, synchronized axes, velocity/acceleration limits, and pure handoff-profile math | M7 coordinated-motion handoff and reversal simulation are not implemented |
| DMC fixed-point data | Aim-point signed encode/decode | Full raw-packet parsing, checksums, response fixtures, and virtual-command state transitions are not implemented |
| M7 firmware integration | M7 firmware compiles through the `firmware-m7` CMake target | No parser, controller, or motion-simulation test seam yet |
| M4 firmware integration | M4 firmware compiles through the `firmware-m4` CMake target | No automated step-generation or shared-memory simulation yet |
| Physical crane | Manual hardware procedures and prior trace review inform acceptance expectations | Laser/plumb measurements, calibration, line-jog continuity, stop-all behavior, and absent-roll operation need versioned acceptance records |

The test-suite evolution plan, including typed packet decoding and a
host-testable virtual-motion controller, is in
[`docs/proposal_virtuals_test_suite.md`](../docs/proposal_virtuals_test_suite.md).
