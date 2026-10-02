<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Fault and stop policy

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own fault latches, admissible commands while faulted, stop reason/severity,
repeated-stop escalation, and reset/recovery policy. Consume input and execution
events; return decisions to the application coordinator. Do not directly call
every controller through a shared global context.

Keep local validation near the operation: kinematics checks reachability, virtual
control checks aim geometry, and motion checks physical constraints. Fault policy
determines the system response. The motion subsystem implements braking; playback
implements frame-aligned stopping; camera control implements its stop sequence.

Normal stop, virtual-group cancellation, stop-all, and emergency stop need explicit
contracts. Existing `emergency` behavior includes deceleration and must not be
described as immediate power removal. Document present timing and recovery before
changing either. A fault must invalidate future continuation requests as well as
the currently executing plan.

## Current implementation

`hardStop`, `exceptionCode`, stop counters, command gating, kill-switch handling, `stopAll()`, and `stopMotor()` in `m7/m7.ino`.

## Interface and state contracts

- Consume typed faults, stop requests, physical input state, and explicit time. Return allowed-command decisions and stop/recovery actions; do not directly mutate another controller.
- Separate stop reason from the braking implementation. Motion computes motor braking; playback handles frame alignment; camera control executes its specified stop behavior.
- Latch and clear faults under a documented policy. A recovery decision does not itself recreate canceled plans, resume playback, or restart line-jog continuation.

## Validation required for extraction

- Characterize normal stop, repeated stop, emergency input, fault while idle/moving, and reset timing.
- Test the allowed-command matrix while faulted, including reporting and explicit recovery.
- Verify each affected controller loses continuation authority and every affected axis receives the defined stop request.

## Open questions

- Should the existing timed automatic hard-stop clearing remain, and how does it interact with a still-asserted input?
- Which faults stop an axis group versus the whole machine?
- What existing DMC code represents each domain fault without adding a protocol extension?

## Planned commits

[M01](TODO.md#m01), [M19](TODO.md#m19), [M20](TODO.md#m20).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [motion](motion.md), [camera auxiliary io](camera-auxiliary-io.md), [application coordinator](application-coordinator.md).
