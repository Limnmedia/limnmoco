# limnmoco
Firmware for the Limnmoco

## configuration

### m4

- m4/config.h
    - pin assignments for motor step/direction
    - pin assignments for remote camera control

### m7

- m7/config.h
    - pin assignments for GIO Output and Input
    - pin assignments for the Kill Switch

## building

### arduino-ide

- install the the board support package for the mbed_giga (arduino:mbed_giga)
- Ensure that the memory is Split 1.5MB Main Core 0.5MB m4 co-processor
- When building/uploading m7 make sure the m7 Main Core is the target
- When building/uploading m4 make sure that the m4 co-processor is the target

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

### CMake host tests and firmware checks

CMake builds and runs host-side tests, and exposes opt-in firmware targets
which delegate to `arduino-cli`; it does not replace the Arduino build system.

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

**upload subprojects:**

```
    arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm7 m7
    arduino-cli upload --port <port> --fqbn arduino:mbed_giga:giga:split=75_25,target_core=cm4 m4
```
