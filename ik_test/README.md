# LIMNMOCO IK Host Test Harness

This folder contains a standalone C++ port of the Blender reference solver from:

```text
limnmoco-blender-addon/LIMNMOCO_CG_RIG/rigs/limnmoco_crane.py
```

The harness has no Arduino or Blender dependencies. Its purpose is to validate the Swing-Boom-Track crane IK math before the solver is wired into the legacy `m7` firmware.

## Files

```text
limnmoco_ik.h    Plain C++ solver types and function declarations
limnmoco_ik.cpp  Plain C++ solver implementation
main.cpp         Console test runner with deterministic sanity cases
```

## Build

Use any C++17 compiler.

With `g++`:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp limnmoco_ik.cpp -o limnmoco_ik_test
```

With `clang++`:

```sh
clang++ -std=c++17 -Wall -Wextra -pedantic main.cpp limnmoco_ik.cpp -o limnmoco_ik_test
```

With MSVC Developer PowerShell:

```powershell
cl /EHsc /std:c++17 main.cpp limnmoco_ik.cpp /Fe:limnmoco_ik_test.exe
```

## Run

```sh
./limnmoco_ik_test
```

On Windows PowerShell:

```powershell
.\limnmoco_ik_test.exe
```

## Current Test Cases

- Blender default panel values:
  - `VTrack = 8`
  - `VEW = 2`
  - `VHeight = 3`
  - `VPan = 0`
  - `VTilt = 0`
  - `VRoll = 0`
  - `Boom Length = 10`
  - `Level Extension = 2.8`
  - `Offset = 0, 0, 0`
- Centered target with no offset.
- Rotated nodal offset sanity case.

The first two cases check exact solved axis values. All cases check reconstruction error.

## Notes

The solver intentionally mirrors the Blender reference model:

- `VEW` maps to X.
- `VTrack` maps to Y.
- `VHeight` maps to Z.
- `VPan` rotates around Z.
- `VTilt` rotates around X.
- `VRoll` rotates around Y.

The harness reports when a solve clamps boom or swing input ratios to keep `asin()` in range. That behavior matches the current Blender reference solver, but the firmware integration still needs a developer decision on whether unreachable targets should clamp, reject, or report an error.
