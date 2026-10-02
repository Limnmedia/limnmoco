<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# DMC command inventory

[M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)

This inventory covers every non-flag `DMC_MSG_*` code in
[dmc_msg.h](../../../../m7/dmc_msg.h): **37 codes, 31 incoming dispatch branches**.
Three codes are emitted without an incoming handler (`GIO_IN`, `MOTOR_HARD_STOP`,
`RT_END`); three are unsupported (`DMX`, `RT_UPLOAD_MOVE_DMX`, `FAN_CONTROL`).
Some handled requests also have unsolicited producers. Direction matters: an
outgoing event is not an implemented incoming command.

Baseline: `394a6dbdf3f9789d16f70848852cc964eaa618a4`. Source-line references are
for that baseline; symbol names remain the useful references as files move.
All entries are **source-inspected**, not dynamically verified. Runtime packet
captures, state/motion traces, and timing/memory measurements are pending unless
an entry explicitly links evidence. No proposed module API is specified.

Implementation sources beyond the M7 sketch include the
[legacy parser/serializer](../../../../m7/dmc_msg.cpp),
[independent and playback calculations](../../../../m7/motion.cpp),
[coordinated executor](../../../../m7/coordinated_motion.cpp),
[firmware state definitions](../../../../m7/dfx.h),
[shared geometry/trajectory library](../../../../common/LimnmocoIK/src), and
[M4 output engine](../../../../m4/m4.ino). The newer
[standalone packet codec](../../../../common/LimnmocoProtocol/src) is not the active
firmware parser at this baseline.

Read the [shared protocol/path and measurement conventions](README.md) first.
`ACK` below means the current four-byte common ACK payload, not the PDF's WORD.
Fields are little-endian. Expected payload sizes describe layouts, not proof of
exact-length validation. All requests are subject to shared checksum/fault gates.
The [bundled specification](../../../DMC-Protocol-2024-02-13.pdf) is a comparison
source; discrepancies below are questions to characterize, not decisions to fix.

## Index

| Code | Command | Current direction/support |
| --- | --- | --- |
| `0x0001` | [HI](hi.md) | Request + startup event |
| `0x0020` | [DMX](dmx.md) | Unsupported request |
| `0x0021` | [GIO_OUT](gio_out.md) | Request |
| `0x0022` | [GIO_IN](gio_in.md) | Device event; incoming query unsupported |
| `0x0023` | [GIO_CAM](gio_cam.md) | Request |
| `0x0030` | [MOTOR_STATUS](motor_status.md) | Request |
| `0x0031` | [MOTOR_MOVE](motor_move.md) | Request |
| `0x0032` | [MOTOR_STOP](motor_stop.md) | Request |
| `0x0033` | [MOTOR_STOP_ALL](motor_stop_all.md) | Request |
| `0x0034` | [MOTOR_GET_POSITION](motor_get_position.md) | Request + periodic report |
| `0x0035` | [MOTOR_RESET_POSITION](motor_reset_position.md) | Request |
| `0x0036` | [MOTOR_JOG](motor_jog.md) | Request |
| `0x0037` | [MOTOR_CONFIGURE](motor_configure.md) | Request |
| `0x0038` | [MOTOR_SET_SPEED](motor_set_speed.md) | Request |
| `0x0039` | [MOTOR_SET_LIMITS](motor_set_limits.md) | Request |
| `0x003A` | [MOTOR_HARD_STOP](motor_hard_stop.md) | Device event; incoming unsupported |
| `0x0100` | [RT_UPLOAD_MOVE_BEGIN](rt_upload_move_begin.md) | Request |
| `0x0101` | [RT_UPLOAD_MOVE_AXIS](rt_upload_move_axis.md) | Request |
| `0x0102` | [RT_UPLOAD_MOVE_DMX](rt_upload_move_dmx.md) | Unsupported request |
| `0x0103` | [RT_UPLOAD_MOVE_END](rt_upload_move_end.md) | Request |
| `0x0104` | [RT_UPLOAD_MOVE_TRIGGERS](rt_upload_move_triggers.md) | Request |
| `0x0110` | [RT_POSITION_FRAME](rt_position_frame.md) | Request |
| `0x0111` | [RT_RUN_MOVE](rt_run_move.md) | Request |
| `0x0112` | [RT_SHOOT_FRAME](rt_shoot_frame.md) | Request |
| `0x0113` | [RT_GO](rt_go.md) | Request + input-originated event |
| `0x0114` | [RT_END](rt_end.md) | Device event; incoming unsupported |
| `0x0115` | [RT_SHOOT_FRAME2](rt_shoot_frame2.md) | Request |
| `0x0116` | [RT_STOP_LOOP](rt_stop_loop.md) | Request |
| `0x0120` | [RT_JOG_ALL](rt_jog_all.md) | Request |
| `0x0200` | [VIRT_CONFIG](virt_config.md) | Request |
| `0x0201` | [VIRT_MOVE](virt_move.md) | Request |
| `0x0202` | [VIRT_STOP](virt_stop.md) | Request |
| `0x0203` | [VIRT_JOG](virt_jog.md) | Request |
| `0x0205` | [VIRT_GET_POSITION](virt_get_position.md) | Request + periodic report |
| `0x0206` | [VIRT_JOG_ON_LINE](virt_jog_on_line.md) | Request with continuing execution |
| `0x0207` | [VIRT_AIM_POINT](virt_aim_point.md) | Request |
| `0x0300` | [FAN_CONTROL](fan_control.md) | Unsupported request; header only |

## Common response and unknown-code paths

`DMC_MSG_FLAG_ACK` is a response flag, not a separate command. The shared footer
in `loop()` builds `request type | 0x8000`, keeps the request ID, and emits a
result code unless a handler sets `responseCode` to zero. Some handlers emit a
data report and still leave the ACK enabled; those cases are listed explicitly.

A parser-accepted unrecognized type reaches the unsupported branch after the
checksum and hard-stop gates. Invalid framing/oversize/type filtering can instead
discard bytes without a response. Characterize these paths separately from a
known-but-unsupported command. Inbound ACK-flagged types also need a host fixture;
they have no ordinary command branch.

The older line-jog code `0x0204` is described as superseded in the PDF history and
is not defined or handled in this firmware. The PDF capability summary also uses
the name `MSG_GO_MOTION2`; its command section identifies `RT_SHOOT_FRAME2` at
`0x0115`, so it is not counted as another established code.

For each entry, add evidence rows by scenario using the template in the M01 guide.
Keep rejected, no-op, and event-only cases as first-class observations.


## Command records

| [HI](hi.md) | `HI` |
| [DMX](dmx.md) | `DMX` |
| [GIO_OUT](gio_out.md) | `GIO_OUT` |
| [GIO_IN](gio_in.md) | `GIO_IN` |
| [GIO_CAM](gio_cam.md) | `GIO_CAM` |
| [MOTOR_STATUS](motor_status.md) | `MOTOR_STATUS` |
| [MOTOR_MOVE](motor_move.md) | `MOTOR_MOVE` |
| [MOTOR_STOP](motor_stop.md) | `MOTOR_STOP` |
| [MOTOR_STOP_ALL](motor_stop_all.md) | `MOTOR_STOP_ALL` |
| [MOTOR_GET_POSITION](motor_get_position.md) | `MOTOR_GET_POSITION` |
| [MOTOR_RESET_POSITION](motor_reset_position.md) | `MOTOR_RESET_POSITION` |
| [MOTOR_JOG](motor_jog.md) | `MOTOR_JOG` |
| [MOTOR_CONFIGURE](motor_configure.md) | `MOTOR_CONFIGURE` |
| [MOTOR_SET_SPEED](motor_set_speed.md) | `MOTOR_SET_SPEED` |
| [MOTOR_SET_LIMITS](motor_set_limits.md) | `MOTOR_SET_LIMITS` |
| [MOTOR_HARD_STOP](motor_hard_stop.md) | `MOTOR_HARD_STOP` |
| [RT_UPLOAD_MOVE_BEGIN](rt_upload_move_begin.md) | `RT_UPLOAD_MOVE_BEGIN` |
| [RT_UPLOAD_MOVE_AXIS](rt_upload_move_axis.md) | `RT_UPLOAD_MOVE_AXIS` |
| [RT_UPLOAD_MOVE_DMX](rt_upload_move_dmx.md) | `RT_UPLOAD_MOVE_DMX` |
| [RT_UPLOAD_MOVE_END](rt_upload_move_end.md) | `RT_UPLOAD_MOVE_END` |
| [RT_UPLOAD_MOVE_TRIGGERS](rt_upload_move_triggers.md) | `RT_UPLOAD_MOVE_TRIGGERS` |
| [RT_POSITION_FRAME](rt_position_frame.md) | `RT_POSITION_FRAME` |
| [RT_RUN_MOVE](rt_run_move.md) | `RT_RUN_MOVE` |
| [RT_SHOOT_FRAME](rt_shoot_frame.md) | `RT_SHOOT_FRAME` |
| [RT_GO](rt_go.md) | `RT_GO` |
| [RT_END](rt_end.md) | `RT_END` |
| [RT_SHOOT_FRAME2](rt_shoot_frame2.md) | `RT_SHOOT_FRAME2` |
| [RT_STOP_LOOP](rt_stop_loop.md) | `RT_STOP_LOOP` |
| [RT_JOG_ALL](rt_jog_all.md) | `RT_JOG_ALL` |
| [VIRT_CONFIG](virt_config.md) | `VIRT_CONFIG` |
| [VIRT_MOVE](virt_move.md) | `VIRT_MOVE` |
| [VIRT_STOP](virt_stop.md) | `VIRT_STOP` |
| [VIRT_JOG](virt_jog.md) | `VIRT_JOG` |
| [VIRT_GET_POSITION](virt_get_position.md) | `VIRT_GET_POSITION` |
| [VIRT_JOG_ON_LINE](virt_jog_on_line.md) | `VIRT_JOG_ON_LINE` |
| [VIRT_AIM_POINT](virt_aim_point.md) | `VIRT_AIM_POINT` |
| [FAN_CONTROL](fan_control.md) | `FAN_CONTROL` |
