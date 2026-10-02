<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Virtual motion controller

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own virtual configuration and origin, logical pose semantics, persistent aim
state, captured line-jog orientation, jog horizons, and logical-axis-to-motion-group
association. Use kinematics and calibration to prepare physical targets, then
submit them through the motion interface.

Keep pure target construction (`AimAwareTarget`, `CameraLineTarget`) adjacent to
this controller. Mathematical aim/direction helpers remain in kinematics. This
distinguishes calculating a target from deciding when and how to execute it.

Commit logical state only after the full target and scheduling request are
accepted. Return virtual-position snapshots derived from the appropriate physical
state, distinguishing effective orientation from aim-relative PAN/TILT offsets.
An absent roll axis remains explicit configuration.

The controller owns line-jog continuation and its failure semantics. A rejected
handoff, completion of the last valid horizon, and an unrecoverable next-target
failure are distinct outcomes. Define cancellation and asynchronous fault
reporting for each; do not silently discard continuation failures. This needs
characterization tests because the current tick path ignores the returned status.

## Current implementation

`Virtual`, `VirtualLineJog`, `msg_virt_*`, and virtual target helpers in `m7/m7.ino`; shared `AimAwareTarget.*` and `CameraLineTarget.*`.

## Interface and state contracts

- Own virtual configuration revision, captured origin, aim offsets, logical target state, and active line-jog/group identity. Read immutable physical snapshots through the motion port.
- Build a candidate target, solve geometry, map every physical axis, and prepare motion before commitment. On rejection preserve logical state, active motion, and prior configuration.
- After accepted motion, commit logical fields without further fallible operations. Tick-driven continuation uses the same prepare/commit path and current group generation; superseded callbacks or requests cannot restart motion.
- Report achieved virtual pose from the explicitly selected physical snapshot, retaining aim-relative offsets separately from effective camera orientation. Never use the last requested target as proof of achieved position.

## Validation required for extraction

- Exercise configure/get-position, invalid reconfigure rollback, captured origin, and no-roll operation.
- Exercise normal moves/jogs/stops, aim enable/update/disable, retained offsets, and geometry/limit failures without partial state changes.
- Simulate multiple line horizons, rejected handoffs, end-of-horizon failure, supersession, and stop-all without motion resurrection.

## Open questions

- When should disable-aim refresh physical position, and how is the retained orientation exposed?
- What event and state transition distinguish a recoverable handoff rejection from an unrecoverable next horizon?
- Which snapshot source and reconciliation point define virtual position reporting during motion?

## Planned commits

[M12](TODO.md#m12), [M13](TODO.md#m13), [M14](TODO.md#m14).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [kinematics](kinematics.md), [axis mapping calibration](axis-mapping-calibration.md), [motion](motion.md), [application coordinator](application-coordinator.md).
