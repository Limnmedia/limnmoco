<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# HI

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0001` · Request + startup event.

**Protocol:** Empty request. Same-type, same-ID reply: 32-byte name, version bytes, motor/DMX/GIO/limit counts, frame capacity, capabilities, and protocol version (51-byte payload). No additional ACK. Startup sends ID 0.

**State:** Reads device identity/constants; output queuing changes transport state. The advertised virtual capability bits exceed the supported configuration types as a documented Dragonframe compatibility workaround. The current constants are distributed across `sendHello()` and `dfx.h`; ownership is documented in the agreed [Device](../../device.md) boundary, with wire-advertisement policy kept in Transport.

**Motion/output:** None requested; inspect startup ordering relative to output initialization.

**Performance considerations:** Fixed-size formatting/checksum and 63-byte framed output; measure startup separately from a request during motion.

**Current code path:** Incoming branch at baseline line 625 in [m7.ino](../../../../m7/m7.ino). `sendHello(msgId)`; `setup()` also calls `sendHello(0)`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Device.
- Response: Device → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Startup announcement: Control → Device → Control → Transport (command-adapter role) → DMC → Transport (transport role).

Device owns the device description. Control only routes the query or startup announcement. Transport maps capabilities to DMC advertisement; DMC supplies protocol-version/layout information and encodes the fields. Device is an agreed boundary; its implementation API remains to be designed.

**Characterization cases:** Exact identity bytes/capabilities; requested ID versus startup ID; repeated hello under queue pressure.

**Captured evidence (NS movement trace):** The external capture
`virt_NS_move_5mm_BCT_v6.csv` records a valid HI exchange at approximately
11.386 seconds into the trace. The trace was produced by an earlier firmware
commit than the current source baseline; its exact firmware commit is not yet
identified. The CSV contains decoded DMC fields rather than the raw serial
bytes (`data` is empty for these rows), so this is a decoded protocol record,
not a byte-for-byte fixture.

| Direction | Start (s) | Duration (s) | ID | Type | Payload length | Result |
| --- | ---: | ---: | --- | --- | ---: | --- |
| Request | 11.385686083 | 0.001037542 | `0x0001` | `0x0001` (`MSG_HI`) | 0 | Valid, checksum valid |
| Response | 11.386570000 | 0.005465750 | `0x0001` | `0x0001` (`MSG_HI`) | 51 (`0x33`) | Valid, checksum valid |

The decoded response fields are: name `limnmoco`; firmware `0.1.2`; 8 motors;
0 DMX channels; 2 GIO outputs; 1 GIO input; 0 hardware limits; upload capacity
10,000 frames (`0x2710`); capabilities `0x07BF`; and protocol version `0x0002`.
The response begins 0.883917 ms after the request start and occupies 5.465750
ms according to the trace timestamps. These timings should be treated as
instrumentation observations until the capture tool's event-timing semantics
are confirmed.

The captured capability mask is `0x07BF`. The current source advertises
`DMC_CAP_AIM_POINT` (`0x0040`) as part of a `0x07FF` mask. Because this capture
predates the current source baseline, the difference is recorded as historical
firmware behavior. M01 should identify the capture's source commit and retain
the comparison as a versioned protocol observation.

**Evidence still pending:** raw serial-byte fixture; state/motion or no-op trace;
and repeatable timing/memory measurements.
