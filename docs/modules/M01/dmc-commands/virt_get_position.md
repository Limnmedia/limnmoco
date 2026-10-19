<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_GET_POSITION

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0205` · Request + periodic report.

**Protocol:** Empty request; configured rig returns six i32/100000 pose fields plus aim-enable u8 and three i32/1000 aim coordinates (37-byte payload), with request ID and no ACK. Unconfigured request returns GENERAL ACK. Periodic caller uses ID 0.

**State:** Runs FK and updates cached virtual pose; aim-relative PAN/TILT offsets remain separate. Failed internal FK refresh needs characterization because the helper can return without replacing cached values.

**Motion/output:** No new plan. Reports calculated pose from motor software positions; absent roll is zero.

**Performance considerations:** Profile FK/BCT and Serial4 tracing plus 49-byte framed response; periodic reports share the motion loop budget.

**Current code path:** Incoming branch at baseline line 1351 in [m7.ino](../../../../m7/m7.ino). `msg_virt_get_position()` → `virt_kinematics()` → `aim_point_to_protocol()` → serializer; also called by periodic `loop()` reporting.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Motion subsystem + Calibration + Kinematics and coordinate geometry.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Periodic pose calculation and report: Control → Virtual → Motion subsystem + Calibration + Kinematics and coordinate geometry → Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Numerical support: Kinematics and coordinate geometry → Mathematics.

**Characterization cases:** Origin normalization, aim offsets, no roll, negative values, unconfigured/faulted query, stale conversion result, and periodic cadence.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

