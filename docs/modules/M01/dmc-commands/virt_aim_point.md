<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_AIM_POINT

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0207` · Request.

**Protocol:** Enable u8 and signed X/Y/Z i32/1000 (13 expected bytes). Success echoes same-type data with request ID and no ACK; invalid values/geometry/scheduling return error ACK. PDF also permits unsolicited aim updates, but no separate producer was found here.

**State:** Enable validates geometry and schedules before committing aim state; initial enable resets offsets, later updates retain them. Disable clears enabled/offset state without an FK refresh in this branch.

**Motion/output:** Enable/update can immediately schedule a coordinated reorientation. Disable does not explicitly stop or replace current motion.

**Performance considerations:** Measure cheap disable separately from enable/update: FK, safe-cylinder checks, aim geometry, IK/BCT, coordinated planning, and trace overhead.

**Current code path:** Incoming branch at baseline line 1361 in [m7.ino](../../../../m7/m7.ino). `aim_point_from_protocol()` → `msg_virt_aim_point()` → aim helpers + `scheduleVirtualPose()` → echo encoding.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Kinematics and coordinate geometry + Calibration → Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing coordinated execution: Control → Motion subsystem → Intercore.
- Numerical support: Kinematics and coordinate geometry + Motion subsystem → Mathematics.

Virtual owns logical/aim/jog state; Motion subsystem owns accepted physical plans and axis state. DMC and Transport handle wire scaling and response encoding without owning either state.

**Characterization cases:** Signed extremes, enable flag, safe-cylinder rejection, initial/update offsets, disable during movement, roll retention, and schedule failure state.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

