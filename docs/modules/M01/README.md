<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# M01: command and execution baseline

[Architecture overview](../README.md) · [Global TODO: M01](../TODO.md#m01)

The [DMC command inventory](dmc-commands/README.md) lists all 37 message codes defined
by the current firmware, including device-originated messages and unsupported
codes. Each entry covers protocol, state, motion, performance considerations,
current implementation, and the proposed participating modules. Module paths
describe responsibilities, not API signatures, class names, or an agreed call
graph.

Source baseline: `394a6dbdf3f9789d16f70848852cc964eaa618a4` on `modules`.
The initial entries come from source inspection and the bundled
[DMC protocol document](../../DMC-Protocol-2024-02-13.pdf). They are not runtime
measurements or proof of hardware behavior. M01 remains open until its fixtures,
transition records, and available performance evidence are collected.

## How to use the inventory

Keep three things separate:

- **Specification:** what the bundled protocol says the host/device should do.
- **Observed implementation:** what the baseline source does, later confirmed
  or corrected by traces and tests.
- **Proposed module ownership:** where that responsibility belongs after
  extraction. A proposed route does not approve changing current behavior.

Each command has a stable heading matching its `DMC_MSG_*` suffix. Add scenario
and evidence links to that entry as work proceeds. Keep raw packet fixtures and
executable characterization tests in `tests/`; link them here rather than storing
another copy. Place measurement records here or in a linked hardware acceptance
location, with the firmware hash and test conditions attached.

## Shared current paths

Incoming requests use this path in [M7](../../../m7/m7.ino):

```text
Serial receive (up to 64 bytes per loop; mirror to Serial1)
  -> dmc_msg_process_character (framing/checksum)
  -> loop checksum and hardStop gates
  -> command branch and helpers
  -> typed reply and/or common ACK footer
  -> writeOutputMessage (queue)
  -> transmitMessages on subsequent loop passes (up to 32 bytes; Serial + Serial2)
```

The loop breaks after one completed command. Motion is updated before command
handling. Receipt, acceptance, first output publication, and physical completion
are separate events. A successful ACK alone does not prove that stepping has
started or that a motor reached its target.

Motion normally continues through `updateMotorVelocities()`, independent or
coordinated execution, coupling expansion, and shared-memory publication. M4
consumes speeds on its 50 Hz boundary and generates pulses in its 200 kHz timer
callback. Camera phase output also runs there. Accumulated steps are emitted-step
accounting, not measured crane position.

## Shared protocol observations

- Frames contain a two-byte marker, 32-bit ID, 16-bit type, 16-bit payload length,
  payload, and two checksum bytes, using little-endian fields. Total frame size
  is payload size plus 12 bytes.
- The firmware parser allocates a 2,048-byte input buffer and a 256-byte response
  buffer at initialization. Its maximum accepted input payload is 2,036 bytes.
  The output ring holds 1,024 bytes. These are source constants, not measured
  peak memory consumption.
- The common ACK footer sets the type's `0x8000` bit, retains the request ID, and
  writes a **four-byte** result code. The PDF specifies a two-byte code. Capture
  the actual host/device exchange before deciding whether anything should change.
- Payload layouts in the inventory describe expected fields/read order. They
  do **not** imply that the handler currently validates the exact length. Legacy
  readers can return zero when data is missing, and many branches do not check
  for trailing data. Characterize malformed input in a host harness before any
  hardware exercise involving motion or array indexing.
- The global hard-stop gate rejects codes at or above `MOTOR_MOVE`, except
  `MOTOR_GET_POSITION`, before normal dispatch. This includes virtual position
  queries and even some stop/unsupported codes. Earlier codes such as status and
  GIO are not blocked by this numeric gate. Bad checksums take precedence.
- `writeOutputMessage()` can drop an output frame when space is insufficient;
  enqueuing and physical transmission need separate evidence. Unsupported incoming
  codes reach the common error ACK unless rejected earlier or discarded by the
  parser's type/length filter.

## Proposed route notation

Use the full module names from the architecture plan in every command entry.
Expand request and response paths explicitly; do not substitute "ingress",
"reply path", "hardware", or other aliases for participating modules. Retain this
convention as new commands and scenarios are documented.

Transport contains two roles. Write `Transport (transport role)` or
`Transport (command-adapter role)` when a
path needs to distinguish byte delivery from command translation. These roles
are not newly named modules.

Each command lists its complete request and response paths, command-gating
consultation, and any separate event or continuing-execution paths. Arrows show
responsibility/data flow, not an API or required synchronous call order. A plus
sign joins collaborating module owners without selecting their invocation order.
Returning information through Control does not make it the
information's owner. Command-gating consultation does not imply all requests
must be rejected while faulted; the command-specific policy remains to be recorded.

| Module name | Boundary document |
| --- | --- |
| DMC | [DMC](../DMC.md) |
| Transport | [Transport](../transport.md) |
| Mathematics | [Mathematics](../mathematics.md) |
| Kinematics and coordinate geometry | [Kinematics and coordinate geometry](../kinematics.md) |
| Calibration | [Calibration](../calibration.md) |
| Motion subsystem | [Motion subsystem](../motion.md) |
| Virtual | [Virtual](../virtual.md) |
| Playback | [Playback](../playback.md) |
| Errors | [Errors](../errors.md) |
| Camera | [Camera](../camera.md) |
| Intercore | [Intercore](../intercore.md) |
| Control | [Control](../control.md) |
| Device | [Device](../device.md), the agreed owner of immutable device identity, firmware metadata, and the assembled HI description. |

Mathematics is listed as a supporting dependency where relevant; it is not a
stateful command owner. Device-originated events start at their state owner and
have their own explicit path through Transport and Packet
protocol. Unsupported incoming features keep their rejection paths without
inventing new implementation modules.

For HI, Device owns identity/firmware metadata and the
assembled immutable description. Runtime configuration remains in its domain
module. DMC owns the DMC protocol version and representation;
Transport owns DMC field mapping and compatibility flags.
Control routes the request and startup announcement.

## What to record for each scenario

| Dimension | Record |
| --- | --- |
| Setup | Firmware hash, board/core, compiler/core package, optimization, clocks, logging/mirrors, transport, calibration, axis configuration, active mode, and pending motion. |
| Protocol | Exact request bytes and ID; every reply/event in order; lengths, signed fields, scales, errors, reply IDs, and any dropped/coalesced output. |
| State | Before/after configuration, mode, targets, ownership/participants, aim/jog state, upload/frame state, camera/GIO state, and fault latch. Include rejected requests. |
| Motion | Planned axis targets and velocity, first changed output interval, trajectory samples, cancellation/reversal/stop behavior, and final emitted-step count. Record physical measurements separately. |
| Performance | Parser/dispatch/encode CPU time, queue residence, request-to-response latency, control-tick/ISR cost, and incremental stack/heap use where measurable. Include packet/axis/record counts. |
| Evidence | Fixture/test/trace paths, apparatus and instrumentation overhead, run count and timing distribution, verdict, unexplained differences, and module discussions needing a decision. |

Measure stages separately when possible:

1. First-to-last request byte arrival: transport receive duration.
2. Last byte accepted to decoded packet: framing/checksum work. Also sum parser
   CPU time across chunks; elapsed time between chunks includes host pacing.
3. Dispatch entry to handler return: validation, state changes, and plan creation.
4. Response encoding/enqueue: separate from waiting in the output queue.
5. First-to-last reply byte transmission: transport send duration.
6. Acceptance to first changed M4 output and eventual completion: motion latency
   and execution, not command-processing latency.

The existing code interleaves some of these stages; instrument actual boundaries
without requiring the new architecture first. Keep instrumentation disabled/enabled
comparisons so tracing overhead is visible. Record counts and min/median/p95/max
only when the sample size supports them; an observed maximum is not a proven
worst-case execution time. Flash/static RAM and peak stack/heap are also recorded
for the complete firmware. Per-command costs are additional observations, not
independently additive estimates of total RAM.

## Scenario record template

```text
Scenario ID / command:
Baseline commit / board / build / logging / transport:
Initial configuration and active mode:
Request bytes or event stimulus:
Expected from specification:
Observed replies/events and state delta:
Motion/output trace and completion condition:
Timing stages / units / sample count / distribution:
Stack or heap delta / firmware memory context:
Evidence paths and reproduction steps:
Difference requiring discussion / proposed module owners:
Status: planned | captured | reproduced | decision needed
```

## Initial cross-command scenarios

- Configure axes and limits, move/jog from rest and while moving, reverse, stop,
  repeat stop-all, and query/reset position.
- Upload, position, preroll, GO, loop/ping-pong, stop-loop, shoot both variants,
  and compare completion events with actual deceleration completion.
- Configure virtuals, move/jog with and without aim, change aim offsets, line-jog
  through multiple horizons, supersede it, then issue stop/fault/reconfiguration.
- Reject malformed/unsupported requests and invalid configurations; capture any
  state change on rejection instead of assuming rollback.
- Exercise live-control axes alongside playback, coupled followers, queue
  congestion, periodic reporting, and sustained command input during motion.

The first pass is an inventory and source-derived set of questions. No timing,
memory, wire capture, or hardware result is filled in by inference.
