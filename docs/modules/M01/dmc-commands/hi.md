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

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.
