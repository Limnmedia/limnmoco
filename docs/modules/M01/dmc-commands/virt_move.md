<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_MOVE

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0201` · Request.

**Protocol:** Virtual axis u8 and target i32/100000 (five expected bytes); common ACK or GENERAL/RANGE. No exact shape check in this branch.

**State:** Validates configuration/axis, then stops an active line jog before later target validation. Refreshes FK; commits logical target after scheduling succeeds.

**Motion/output:** Builds aim-aware pose, performs IK/calibration and plans five/six coordinated axes. Failed target can still have canceled the old line jog.

**Performance considerations:** Measure FK + aim + IK + BCT + coordinated preparation, and substantial Serial4 trace overhead separately; compare aim/BCT/roll combinations.

**Current code path:** Incoming branch at baseline line 1325 in [m7.ino](../../../../m7/m7.ino). `msg_virt_move()` → `virt_kinematics()` → `build_aim_aware_target()` → `scheduleVirtualPose()` → coordinated start → `commitVirtualTarget()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Kinematics and coordinate geometry + Calibration → Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing coordinated execution: Control → Motion subsystem → Intercore.
- Numerical support: Kinematics and coordinate geometry + Motion subsystem → Mathematics.

Virtual owns logical/aim/jog state; Motion subsystem owns accepted physical plans and axis state. DMC and Transport handle wire scaling and response encoding without owning either state.

**Characterization cases:** Every virtual axis, unreachable/soft-limit target, aim-relative PAN/TILT, no roll, no-op target, and rejected supersession of a line jog.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

