<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Modularization TODO

[Architecture overview](README.md)

This is the global implementation checklist for `modules`. The branch starts at
`main` commit `d4a0a34`. M00 is complete; M01–M25 remain pending. The module
boundaries and commit sequence have been reviewed and accepted as the working
plan, subject to findings during implementation. Unchecked commit subjects are
planning labels, not claims that commits exist.

Check an item only after its implementation and stated verification are complete,
then append the actual commit hash and evidence. For an item completed in the
same commit as its record, identify that commit by subject and add its hash in
the next checklist update. Split a large item into commits
with suffixes such as `M06a`/`M06b`, preserving links and recording scope. Module
files own design discussion; this file owns ordering and completion status.

## Working rules

- Preserve current observable behavior during extraction. Track discovered bugs
  and desired policy changes as separate fixes with explicit acceptance criteria.
- Keep every commit buildable. Use temporary forwarding adapters, remove them
  when consumers migrate, and never allow legacy and new code to drive the same
  axis simultaneously.
- Run relevant host tests for each implementation step and compile both firmware
  targets for firmware/build-boundary changes. Record hardware evidence when an
  item changes physical timing, inter-core behavior, or stop execution. Do not
  require unavailable hardware for an unrelated pure-code change.
- Keep numerical, wire, and physical-motion behavior stable separately. Do not
  combine a new solver model, wire format, or inter-core handshake with extraction.

## Phase overview

| Phase | Work | Completion evidence |
| --- | --- | --- |
| 0. Characterize | Record command/state and stop/supersession behavior, timing, RAM, and wire fixtures. Identify intentional fixes separately from refactoring. | Existing suites and both firmware builds pass; representative direct, virtual, playback, stop, and camera traces are captured. |
| 1. Establish pure boundaries | Separate math, kinematics, calibration helpers, wire conversion, and trajectory primitives through narrow headers and build targets. Retain compatibility wrappers temporarily. | Existing numeric fixtures pass unchanged; portable targets do not include Arduino or global firmware state. |
| 2. Consolidate protocol | Add bounded streaming parsing, typed configuration/command/response codecs, and transport/dispatch adapters. Replace one command family at a time. | Exact wire parity, fragmentation/resynchronization, malformed lengths, signed values, and response ownership are tested. |
| 3. Establish motion ownership | Introduce axis snapshots/configuration and an instance-based execution facade. Wrap then extract existing independent and coordinated paths; place coupling and reservations behind the facade. | Host simulation covers current velocity, reversal, limits, handoff, group stop, followers, and live-axis coexistence. No second writer remains for migrated axes. |
| 4. Extract virtual control | Move configuration/origin, aim, jog state, and target transactions behind the motion port. | Packet-to-state tests cover rejection without mutation, optional roll, reconfiguration, supersession, and continuation failures. |
| 5. Extract playback and I/O | Separate move storage, timeline/shooting state, camera/trigger sequencing, and frame-aligned stopping. Route execution through motion ownership. | Upload bounds and invalidation, loop/ping-pong, live overrides, triggers, and stop sequences match characterized behavior. |
| 6. Complete application policy | Centralize fault/recovery and cross-controller command policy; remove legacy shared globals and DMC dependencies from domain code. | Cross-mode sequence tests verify exactly one owner and defined responses for cancellation, fault, and recovery. |
| 7. Isolate the hardware boundary | Consolidate the shared ABI and M7 link adapter, then extract M4 pulse arithmetic while retaining interrupt timing. | Both cores compile against one ABI; deterministic link/pulse tests pass; hardware verifies pulse timing, camera phase, resets, and stopping. |
| 8. Enforce boundaries | Add CI/build dependency checks, refresh architecture and acceptance docs, remove transitional wrappers. | Host tests and firmware compile checks run routinely; hardware acceptance and timing/memory results are versioned. |

Introduce thin fault and output ports when earlier phases need them; later phases
complete their extraction. Avoid simultaneous changes to the planner algorithm,
DMC wire format, and inter-core handshake. Each migration step leaves the firmware
buildable and gives an existing behavior one clear owner.

## Planned commit series

Run in the order below by default. Dependencies name required earlier milestones;
existing legacy adapters keep unrelated paths working until their migration.

### M00

- [x] `docs: organize modular architecture and implementation roadmap`

Scope: move the overview, give all 12 boundaries a discussion document, and
establish this checklist. Completed with the overview, 12 boundary documents,
project README links, and M00–M25 roadmap. Relative links and anchors, retained
architecture content, and the `modules` branch baseline were verified. No firmware
behavior changes.

### M01

- [ ] `test: characterize firmware commands and motion transitions`

Depends on M00. Record representative wire fixtures and a command-by-active-mode
matrix for direct, virtual, playback, configuration/reset, and stop commands.
Record available build, memory, and timing baselines; distinguish hardware traces
from host fixtures and mark unavailable measurements explicitly. Add narrow
legacy test adapters where needed rather than rewriting behavior to make it fit.
Complete when baseline checks are reproducible and disputed behavior is listed.
Boundaries: [application](application-coordinator.md), [faults](fault-stop-policy.md).

### M02

- [ ] `refactor(math): extract portable vector and matrix primitives`

Depends on M01. Create the [mathematics](mathematics.md) target/library and migrate
existing implementations without changing precision or rotation behavior. Retain
forwarding headers for unmigrated consumers. Complete when existing numerical
fixtures pass, standalone consumers build, and both firmware targets compile.

### M03

- [ ] `refactor(kinematics): isolate geometry and coordinate contracts`

Depends on M02. Give [kinematics](kinematics.md) a narrow API for FK/IK, frame
conversion, aim geometry, and camera-line direction. Retain compatibility wrappers
for target construction and wire conversion until their owners migrate. Complete
when existing FK/IK and coordinate fixtures pass unchanged and pure geometry has
no motor, protocol, or Arduino dependency. Do not add the horizontal-radius model.

### M04

- [ ] `refactor(calibration): extract joint and motor-step mapping`

Depends on M03. Extract [mapping/calibration](axis-mapping-calibration.md), BCT,
SPU, and optional-roll conversion behind immutable inputs. Wrap existing M7
configuration until the virtual controller owns it. Complete when compensated
targets match fixtures, failed conversions leave output unchanged, and raw BCT
steps cannot accidentally receive SPU scaling twice.

### M05

- [ ] `refactor(protocol): add bounded streaming frame codec`

Depends on M01. Extend [protocol](packet-protocol.md) with an instance-based
incremental parser, explicit buffer/view lifetimes, and bounded encode/decode.
Keep the existing complete-frame fixtures as wire references. Complete when
fragmentation, corruption, resynchronization, maximum lengths, and independent
parser instances are tested with no controller mutation or per-byte allocation.

### M06

- [ ] `refactor(protocol): define typed command and response codecs`

Depends on M05. Add configuration, motor, virtual, upload/playback, camera/GIO,
ACK, and event payloads in independently reviewable command-family commits.
Move wire scales and aim fixed-point conversion out of the geometry package.
Complete when each family has exact wire fixtures, malformed-shape rejection,
signed/range checks, and explicit response behavior. Split this item as needed;
the current virtual packet prototypes do not constitute complete coverage.
Boundaries: [protocol](packet-protocol.md), [command adapter](transport-command-adapter.md).

### M07

- [ ] `refactor(m7): separate transport and command dispatch`

Depends on M06. Introduce [transport/command adapters](transport-command-adapter.md),
bounded queue policy, and typed request/result mapping. Route each command family
to narrow legacy wrappers until its controller migrates. Complete when wire
parity, exactly one immediate outcome, and congestion behavior pass tests while
the old and new paths cannot both execute a command. Preserve serial mirroring
and loop budgets unless a separate change is justified.

### M08

- [ ] `refactor(motion): establish axis state and execution ownership`

Depends on M04 and M07. Introduce the [motion](motion.md) facade, immutable axis
snapshots, configuration revisions, and group handles. Adapt all existing axis
writers, including playback, behind a temporary internal legacy executor before
claiming exclusive ownership. Complete when no application consumer can obtain
mutable axis state and reservation/commit rejection leaves existing motion intact.

### M09

- [ ] `refactor(motion): extract independent planning and execution`

Depends on M08. Move independent point-to-point, jog, segment progression, and
braking into instance-owned planner/executor state. Preserve reversal, initial
velocity, and endpoint semantics. Complete when deterministic simulations match
legacy traces for short moves, reversals, stopping, and independently moving axes.
Boundary: [motion](motion.md).

### M10

- [ ] `refactor(motion): extract coordinated execution and handoff`

Depends on M09. Move shared trajectory profiles to the motion package and replace
the static coordinated state with an owned executor. Complete when synchronized
limits, compatible and incompatible handoffs, completion, and group cancellation
are covered, and coordinated execution never clears independent planner internals.
Boundary: [motion](motion.md).

### M11

- [ ] `refactor(motion): centralize coupling and group reservations`

Depends on M10. Reserve leaders and followers together, validate target and
configuration conflicts, and centralize physical sample generation. Complete when
mixed ownership, reversed followers, follower limits, and group stops are tested.
If new rejection rules differ from the characterized firmware, identify them as
separate fixes rather than hiding them in the extraction.
Boundary: [motion](motion.md).

### M12

- [ ] `refactor(virtual): extract configuration and position reporting`

Depends on M04 and M11. Introduce the [virtual controller](virtual-controller.md)
with owned geometry/calibration/mapping, captured origin, and explicit snapshot
selection. Complete when configure/get-position, invalid reconfigure rollback,
absent roll, stale revisions, and coordinate reporting are tested through the
command adapter. Existing move/aim paths use temporary controller-owned adapters.

### M13

- [ ] `refactor(virtual): migrate move jog and stop transactions`

Depends on M12. Route normal virtual targets through geometry, calibration, and
motion preparation/commit. Bind logical axes to motion group identities. Complete
when invalid targets cannot partially change logical state or motion, physical
limits are enforced, and stop/supersession affects every reserved participant.
Boundaries: [virtual](virtual-controller.md), [motion](motion.md).

### M14

- [ ] `refactor(virtual): migrate aim and continuous line jogging`

Depends on M13. Move aim state and pure target builders beside the virtual
controller, then migrate horizon continuation and captured orientation. Complete
when multi-horizon simulation covers aim offsets, no-roll behavior, rejected
handoffs, cancellation, and no restart after supersession. Resolve and document
the next-horizon failure policy before adding any behavior-changing fix; record
its existing-DMC notification mapping separately.
Boundary: [virtual](virtual-controller.md).

### M15

- [ ] `refactor(playback): extract bounded uploaded move storage`

Depends on M07 and M08. Give [move storage](playback-move-store.md) exclusive
ownership of upload bounds, records, trigger data, and dataset validity/generation.
Adapt legacy playback to its read-only view. Complete when interrupted/replaced
uploads and record bounds are tested and incomplete datasets cannot execute.
Confirm actual memory usage; do not allocate a second full dataset implicitly.

### M16

- [ ] `refactor(io): extract camera trigger and input state`

Depends on M07. Extract [camera/GPIO](camera-auxiliary-io.md) countdowns, masks,
blips, and debounce into portable state components with board output adapters.
Expose direct and timeline intents with explicit ownership; retain M4 phase
timing. Complete when pulse/debounce/overlap tests and existing camera sequences
pass. Leave final cross-controller stop policy for M19/M20.

### M17

- [ ] `refactor(playback): extract physical track execution and frame clock`

Depends on M11, M15, and M16. Move interpolation, frame positioning, live run,
jog-all, and loop/ping-pong into [playback](playback-move-store.md). Replace its
internal legacy writer with bounded motion submissions; preserve independent
live-control reservations. Complete when frame boundaries, loops, overrides,
and physical samples match fixtures without passing tracks through virtual IK.

### M18

- [ ] `refactor(playback): isolate shooting and exposure sequencing`

Depends on M17. Move Go Motion variants, preroll/wait/shoot/postroll, synchronized
trigger intents, and frame-aligned stops into playback-owned timeline logic.
Complete when exposure windows, interrupted shots, camera sequences, and stop
alignment are tested using explicit time. Record hardware camera acceptance where
physical timing is affected. Boundaries: [playback](playback-move-store.md),
[camera/GPIO](camera-auxiliary-io.md).

### M19

- [ ] `refactor(control): centralize fault and recovery policy`

Depends on M14 and M18. Extract [fault policy](fault-stop-policy.md), stop reasons,
repeated-stop escalation, command gating, and recovery. Route decisions to motion,
virtual, playback, and I/O owners through the application. Complete when the
faulted-command matrix, cancellation of future work, and recovery timing match
the agreed contracts. Changes to automatic clearing or emergency deceleration
require explicit behavior-fix commits and physical acceptance criteria.

### M20

- [ ] `refactor(app): centralize supersession and event routing`

Depends on M19. Complete the [application coordinator](application-coordinator.md)
and migrate command adapters to domain results/events. Remove duplicated mode
policy from handlers. Complete when cross-mode sequence tests cover direct,
virtual, playback, reconfiguration/reset, stop, and stale events, and every command
has one defined outcome without bypassing motion ownership.

### M21

- [ ] `refactor(intercore): share ABI and isolate M7 output publication`

Depends on M20. Consolidate the current shared layout and move shared-address,
fixed-point conversion, publication, and snapshot access into the
[inter-core adapter](intercore-hardware.md). Add both-core layout assertions and
document current field ownership/synchronization. Complete when firmware builds
and publication/conversion tests pass without changing the ABI. Document any
unresolved atomicity or ordering issue; layout assertions alone do not resolve it.

### M22

- [ ] `refactor(m4): extract pulse engine and board adapters`

Depends on M21. Separate portable pulse/phase transitions from timer interrupts,
GPIO, and board configuration. Complete when pulse arithmetic, direction changes,
pulse completion, and shutter wrap tests pass and hardware confirms actual timing.
Keep M7/M4 handshake changes and any new stale-output safeguard in distinct,
explicitly designed follow-up commits. Boundary: [inter-core/hardware](intercore-hardware.md).

### M23

- [ ] `refactor: remove transitional firmware globals and wrappers`

Depends on M22. Remove legacy parser/executor paths, obsolete M7 math wrappers,
and the omnibus `dfx.h` dependencies as their last consumers migrate. Keep sketch
entrypoints focused on setup/loop wiring. Complete when every mutable state has
one documented owner and portable public headers have no Arduino/global-context
dependency. Preserve compatibility headers only where an identified consumer
still needs them. Boundaries: [overview](README.md), [application](application-coordinator.md).

### M24

- [ ] `ci: enforce module dependencies and firmware build checks`

Depends on M23. Add repeatable host/build checks for the final module graph,
bounded interfaces, and both firmware targets with the board dependency pinned.
Account for Arduino library discovery and sketch source placement. Complete when
clean-checkout checks catch forbidden dependencies and missing source/library
wiring. CI can be introduced earlier incrementally; this milestone makes the
final boundary rules enforceable. Boundaries: [overview](README.md),
[transport](transport-command-adapter.md).

### M25

- [ ] `docs: record modular firmware acceptance and remaining decisions`

Depends on M24. Record firmware hashes, board configuration, calibration, host
results, timing/memory measurements, and hardware acceptance for virtual motion,
playback, stops, camera timing, and the inter-core link. Complete only when the
required hardware evidence exists; otherwise leave it pending and state the gap.
Refresh all module statuses and document remaining intentional differences.
Boundary: [inter-core/hardware](intercore-hardware.md), with evidence linked from
the affected controller documents.

## Follow-up changes to schedule after a decision

These are not silently included in the refactor. Add named commit items and
acceptance criteria once the relevant module discussion resolves them.

- [ ] Decide whether the measured horizontal-radius correction should be
  implemented after kinematics extraction; keep calibration evidence with it.
- [ ] Decide line-jog continuation/failure behavior and its DMC event mapping.
- [ ] Decide any changes to permissive packet validation and queue-overflow behavior.
- [ ] Decide any changes to stop escalation, automatic recovery, and supersession.
- [ ] Resolve inter-core snapshot/reset synchronization and stale-output behavior;
  schedule paired firmware changes and hardware validation where required.
- [ ] Confirm supported boards and acceptance coverage for each.

## Completion record

- M00: completed in this documentation commit,
  `docs: organize modular architecture and implementation roadmap`.
  Verified all 15 Markdown files and 153 local links/anchors, preserved the
  original architecture content and copyright headers, and checked whitespace.
  The plan was reviewed and accepted for implementation. Firmware was unchanged;
  host/firmware tests were not rerun for this documentation-only change.

No firmware implementation milestones are complete yet. Hardware acceptance
remains distinct from host/build success.
