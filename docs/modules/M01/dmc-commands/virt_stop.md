<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_STOP

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0202` · Request.

**Protocol:** Virtual axis u8; ACK or GENERAL/RANGE. No-roll ROLL is rejected. Other invalid axes can reach coordination reset before the final error.

**State:** Matching active line-jog stop axis stops all mapped participants and clears line-jog state. Otherwise resets coordinated state and dispatches a logical-axis-specific physical subset.

**Motion/output:** TRACK stops track; EW stops swing/track/pan; NS stops boom/track; rotations stop corresponding axes. Characterize nonmatching line-jog stops and incomplete group stops separately.

**Performance considerations:** Measure subset versus full-participant stop setup and output-to-rest duration.

**Current code path:** Incoming branch at baseline line 1338 in [m7.ino](../../../../m7/m7.ino). `msg_virt_stop()` → `stopVirtualLineJogMotion()` or `coordinated_motion_reset()` + `msg_motor_stop()` calls.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Braking execution: Control → Motion subsystem → Intercore.

Virtual identifies the affected logical motion/group; Motion subsystem owns its braking execution.

**Characterization cases:** Recorded line axis, different logical axis, coordinated multi-axis target, invalid axis mutation, aim enabled, and no-roll rig.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

