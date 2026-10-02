<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_SET_SPEED

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0038` · Request.

**Protocol:** Axis u8, max velocity u32, max acceleration u32 (nine expected bytes); common ACK/RANGE. Velocity is clamped to at least 10; zero acceleration becomes half the resulting velocity.

**State:** Writes configured dynamics directly; no local movement gate or explicit existing-plan rebuild.

**Motion/output:** Affects subsequent planning; determine which current modes sample updated limits during execution.

**Performance considerations:** Small constant handler; distinguish configuration update from any later replan cost.

**Current code path:** Incoming branch at baseline line 722 in [m7.ino](../../../../m7/m7.ino). Inline `motorPtr->maxVelocity/maxAcceleration` writes.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Runtime axis configuration remains in Motion subsystem; it is not moved into Control or Device.

**Characterization cases:** Zero/default values, high values, invalid axis, update during independent/coordinated jog, and short payloads.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

