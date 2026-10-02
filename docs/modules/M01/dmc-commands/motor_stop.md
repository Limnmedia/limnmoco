<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_STOP

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0032` · Request.

**Protocol:** Reads axis u8; returns ACK or axis-range error. No exact length check here.

**State:** In JOG mode or for a live-control axis, installs stopping segments; otherwise can ACK without stopping. Updates mode/synchronization for eligible non-live axes.

**Motion/output:** Decelerates through `stopMotor()` rather than immediately disabling pulses. Already stopped/stopping axes can be no-ops.

**Performance considerations:** Measure acceptance and braking-plan cost separately from physical stopping duration; compare at velocity and near limits.

**Current code path:** Incoming branch at baseline line 698 in [m7.ino](../../../../m7/m7.ino). `msg_motor_stop()` → `stopMotor()` → later independent update.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Braking execution: Control → Motion subsystem → Intercore.

**Characterization cases:** Idle/stopping, playback-owned axis, live-control axis, and participant in a coordinated virtual group.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

