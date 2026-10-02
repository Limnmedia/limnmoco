<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_JOG_ON_LINE

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0206` · Request with continuing execution.

**Protocol:** Axis u8 (0..4), signed speed i16 (three expected bytes). Rejects zero, INT16_MIN, magnitude above 10000, bad axis, or unavailable configuration; common ACK.

**State:** Captures effective orientation, signed rate, logical stop axis, and active line state. Restores prior line state if initial execution fails; later continuation failures are returned to a tick caller that currently discards the status.

**Motion/output:** Builds 20 mm translation or 2-degree rotation horizons; first starts a coordinated plan, later ticks try handoff. Failed handoff can allow the old horizon to finish; verify next-target failure and continuity.

**Performance considerations:** Measure initial command cost separately from recurring FK/target/IK/handoff cost at 50 Hz, including trace output and worst observed continuation interval.

**Current code path:** Incoming branch at baseline line 1354 in [m7.ino](../../../../m7/m7.ino). `msg_virt_jog_on_line()` → `executeVirtualLineJog()` → `build_camera_line_target()` → `scheduleVirtualPose()`; continued inside `updateMotorVelocities()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Kinematics and coordinate geometry + Calibration → Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Horizon continuation: Control → Virtual → Kinematics and coordinate geometry + Calibration → Motion subsystem.
- Continuing coordinated execution: Control → Motion subsystem → Intercore.
- Numerical support: Kinematics and coordinate geometry + Motion subsystem → Mathematics.

Virtual owns logical/aim/jog state; Motion subsystem owns accepted physical plans and axis state. DMC and Transport handle wire scaling and response encoding without owning either state.

**Characterization cases:** Signed directions/all axes, aim compensation, multiple horizons, incompatibility/reversal, limits, next-target failure, logical stop, and stop-all.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

