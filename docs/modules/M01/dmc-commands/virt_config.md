<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# VIRT_CONFIG

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0200` · Request.

**Protocol:** Type u8; only NONE and BOOM_SWING_TRACK are implemented. Type 1 reads six (axis u32, SPU u32/1000, position i32/100000) triples, five geometry i32/u32 values scaled by 1000, optional 121 signed raw-step BCT values, and optional safe distance. Base type-1 payload is 93 bytes; full known extension is 581. Trailing extensions are deliberately ignored. ACK/GENERAL/UNSUPPORTED.

**State:** `clearVirtualConfiguration()` runs before type parsing/validation: prior configuration, aim, and virtual flags are cleared even on failure. Success commits candidate origin/geometry/BCT, SPU, and axis flags. SPU /1000 is a firmware convention not stated in the PDF.

**Motion/output:** Clearing configuration resets coordination and stops an active line jog; captures FK origin but does not request a new target. Rejection therefore is not currently rollback of the prior configuration.

**Performance considerations:** Profile base/full BCT parsing, validation/interpolation/FK and large candidate stack storage; reconfiguration while jogging also incurs stop work.

**Current code path:** Incoming branch at baseline line 1137 in [m7.ino](../../../../m7/m7.ino). Inline config branch → `clearVirtualConfiguration()` → BCT validation/conversion + `solve_fk()` → candidate/configuration writes.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual → Calibration + Kinematics and coordinate geometry + Motion subsystem.
- Response: Virtual → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Resulting cancellation output, if any: Control → Motion subsystem → Intercore.
- Numerical support: Kinematics and coordinate geometry → Mathematics.

Virtual owns crane configuration, calibration/mapping data, origin, and aim state. Motion subsystem owns axis configuration; Calibration and Kinematics and coordinate geometry perform conversions using those owners’ data.

**Characterization cases:** NONE, unsupported type, duplicate axes, axis narrowing, absent roll, invalid SPU/BCT, truncated optional fields, extension bytes, and previous-state loss on rejection.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

