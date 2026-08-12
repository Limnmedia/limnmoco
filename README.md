# limnmoco

Firmware, virtual-crane inverse kinematics, and host-side tests for Limnmoco.

## configuration

### m4 co-processor

- m4/config.h
    - pin assignments for motor step/direction
    - pin assignments for remote camera control

### m7 main processor

- m7/config.h
    - pin assignments for GIO Output and Input
    - pin assignments for the Kill Switch

## building

### arduino-ide

- Install the board support package for `arduino:mbed_giga`.
- Select the `75_25` flash split: 1.5 MB for the m7 main processor and 0.5 MB
  for the m4 co-processor.
- When building or uploading m7, select the m7 main processor target.
- When building or uploading m4, select the m4 co-processor target.

### arduino-cli

**install giga r1 support:**

```
    arduino-cli core install arduino:mbed_giga
```

**find connected boards:**

```
    arduino-cli board list
```

make a note of the port for uploading

**compile subprojects:**

```
    arduino-cli compile --libraries common --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm7 m7
    arduino-cli compile --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm4 m4
```

### CMake tests and firmware checks

CMake 3.16+ and a C++17 compiler build and run the host-side suites. CMake also
provides opt-in firmware targets which delegate to `arduino-cli`; it does not
replace the Arduino build system.

```sh
cmake -S . -B build
cmake --build build --target host-tests
cmake --build build --target firmware-m7
cmake --build build --target firmware-m4
cmake --build build --target verify
```

`firmware-m7`, `firmware-m4`, and `verify` are available only when
`arduino-cli` is installed and the Giga board core has been installed as above.
They are intentionally not part of CMake's default build target.

See the [host test suite](ik_test/README.md) for test coverage and known gaps.

**upload subprojects:**

```
    arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm7 m7
    arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm4 m4
```
