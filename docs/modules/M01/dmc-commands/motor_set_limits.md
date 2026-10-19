<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_SET_LIMITS

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0039` · Request.

**Protocol:** Axis u8, lower-enable u8, lower i32, upper-enable u8, upper i32, HW_SET u8 (12 expected bytes). HW_SET is consumed but ignored; normal success ACK.

**State:** Directly replaces software limit fields, without lower/upper consistency validation in this branch.

**Motion/output:** No immediate motion request. Record which planners and stopping paths enforce updated limits; do not assume enforcement is uniform.

**Performance considerations:** Constant decode/write work; profile stop/replan consequences separately.

**Current code path:** Incoming branch at baseline line 755 in [m7.ino](../../../../m7/m7.ino). Inline limit writes; consumers include coordinated target checks and `stopMotor()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Runtime axis configuration remains in Motion subsystem; it is not moved into Control or Device.

**Characterization cases:** Reversed limits, enable bytes, boundary target, update while moving, ignored HW_SET, and coupled-axis constraints.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

