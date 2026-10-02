<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Device

[Architecture overview](README.md) · [HI inventory](M01/dmc-commands/hi.md)

Status: agreed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own the device's identity and immutable description for the selected firmware
build and board: product name, firmware version, supported functions, and the
assembled resource-capacity description. This gives HI an explicit data owner.
Control requests or routes this information; it does not own it.

Device is not a repository for every module's configuration. Mutable axis
settings remain in Motion subsystem; geometry, mapping, calibration, and aim state
remain in Virtual; uploaded data remains in Playback. Persistence, if introduced,
must respect those owners.

Device can be immutable data assembled at build/startup time. No mutable
singleton, class hierarchy, or additional runtime task is implied.

## HI field ownership

| HI information | Source of truth and responsibility |
| --- | --- |
| Device name and firmware version | Device, initialized from product/build metadata. Firmware version is distinct from the DMC protocol version. |
| Motor capacity | Motion subsystem capacity for the selected build; Device incorporates the same value, not an independently maintained constant. |
| DMX, GIO, and physical-limit capacities | Selected feature/board configuration from Camera and Intercore as applicable; Device exposes the supported capacities. Unsupported facilities remain zero. |
| Uploaded frame capacity | Playback storage capacity; the value in Device must agree with allocated storage. |
| Implemented feature set | Device describes the selected build using the capabilities of the participating modules. It is not inferred from whichever modules happen to report activity. |
| DMC capability bits and compatibility exceptions | Transport (command-adapter role) maps supported features to advertised DMC flags. Keep the current extra virtual flags as an explicit compatibility policy, distinct from actual supported configuration types. |
| DMC protocol version, field layout, padding, and numeric encoding | DMC. It encodes supplied values; it does not own the product identity or runtime configuration. |
| Request ID and unsolicited startup ID | Transport (command-adapter role); DMC serializes them. |

Assembling a read-only profile must not create competing sources of truth. Use
the owners' build constants/descriptions and validate the assembled values against
the actual compiled capacities. HI should not require polling every controller
on each request. If a future feature makes advertised capacity dynamic, revisit
that field's snapshot contract explicitly.

## Proposed module participation

Request:

```text
Transport (transport role)
  -> DMC
  -> Transport (command-adapter role)
  -> Control
  -> Device
```

Response:

```text
Device
  -> Control
  -> Transport (command-adapter role)
  -> DMC
  -> Transport (transport role)
```

Control schedules the startup announcement using the same profile;
there is no received packet for that event. Consultation with Errors
is listed separately in the inventory, and does not transfer ownership of HI data.
These paths list responsibilities rather than prescribing calls or APIs.

## Current implementation

`sendHello()` in `m7/m7.ino` assembles identity, counts, capabilities, and protocol
version inline. Version and capacity macros also live in `m7/dfx.h`; the same
capacities influence array allocation and motion code. Extract their ownership
without introducing duplicate literal values.

## Validation considerations

- Preserve the characterized HI and startup bytes, including IDs and the current
  compatibility advertisement, until a separate behavior change is decided.
- Verify advertised capacities against actual motion, I/O, and upload storage.
- Verify that supported features and compatibility-advertised flags remain
  distinguishable, especially for the extra virtual capability bits.
- Keep the profile independent of DMC types and Arduino APIs.
- Verify HI queries do not mutate runtime controller configuration or motion.

## Open questions and implementation planning

- Confirm the Device implementation boundary and retain its agreed ownership
  while selecting an implementation API.
- Decide where product/build metadata and board-selected capacities are assembled
  without making Control their owner.
- Record precisely which advertisement rules are required for Dragonframe
  compatibility and which belong to the actual supported feature set.

Extract Device before migrating HI in M07, alongside the M06/M07 codec/adapter
work. Its ownership is agreed; the implementation API remains open for design
during extraction.

## Discussion and decisions

M01 review: configuration ownership was missing from the HI path. Control's
routing role does not permit it to become the owner of other
modules' state. DMC's ownership of representation likewise does not
make it the owner of represented device data. Device owns the immutable
device description; runtime configuration remains local
to its domain owner.
