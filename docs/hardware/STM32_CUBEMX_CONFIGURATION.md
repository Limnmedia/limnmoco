# STM32CubeMX configuration record — Arduino GIGA R1 WiFi

**Status:** pre-code-generation configuration checkpoint, 2026-10-17  
**Project:** `limnmoco`, STM32 firmware in `stm32/`  
**Target:** Arduino GIGA R1 WiFi, STM32H747XIH6 (CubeMX family designation `STM32H747XIHx`)  
**Cube firmware:** STM32Cube FW_H7 **v1.13.0**  
**Toolchain generator:** **CMake** (GNU Arm toolchain intended)

> **Evidence convention:** “Configured” records the values reported in the CubeMX UI during this session; it does **not** imply a build or hardware test. “Pending” identifies unverified implementation details or deliberately deferred choices. No code had been generated or reviewed at the time of this checkpoint.

## 1. Project objectives and architecture

Replace Arduino/Mbed-based GIGA firmware with STM32 HAL/LL and C/C++ firmware maintained in the existing Git repository, using CMake, VS Code and STLINK-V3SET. Favor separately testable application logic, predictable execution time, and explicit cross-core ownership. The external motion-control application is **Dragonframe**; maintain compatibility with its existing device/protocol as integration work proceeds.

- **Cortex-M7 / CPU1:** USB device, CDC ACM transport, Dragonframe/DMC protocol processing and command dispatch.
- **Cortex-M4 / CPU2:** deterministic motion/stepper-control execution; avoid blocking USB work in this core.
- **Inter-core interface:** still to be designed; potentially shared SRAM plus hardware semaphore (HSEM) notifications, with a documented memory-ordering/cache-coherency protocol.
- **RTOS:** none initially. FreeRTOS on M7 is a future option, not selected.
- **Optimization policy:** first make a correct and measurable 480/240 MHz baseline; benchmark against slower clock/voltage operating points afterward.

## 2. Board-specific hardware facts

- MCU: **STM32H747XIH6**, dual Cortex-M7 and Cortex-M4.
- HSE crystal: **16 MHz**; LSE crystal: **32.768 kHz**. The CubeMX default HSE field initially showed 25 MHz and was corrected to 16 MHz.
- USB-C device data path (schematic USB0): connector **A6 D+** → `USBFS_D_P` → MCU **E16 / PA12**; connector **A7 D−** → `USBFS_D_N` → MCU **E17 / PA11**.
- USB OTG FS block diagram in board schematic shows **VBUS and ID unconnected** at that peripheral; therefore software VBUS sensing is disabled.
- Do not confuse the USB-C device port with the board's other USB connection/host functionality.
- For oscillator pins, RCC assigns `PH0/PH1` (HSE) and `PC14/PC15` (LSE).

References: [Arduino GIGA schematic (ABX00063)](https://docs.arduino.cc/resources/schematics/ABX00063-schematics.pdf), [full pinout](https://docs.arduino.cc/resources/pinouts/ABX00063-full-pinout.pdf), [ArduinoCore-mbed GIGA variant](https://github.com/arduino/ArduinoCore-mbed/tree/main/variants/GIGA).

## 3. Pinout & Configuration — System Core

### RCC

| Field | Selection / observed value | Reason / caveat |
|---|---|---|
| HSE Mode | Crystal/Ceramic Resonator | Physical 16 MHz HSE crystal on GIGA |
| LSE Mode | Crystal/Ceramic Resonator | Physical 32.768 kHz LSE crystal |
| MCO1 / MCO2 | Disabled | Not needed for initial firmware |
| I2S_CKIN / CRS SYNC | Disabled | No current consumer requiring these inputs |
| Supply Source | `PWR_LDO_SUPPLY` | Matches the Arduino GIGA Mbed variant configuration as investigated |
| Power Regulator Voltage Scale | **Scale 0 (VOS0)** | Needed for 480 MHz M7 operating point with LDO; initially VOS3, CubeMX later auto-selected VOS2, then we selected VOS0 |
| Flash Latency | **4 WS (5 CPU cycles)** | CubeMX automatically adjusted for configured clock/voltage; validate generated HAL sequence |
| TIM Prescaler Selection | Disabled | Leave default; revisit when using advanced timer clock configurations |
| HSE Startup Timeout | 100 ms | HAL software startup polling timeout, not crystal startup measured on hardware |
| LSE Startup Timeout | 5000 ms | HAL software startup polling timeout |
| CSI Calibration | 32 | Default factory trim setting |
| HSI Calibration | 64 | Default factory trim setting |
| VDD | 3.3 V | CubeMX modeling value for board MCU supply, not a voltage regulator programming command |
| Product Revision | Rev.V | UI setting; **verify physical silicon revision** using debugger/programmer and errata |
| RCC global NVIC interrupt | Disabled | Not needed for basic bring-up |
| User Constants | Empty | No constants defined |

GPIO RCC pin assignments `PH0/PH1`, `PC14/PC15` were left as generated. Green/gray per-module indicators represent CubeMX configuration status; gray modules do not require enabling merely to fill every section.

### Cortex-M7

- **Speculation Default Mode:** Enabled (left at default).
- **CPU I-cache and D-cache:** Disabled for initial bring-up. **Performance and coherence implications pending**; enabling M7 D-cache requires proper MPU memory attributes and/or explicit cache maintenance for DMA/shared memory.
- **MPU Control Mode:** `Background Region Privileged accesses only + MPU disabled during hard fault, NMI and FAULTMASK handlers` (as displayed).
- **MPU region 0:** enabled, base `0x00000000`, size 4 GiB, subregion-disable mask `0x87`, TEX=0, all access not permitted, execute disabled, shareable enabled, cacheable/bufferable disabled. Regions 1–15 disabled. **This configuration was left provisionally, not validated on hardware.** The subregion-disabled portions fall through to another matching region or privileged background memory map; the enabled portions prohibit access. Review MPU enablement, linker placements, privilege assumptions, and fault handling **before reliance on this setup**.
- User Constants: empty.

### Cortex-M4

- MPU disabled (default); no corresponding M7-style I/D cache.
- `SYS_M7` and `SYS_M4`: no parameters to configure at this stage.

### NVIC1 (M7) / NVIC2 (M4)

- Kept architectural exception handlers generated as default. SysTick priority **15**, with its HAL timebase handling enabled.
- `USB OTG FS global IRQ` **enabled on M7**; preemption priority **0**, subpriority **0** (temporary bring-up setting).
- Code-generation settings for USB global interrupt: init-order selection **No**, generate enable in init **Yes**, generate IRQ handler **Yes**, call HAL handler **Yes**.
- Dedicated USB FS EP1 OUT and EP1 IN IRQs disabled.
- M7/M4 have different cross-core event and HSEM interrupt vector names (`CM4 sent event for CM7` / `CM7 sent event for CM4`; `HSEM1` / `HSEM2`); none enabled yet.
- **Later:** define IRQ-priority scheme before mixing USB, motion-related IRQs, and possibly FreeRTOS. Priority 0 is highest configurable urgency and not necessarily desirable for USB long term.

### DMA, BDMA, MDMA, watchdogs, other gray System Core modules

- **DMA1/DMA2:** no requests; MemToMem empty.
- **BDMA:** request/memory-transfer tables empty.
- **MDMA:** not enabled or configured.
- **IWDG/WWDG and other unused modules:** not enabled.
- Enable only when there is an actual use case, then check bus accessibility, cache coherency, resource ownership and interrupts.

### Pinout menu

- **Keep Current Signals Placement:** enabled. Preserves existing pin assignments during later peripheral configuration but does not prevent intentional manual assignment changes or lock all peripheral parameters.
- Avoid indiscriminate `Clear Pinout`, `Reset Used GPIOs`, or `Set Unused GPIOs` on an existing board without checking schematic connections.

## 4. Clock Configuration — confirmed UI values

| Signal/field | Value | Result |
|---|---:|---|
| HSE | 16 MHz | PLL source |
| LSE | 32.768 kHz | Low-speed external oscillator |
| PLL Source Mux | HSE | All main PLLs share source selection |
| DIVM1 | /4 | PLL1 reference input 4 MHz |
| DIVN1 | ×240 | PLL1 VCO 960 MHz |
| FRACN1 | 0 | Integer multiplication |
| DIVP1 | /2 | **PLL1P = SYSCLK 480 MHz** |
| DIVQ1 | /20 | **PLL1Q = USB FS 48 MHz** |
| System Clock Mux | PLL1 / PLL1P | CPU/bus root source |
| D1CPRE | /1 | **M7 480 MHz** |
| HPRE | /2 | **M4 and AHB/AXI 240 MHz** |
| D1PPRE / D2PPRE1 / D2PPRE2 / D3PPRE | each /2 | **APB1–APB4 120 MHz** |
| VOS | Scale 0 | Target voltage scale |
| Flash | 4 WS | CubeMX-selected wait states |
| USB Clock Mux | PLL1Q | 48 MHz USB FS kernel clock |

PLL calculation: `16 MHz / 4 × 240 = 960 MHz` VCO; `960 / 2 = 480 MHz` PLL1P; `960 / 20 = 48 MHz` PLL1Q. After edits **CubeMX showed no clock warnings/errors**. This is a configuration validation, not proof of physical clock startup or USB enumeration.

**Future clock/power analysis:** 400 MHz M7 / 200 MHz M4 at VOS1 may save material power. Earlier analysis compared example STM32H747 datasheet current figures at 400 vs 480 MHz; verify exact datasheet revision, measurement conditions, and board-level current before quoting percentages for this design. The exact power cost depends on activities of both cores/peripherals, voltage scaling, idle time and execution time. Measure worst-case timing as well as average current. Do not change the PLL alone: recalculate M4, AHB, APB, USB kernel clock, voltage scale, Flash latency, timers and any protocol baud/timing dependencies.

## 5. Connectivity → USB_OTG_FS (M7 owner)

| Setting | Value | Why |
|---|---|---|
| Core assignment | **M7 only** | USB/host-protocol ownership separated from deterministic M4 motor control |
| Mode | `Device_Only` | PC is host; GIGA is USB device |
| Speed | Full Speed (12 Mbit/s) | Hardware device path; actual payload throughput lower |
| GPIO | PA11 `USB_OTG_FS_DM`, PA12 `USB_OTG_FS_DP` | Verified against schematic |
| GPIO config | AF push-pull, no pulls, Low speed | CubeMX-selected; USB PHY manages signaling |
| Activate VBUS / VBUS sensing | Disabled | USB FS VBUS and ID unconnected on GIGA schematic |
| Activate SOF / Signal SOF | Disabled | No external SOF output needed |
| Internal IP DMA | Disabled | Simpler PIO bring-up; revisit if performance demands |
| Low Power | Disabled | Simpler initial operation |
| Battery Charging | Disabled | Not needed |
| Link Power Management | Disabled | Matches middleware setting below |
| Dedicated EP1 Interrupt | Disabled | Use regular USB global IRQ |
| USB FS Global IRQ | Enabled (M7, priority 0) | HAL PCD dispatch |
| User Constants | Empty | Default |

USB clock mux set to PLL1Q at **48 MHz**. Note: VBUS-sensing-disabled behavior and host disconnect/reconnect should be tested on hardware.

## 6. Middleware → USB_DEVICE (M7)

Selected **Communication Device Class (CDC ACM)** on FS IP. HS IP mode remains unavailable/disabled. USB_HOST is not enabled. USB hardware configuration, HAL PCD driver, and USB_DEVICE middleware are distinct layers.

| Parameter | Value/status |
|---|---|
| `USBD_MAX_NUM_INTERFACES` | 1 (default; check actual generated descriptor interface count, as CDC ACM normally uses control and data interfaces) |
| `USBD_MAX_NUM_CONFIGURATION` | 1 |
| `USBD_MAX_STR_DESC_SIZ` | 512 bytes |
| `USBD_SELF_POWERED` | Enabled **provisionally**; verify power circuitry and descriptor correctness |
| `USBD_DEBUG_LEVEL` | 0 |
| `USBD_LPM_ENABLED` | **0**, changed from default 1 to match disabled peripheral LPM |
| USB CDC Rx Buffer Size | 2048 bytes |
| USB CDC Tx Buffer Size | 2048 bytes |
| User Constants | Empty |

The CDC buffer sizes are application/middleware allocations; USB full-speed bulk packets max at 64 bytes. Receive buffering, re-arming, handling backpressure, transfer completeness and concurrency must be implemented/tested in application code.

### USB descriptor values currently in CubeMX (NOT CHANGED)

| Field | CubeMX default |
|---|---|
| VID | 1155 = `0x0483` (ST) |
| PID (FS) | 22336 = `0x5740` |
| Language | English (US) |
| Manufacturer | `STMicroelectronics` |
| Product | `STM32 Virtual ComPort` |
| Configuration string | `CDC config` |
| Interface string | `CDC Interface` |

**Existing Arduino Mbed application as observed with Linux tools:**

- USB VID:PID `2341:0266` (Arduino/Giga), manufacturer `Arduino`, product `Giga`.
- `bcdUSB = 2.00`, `bcdDevice = 1.01`.
- Serial string example `004D00333333510432363530`; **do not hardcode this serial**; if needed derive a stable serial from hardware unique ID.
- `bDeviceClass = 0xEF` (Miscellaneous, commonly used with Interface Association Descriptors).
- CDC communication interface class 2, ACM subclass 2, and CDC Data interface class 10.
- At least one full-speed bulk endpoint `0x01` (EP1 OUT), 64-byte max packet.
- Existing Linux device appears as `/dev/ttyACM0`.

**Compatibility considerations:** Dragonframe discovers COM ports, but whether it also filters VID/PID/product strings or probes an application-level **DMC protocol handshake** is not proven. CDC ACM ensures an OS serial-device interface; it does not itself guarantee Dragonframe recognition. Preserve/reimplement the DMC wire protocol, line-control behavior (DTR/RTS/baud requests if relevant), message framing and timeout rules. Compare all USB configuration/interface/endpoint descriptors, including IN endpoints, rather than assuming identity from the excerpt above. Don't casually use Arduino's VID/PID in firmware intended for distribution; evaluate ownership/authorization and compatibility testing separately. The device bootloader may present a different USB identity than normal application firmware.

## 7. Project Manager — Project

| Parameter | Value |
|---|---|
| Project name | `stm32` |
| Project location | `/home/cade/Projects/programming/limnmoco` |
| Toolchain folder location | `/home/cade/Projects/programming/limnmoco/stm32` |
| Application structure | Advanced (read-only in UI) |
| Dual-core boot mode | **Both CPUs Booting at Once** (read-only in UI) |
| Do not generate main() | Disabled; CubeMX may generate `main.c` for both cores |
| Toolchain/IDE | **CMake** (changed from EWARM) |
| Generate Under Root | Disabled (read-only in UI) |
| MCU | `STM32H747XIHx` |
| Firmware | `STM32Cube FW_H7 v1.13.0` |

**Linker minimums per core:** heap `0x200` (512 B), stack `0x400` (1024 B). These are provisional *minimum linker reservations*, **not verified safe runtime stack/heap budgets**. Inspect the generated linker scripts and actual SRAM assignments; measure stack high-water/maximum use. Prefer fixed-capacity data and no routine heap churn on M4. For any C++ translation units, confirm constructors/static initialization and C/C++ linking behavior for both cores.

**Thread Safe Settings:** `Enable multi-threaded support` **disabled** for both M4 and M7; `Thread safe locking strategy` left at default mapping depending on RTOS. This does **not** make ISR/foreground access or inter-core shared memory thread-safe. Revisit if introducing an RTOS, threaded newlib use, or shared runtime state.

**Dual-core boot warning:** CubeMX displayed “Both CPUs Booting at Once.” Earlier Arduino firmware reportedly starts M4 under M7 control. **Do not assume generated startup replicates Arduino's behavior.** Review option bytes, BOOT address, generated CPU startup and synchronization before first flash or running both cores.

## 8. Pending work and change-control checklist

### Before first Generate Code

1. Preserve the `.ioc` and all existing work in Git; confirm `git status` and protect uncommitted changes. The button may initiate generation immediately; no prior generated output has been reviewed.
2. Confirm generated destination paths are confined to intended `stm32/` subtree; inspect all additions and modifications with Git.
3. Pin/record CubeMX version, `STM32Cube FW_H7 v1.13.0`, compiler/CMake versions and toolchain file paths for reproducibility.
4. Inspect generated CMake/target structure, both core linker scripts, startup files, vector tables, `SystemClock_Config`, VOS0 sequencing and `FLASH_ACR` latency.
5. Audit **bootloader and Flash offsets before programming**. Existing Arduino Mbed application placement was previously discussed as an offset around `0x08040000`; verify on the actual board/variant instead of assuming CubeMX linker defaults preserve its bootloader. Avoid mass erase or overwriting bootloader/critical option bytes until deliberate decision and recovery plan.
6. Confirm generated middleware descriptors, CDC endpoint layout, FIFO sizing and USB interrupt handler wiring.
7. Confirm that M7/M4 linker memory regions don't overlap, that shared SRAM is intentionally reserved, and that neither core overwrites the other's initialized data.

### First hardware bring-up

- Run minimal firmware with clear core-specific logging/LED/debug markers before adding motion output.
- Verify HSE/PLL1/clock startup, 480/240 MHz clocks, VOS0, Flash access, M4 start behavior, and basic USB enumeration.
- Test `/dev/ttyACM*` enumeration, Windows COM port behavior, unplug/replug, USB reset, suspend and host discovery.
- Record actual USB descriptors with `lsusb -v` and compare to old image.
- Set up fault diagnostics and stack-usage instrumentation; 1 KiB stack is not a validated production budget.
- Validate no unintended actuator activity while resetting/debugging/flashing.

### When adding components later

| Trigger | Required configuration / design review |
|---|---|
| Dragonframe protocol implementation | Preserve DMC framing/commands/responses, CDC port discovery/control lines, timeouts and VID/PID assumptions |
| GPIO for stepper drivers | Map pins with the GIGA schematic and wiring; safe reset states, output polarity, drive strength, timing and contention |
| M4 timers / motor pulse generation | Choose timers/clock domain, APB timer multipliers, prescalers, DMA/IRQ strategy; verify worst-case jitter under load |
| Inter-core messaging | Allocate aligned shared SRAM, define ownership/ring or mailbox protocol, HSEM/SEV interrupts if appropriate, barriers/cache maintenance |
| Enable I/D cache on M7 | MPU regions and cacheability, DMA buffer alignment, shared memory non-cacheable region vs explicit cache clean/invalidate |
| DMA/BDMA/MDMA | Controller reachability to memory and peripheral, stream/request mapping, DMA-safe buffers, cache coherency, interrupt priorities |
| Add UART debug mirror | Verify board wiring, choose owning core, baud source/clock and GPIO AF, DMA/IRQ mode, framing |
| Add FreeRTOS on M7 | Revisit SysTick/PendSV/SVC, HAL tick source, NVIC priority and maximum syscall priority, thread-safe runtime locking |
| Add USB features/classes | Recheck 48 MHz clock, FIFO/RAM constraints, endpoint layout, descriptors and host driver behavior |
| Power reduction to 400/200 MHz | Review VOS1, PLL1P **and PLL1Q USB 48 MHz**, Flash latency, AHB/APB/timer frequencies, WCET and current measurements |
| Watchdog / safety | Decide independent watchdog per core, feeding/health strategy, fault containment, reset cause reporting |
| Production firmware deployment | Establish licensing/USB IDs, bootloader strategy, recovery and repeatable programming artifacts |

## 9. Technical references

- [Arduino GIGA R1 WiFi schematic (ABX00063)](https://docs.arduino.cc/resources/schematics/ABX00063-schematics.pdf)
- [Arduino GIGA R1 WiFi pinout](https://docs.arduino.cc/resources/pinouts/ABX00063-full-pinout.pdf)
- [ArduinoCore-mbed GIGA variant](https://github.com/arduino/ArduinoCore-mbed/tree/main/variants/GIGA)
- **ST RM0399**, STM32H747/757 reference manual — RCC, PWR, FLASH, MPU-adjacent system hardware, USB OTG FS, NVIC, HSEM, memory domains. Locate the current revision from ST's STM32H747 documentation portal.
- **ST DS12930**, STM32H747xI/G datasheet — electrical/clock/power and operating limits.
- **ST AN5215**, STM32H7 power management — voltage scales and performance levels.
- **ST PM0253**, Cortex-M7 programming manual — exceptions, MPU, speculative execution and caches.
- **ST PM0214**, Cortex-M4 programming manual — exceptions, MPU, execution model.
- **ST AN2867**, oscillator design guide — oscillator behavior/startup.
- **ST ES0392**, STM32H747/757 errata — check actual die revision.
- [STM32CubeH7](https://github.com/STMicroelectronics/STM32CubeH7) — HAL and middleware sources (verify checked-out tag corresponds to v1.13.0).

**Document maintenance:** Update this record whenever a CubeMX setting changes. Capture the `.ioc` diff and generated-code diff together, and distinguish confirmed hardware measurements from design assumptions.
