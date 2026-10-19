<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Calibration

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own pure conversion between physical joint values and motor steps: steps per
unit, direction conventions, boom compensation lookup, virtual joint-to-axis
mapping, and optional-roll handling. Conversion receives validated configuration
and returns all required physical-axis targets or an error.

This is the boundary between kinematics and motor execution. BCT values already
represent raw motor steps; the API must make applying SPU a second time difficult.
Neither the solver nor the motion engine should interpret a BCT.

Configuration is owned by its consumer, rather than a mutable global settings
service: motion owns axis limits/dynamics/coupling; virtual control owns crane
geometry, calibration, axis mapping, and the captured origin. Mapping helpers
read immutable snapshots. Configuration revisions invalidate stale prepared
plans. Persistent storage can be added later without entering the solver.

## Current implementation

`BoomCompensation.*`, `boomMotorStepsToGeometricAngle()`, `boomGeometricAngleToMotorSteps()`, and conversion code in `virt_kinematics()` / `scheduleVirtualPose()`.

## Interface and state contracts

- Receive immutable calibration/mapping and physical-axis configuration snapshots. Return a complete joint observation or complete physical target set; leave output unchanged on conversion failure.
- Give raw motor steps and joint engineering units distinct representations. BCT interpolation returns raw steps; SPU conversion is the alternative path, not a second scaling pass.
- A validated mapping explicitly represents absent roll and rejects conflicting axis assignments. Prepared results retain the configuration revisions on which they depend.

## Validation required for extraction

- Cover increasing/decreasing BCT tables, out-of-range values, interpolation, inverse conversion, and signed raw-step encoding.
- Test optional roll, invalid SPU, duplicate mappings, and stale configuration revisions.
- Run existing compensated named moves through the extracted mapping without changing targets.

## Open questions

- What exact validation and reconfiguration rules apply to a virtual axis mapped onto a coupled physical follower?
- Which calibration values belong in the current runtime configuration, and which future values would require persistence?

## Planned commits

[M04](TODO.md#m04), [M12](TODO.md#m12).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [kinematics](kinematics.md), [motion](motion.md), [Virtual](virtual.md).
