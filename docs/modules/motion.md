<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Motion subsystem

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own commanded physical-axis state and execution. Expose operations such as
prepare/start, replace/handoff, stop a group, stop all, obtain a snapshot, and
advance one accepted control tick. Emit physical-axis samples and completion or
fault events; do not write hardware or serialize messages.

Internal components:

| Component | Cohesive responsibility |
| --- | --- |
| Axis bank | Validated axis configuration, commanded position/velocity, emitted-step observations, and movement status. No public mutable `Motor*` access. |
| Independent planner/executor | Per-axis moves, jog horizons, braking, and reversal from the current position and velocity. Axes can progress independently. |
| Coordinated planner/executor | Shared progress profile, participant set, synchronized limits, and velocity-preserving handoff validation. |
| Execution ownership | Enforce one writer per axis, reserve whole groups, transfer ownership atomically, and apply stops to the correct participant set. |
| Output mapping | Expand coupled followers and produce the physical-axis sample consumed by the output adapter. Validate follower limits and conflicts before acceptance. |

Call the first planner *independent motion*: lack of synchronization is an
intentional mode, not an absence of control. Share trajectory primitives where
their semantics match; retain separate plan/state types where they differ.

Playback is another source of axis samples. It enters through a bounded execution
interface and never writes the axis bank directly. It may coexist with independent
live-control axes when their reservations do not conflict. Coupled followers are
part of the reserved set and cannot also be independently commanded.

Application policy decides whether a command may supersede another activity;
the motion subsystem enforces reservations and physical validity. This separates
Dragonframe behavior from execution without duplicating arbitration rules.

An alternative of two public motion modules would duplicate state, limits, stop,
and output coordination. A single undifferentiated planner would entangle their
different algorithms. One subsystem with explicit internal planners avoids both
problems and allows each planner to be tested independently.

## Current implementation

`Motor`/`MotorMove` in `m7/dfx.h`, `calculatePointToPoint()`, `m7/coordinated_motion.*`, and motion update/stop/coupling paths in `m7/m7.ino`.

## Interface and state contracts

- Expose immutable axis snapshots and bounded prepared plans. Preparation validates limits, configuration revisions, current execution generation, group ownership, and coupled followers before mutation.
- Commit a prepared plan synchronously on the control thread. A stale generation rejects commit; successful commit transfers every affected reservation and installs the plan together. Controller state updates after acceptance must be non-failing assignments.
- Independent plans own per-axis segment cursors and reversal/braking state. Coordinated plans own participant sets and shared progress. Neither edits the other planner’s internal fields.
- An accepted tick updates commanded state once and emits one physical sample. Playback supplies reserved-axis samples through this same boundary. Emitted-step observations remain a distinct input, never an assertion of measured mechanical position.
- Stops identify a participant group and reason. Braking retains ownership until completion; coupled followers stay in that ownership set. Failed replacement does not silently release the previous plan.

## Validation required for extraction

- Simulate zero-distance moves, initial velocity, short moves, reversal, braking, and independent axes with different completion times.
- Simulate coordinated limits, compatible/incompatible handoffs, group completion, cancellation, and stopping every participant.
- Reject overlapping reservations, follower conflicts, and stale plans without mutation; verify playback/live-control coexistence.
- Verify bounded tick work and no allocations; compare physical outputs with the legacy implementation before removing it.

## Open questions

- Should group identity be an opaque handle with a generation, and when is it invalidated?
- Which stopping paths can preserve coordinated geometry, and which deliberately brake axes independently?
- How much independent profile code can be shared without changing its existing reversal and endpoint semantics?

## Planned commits

[M08](TODO.md#m08), [M09](TODO.md#m09), [M10](TODO.md#m10), [M11](TODO.md#m11), [M13](TODO.md#m13), [M17](TODO.md#m17).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [virtual controller](virtual-controller.md), [playback move store](playback-move-store.md), [fault stop policy](fault-stop-policy.md), [intercore hardware](intercore-hardware.md).
