<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_JOG

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0203` · Request.

**Protocol:** Virtual axis u8, speed u16, destination i32 (seven expected bytes). Destination is raw steps on the primary physical axis, not virtual units/100000. ACK or GENERAL/RANGE; zero speed is rejected.

**State:** Refreshes FK, temporarily scales primary-axis dynamics then restores them, selects a horizon, and may cancel line-jog state. Reversal calls virtual stop. Aim path commits an effective logical target after accepted scheduling.

**Motion/output:** Builds coordinated compensation for the selected axis; aim-enabled jogging uses the full coordinated pose, while ordinary jogging selects a smaller physical set.

**Performance considerations:** Measure primary horizon, conversion, aim/non-aim solver work, selected axis count, tracing, and repeated request rate.

**Current code path:** Incoming branch at baseline line 1343 in [m7.ino](../../../../m7/m7.ino). `msg_virt_jog()` → `calculateJogDestination()` → FK/BCT/IK → `coordinated_motion_start()` or aimed `scheduleVirtualPose()`; reversal → `msg_virt_stop()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Kinematics and coordinate geometry + Calibration → Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing coordinated execution: Control → Motion subsystem → Intercore.
- Numerical support: Kinematics and coordinate geometry + Motion subsystem → Mathematics.

Virtual owns logical/aim/jog state; Motion subsystem owns accepted physical plans and axis state. DMC and Transport handle wire scaling and response encoding without owning either state.

**Characterization cases:** Raw destination semantics, reversal, speed outside nominal range, each virtual axis, aim/BCT, line-jog supersession, and limits.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

