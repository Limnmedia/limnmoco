# LIMNMOCO IK Host Test Harness

This folder contains a standalone C++ port of the Blender reference solver from:

```text
limnmoco-blender-addon/LIMNMOCO_CG_RIG/rigs/limnmoco_crane.py
```

The harness has no Arduino or Blender dependencies. It validates the same shared Swing-Boom-Track crane IK/FK library used by the `m7` firmware.

## Files

```text
limnmoco_ik.h    Test-facing adapters and host-only types
limnmoco_ik.cpp  Test-facing adapters
main.cpp         Console test runner with deterministic sanity cases
../common/LimnmocoIK/  Shared IK/FK library used by firmware and host tests
```

## Build

Use any C++17 compiler.

With `g++`:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -I../common/LimnmocoIK/src \
  main.cpp limnmoco_ik.cpp ../common/LimnmocoIK/src/LimnmocoIK.cpp \
  ../common/LimnmocoIK/src/Vec3.cpp ../common/LimnmocoIK/src/Mat3.cpp \
  ../common/LimnmocoIK/src/CoordinatedTrajectory.cpp \
  ../common/LimnmocoIK/src/BoomCompensation.cpp \
  -o limnmoco_ik_test
```

With `clang++`:

```sh
clang++ -std=c++17 -Wall -Wextra -pedantic -I../common/LimnmocoIK/src \
  main.cpp limnmoco_ik.cpp ../common/LimnmocoIK/src/LimnmocoIK.cpp \
  ../common/LimnmocoIK/src/Vec3.cpp ../common/LimnmocoIK/src/Mat3.cpp \
  ../common/LimnmocoIK/src/CoordinatedTrajectory.cpp \
  ../common/LimnmocoIK/src/BoomCompensation.cpp \
  -o limnmoco_ik_test
```

With MSVC Developer PowerShell:

```powershell
cl /EHsc /std:c++17 /I..\common\LimnmocoIK\src main.cpp limnmoco_ik.cpp ..\common\LimnmocoIK\src\LimnmocoIK.cpp ..\common\LimnmocoIK\src\Vec3.cpp ..\common\LimnmocoIK\src\Mat3.cpp ..\common\LimnmocoIK\src\CoordinatedTrajectory.cpp /Fe:limnmoco_ik_test.exe
```

The trajectory primitive has a separate focused test:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -I../common/LimnmocoIK/src \
  coordinated_trajectory_test.cpp \
  ../common/LimnmocoIK/src/CoordinatedTrajectory.cpp \
  -o coordinated_trajectory_test
./coordinated_trajectory_test
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
- CSV fixture sweeps for linear vEW/vNS/vTrack displacement.
- CSV fixture sweeps for vPAN/vTilt/vRoll with nodal offsets
  `(-0.2727, -0.1463, 0.3179) mm`.

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

The shared library also exposes the `Vec3` and `Mat3` types and their vector,
matrix, and rotation operations for other host or firmware modules.
