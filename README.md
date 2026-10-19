# LIMNMOCO

Firmware, virtual-crane inverse kinematics, and host-side tests for Limnmoco.

The [modular firmware architecture plan](docs/modules/README.md) links the module
discussions and [planned commit sequence](docs/modules/TODO.md).

LIMNMOCO is a six-axis physical motion-control crane for stop-motion animation, developed by LIMNMEDIA. This repository contains the Arduino firmware that runs the crane controller and communicates with the motion-control system.

## Overview

The LIMNMOCO machine has six physical motion axes:

```text
Track → Swing → Boom → Pan → Tilt → Roll
```

The firmware is designed for an Arduino GIGA R1 and is divided between its M7 main core and M4 co-processor. The controller firmware provides eight motor channels plus one camera channel. That controller capacity is separate from the six-axis physical description of the LIMNMOCO crane; this repository does not define an axis-to-channel mapping.

## Project Status

This repository is primarily a firmware source repository. It contains the M7 and M4 Arduino sketches, motion calculations, DMC message handling, hardware configuration, and a DMC protocol reference PDF.

The source includes DMC virtual-axis capability constants, but their presence does not establish that virtual-axis functionality is complete. Treat the firmware as an actively developed controller implementation and verify behavior on the intended hardware before production use.

## Firmware Architecture

```text
Arduino GIGA R1
├── M7 main core
│   ├── DMC message parsing and responses
│   ├── motor state and motion calculations
│   ├── move upload/playback and limits
│   └── shared motor and camera state
└── M4 co-processor
    ├── step/direction pulse generation
    ├── eight motor outputs
    └── camera meter/shutter outputs
```

The two sketches exchange shared motor and camera state through the GIGA's shared memory and RPC setup. The M7 sketch handles incoming DMC commands and calculates motor state; the M4 sketch uses that state to generate the motor pulses and camera outputs.

## Dragonframe / DFMoco

The firmware source is based on the original `dmc-lite` sketch by Dyami Caliri at DZED Systems, Inc. (Dragonframe), and the original code is used with permission from the original author.

The source headers identify the following copyright notices:

- Copyright 2023 by DZED Systems LLC
- Copyright 2026 by Limnmedia LLC

This attribution describes the source provenance of the firmware and DMC implementation. It is separate from the repository license described below.

## Repository Structure

```text
m7/
  m7.ino       M7 main-core sketch
  config.h     M7 board and logic I/O configuration
  dfx.h        DMC data structures and controller constants
  dmc_msg.*    DMC protocol message handling
  motion.*     Motion calculation functions

m4/
  m4.ino       M4 co-processor sketch
  config.h     M4 step/direction and camera I/O configuration

docs/
  DMC-Protocol-2024-02-13.pdf

LICENSE        BSD 3-Clause license for this repository
```

## Building

### Arduino IDE

1. Install the board support package for `mbed_giga` (`arduino:mbed_giga`).
2. Set the memory split to **1.5 MB Main Core / 0.5 MB M4 co-processor**.
3. When building or uploading `m7`, select the M7 Main Core target.
4. When building or uploading `m4`, select the M4 co-processor target.

### Arduino CLI

Install GIGA R1 support:

```text
arduino-cli core install arduino:mbed_giga
```

Find connected boards and note the upload port:

```text
arduino-cli board list
```

Compile the two sketches:

```text
arduino-cli compile --fqbn arduino:mbed_giga:giga,split=75_25,target_core=cm7 m7
arduino-cli compile --fqbn arduino:mbed_giga:giga,split=75_25,target_core=cm4 m4
```

Upload the two sketches:

```text
arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm7 m7
arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm4 m4
```

## Hardware

The current board configuration targets the Arduino GIGA R1 and uses 3.3 V TTL signals. The M4 configuration defines eight step/direction output pairs and camera meter/shutter outputs. The M7 configuration defines logic outputs and optional logic-switch and kill-switch inputs.

The pin assignments are in `m4/config.h` and `m7/config.h`.

## Documentation

The repository's DMC protocol reference is available at [`docs/DMC-Protocol-2024-02-13.pdf`](docs/DMC-Protocol-2024-02-13.pdf). The source files and their headers are the authoritative references for the current firmware behavior and attribution.

## License and Acknowledgements

This repository is distributed under the BSD 3-Clause License. See [`LICENSE`](LICENSE) for the complete license text.

The firmware source acknowledges the original `dmc-lite` sketch by Dyami Caliri at DZED Systems, Inc. / Dragonframe and records that the original code is used with permission from the original author. Copyright notices for DZED Systems LLC and Limnmedia LLC are preserved in the source headers and summarized above.
## CMake Tests and Firmware Checks

CMake 3.16+ and a C++17 compiler build and run the host-side suites. CMake also
provides opt-in firmware targets which delegate to `arduino-cli`; it does not
replace the Arduino build system.

```sh
cmake -S . -B build
cmake --build build --target host-tests
cmake --build build --target verify
```

`firmware-m7`, `firmware-m4`, and `verify` are available only when
`arduino-cli` is installed and the Giga board core has been installed as above.
They are intentionally not part of CMake's default build target.

To enable upload targets, configure the serial port explicitly, then select
the desired processor target:

```sh
cmake -S . -B build -DLIMNMOCO_UPLOAD_PORT=/dev/ttyACM0
cmake --build build --target upload-m7
cmake --build build --target upload-m4
cmake --build build --target upload-all
```

`upload-m7` and `upload-m4` each compile and upload the selected processor.
`upload-all` uploads M7 first, then M4.

See the [host test suite](tests/README.md) for test coverage and known gaps.

Use the Arduino IDE to upload either processor after selecting its target and
the required 75_25 flash split above.
