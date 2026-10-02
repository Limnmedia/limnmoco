<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Uploaded move store and playback controller

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

The move store owns upload state, frame/axis bounds, track data, trigger data,
and the committed dataset's validity. It presents read-only tracks to playback.
An incomplete upload must not become an executable move.

The playback controller owns frame time, preroll/wait/shoot/postroll, live run,
loop/ping-pong, jog-all, overrides, and synchronization of motion with exposure
and trigger events. Move `evaluateMove()` and the Go Motion calculations here as
pure helpers where possible. A frame clock is not a physical motor even if it
temporarily reuses the same profile algorithm.

Keep these separate components: storage changes for upload and memory reasons;
playback changes for timeline and shooting behavior. Do not copy the full dataset
when preparing playback. Eight 10,000-entry int32 tracks alone use 320,000 bytes.
Initially prohibit playback during replacement uploads and invalidate the old
dataset on upload begin; double-buffering requires a demonstrated memory budget
and an explicit change to upload semantics.

Uploaded real-axis trajectories must bypass virtual aim/IK. Virtual control and
playback are peers, not one another's implementation dependencies.

## Current implementation

`AxisMoveData`, `GoMotionMove`, overrides, and frame state in `m7/dfx.h`; Go Motion/interpolation helpers in `m7/motion.cpp`; upload and playback handlers in `m7/m7.ino`.

## Interface and state contracts

- The store exposes begin/write/finish/abort and a generation-tagged read-only committed view. Validate every frame and axis range; no consumer can execute an incomplete generation.
- Use the existing fixed storage initially. Explicitly invalidate replacement uploads rather than promising rollback of an entire dataset without memory for a second copy. Packet-level rejection must still avoid unintended partial writes.
- Playback owns the frame clock and timeline state, reserves its axis set through motion, and produces physical samples plus camera/trigger intents for an accepted tick.
- Uploaded physical tracks bypass virtual geometry. Live-control axes use independent reservations. Completion, pause/stop, preroll, and loop transitions have explicit events and ownership changes.

## Validation required for extraction

- Test upload limits, truncated records, incomplete/aborted/replaced generations, frame indexing, and playback during upload.
- Test interpolation, preroll, exposure windows, Go Motion variants, looping/ping-pong, and frame-aligned stop.
- Compare live overrides and mixed playback/independent axis samples; verify that no aim transform is applied.
- Measure peak RAM including parser, store, and prepared plans; do not silently reduce supported frame count.

## Open questions

- Does existing upload begin always invalidate prior playback, and what response should an interrupted upload produce?
- Who wins when a direct camera/GIO command overlaps a playback-owned exposure or trigger?
- Which end-frame and interpolation boundary conventions must remain byte-for-byte compatible in position replies?

## Planned commits

[M15](TODO.md#m15), [M17](TODO.md#m17), [M18](TODO.md#m18).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [motion](motion.md), [camera auxiliary io](camera-auxiliary-io.md), [fault stop policy](fault-stop-policy.md), [application coordinator](application-coordinator.md).
