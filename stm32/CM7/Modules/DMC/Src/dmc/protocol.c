/**
 * Copyright (C) 2026 Cade Weinberg
 *
 * This file is part of Limnmoco.
 *
 * Limnmoco is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Limnmoco is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Limnmoco.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <string.h>

#include "config/config.h"
#include "dmc/protocol.h"

void dmc_msg_header_init(DmcHeader *msg, uint32_t id, uint16_t type,
                         uint16_t length) {
  assert(msg != NULL);
  msg->marker[0] = 'D';
  msg->marker[1] = 'F';
  msg->id = id;
  msg->type = type;
  msg->length = length;
}

void dmc_msg_hi_response_init(DmcHiResponse *msg) {
  assert(msg != NULL);
  memset(msg, 0, sizeof(*msg));
  dmc_msg_header_init(&msg->header, 0, DMC_MSG_HI, sizeof(*msg));
  memcpy(msg->name, "limnmoco", sizeof("limnmoco") - sizeof(char));
  msg->fw_major = LIMNMOCO_VERSION_MAJOR;
  msg->fw_minor = LIMNMOCO_VERSION_MINOR;
  msg->fw_rev = LIMNMOCO_VERSION_PATCH;
  msg->motor_count = LIMNMOCO_MOTOR_COUNT;
  msg->dmx_count = LIMNMOCO_DMX_COUNT;
  msg->gio_out_count = LIMNMOCO_GIO_OUT_COUNT;
  msg->gio_in_count = LIMNMOCO_GIO_IN_COUNT;
  msg->hw_limit_count = LIMNMOCO_HW_LIMIT_COUNT;
  msg->upload_frame_count = LIMNMOCO_FRAME_COUNT;
  msg->capabilities = LIMNMOCO_CAPABILITIES;
  msg->protocol_version = LIMNMOCO_PROTOCOL_VERSION;
}

// #NOTE: So these routines are the primitives out of which we will construct
// the rest of the serialize and deserialize routines. Given that they will
// be called over and over, and we know the size of the packet up front,
// (immediately after deserializing the header, or upon entry to the
// serialize routine), we know when it is safe to use an unchecked memcpy.
// so we probably can factor the safety check to the top-level routines,
// and let the primitives just do the copy.

static uint8_t *serialize_u8(uint8_t *cursor, uint8_t value) {
  *cursor++ = value;
  return cursor;
}

static uint8_t *serialize_u16(uint8_t *cursor, uint16_t value) {
  memcpy(cursor, &value, sizeof(value));
  return cursor + sizeof(value);
}

static uint8_t *serialize_u32(uint8_t *cursor, uint32_t value) {
  memcpy(cursor, &value, sizeof(value));
  return cursor + sizeof(value);
}

static uint8_t *serialize_i32(uint8_t *cursor, int32_t value) {
  memcpy(cursor, &value, sizeof(value));
  return cursor + sizeof(value);
}

static const uint8_t *deserialize_u8(const uint8_t *cursor, uint8_t *value) {
  *value = *cursor++;
  return cursor;
}

static const uint8_t *deserialize_u16(const uint8_t *cursor, uint16_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + sizeof(*value);
}

static const uint8_t *deserialize_u32(const uint8_t *cursor, uint32_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + sizeof(*value);
}

static const uint8_t *deserialize_i32(const uint8_t *cursor, int32_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + sizeof(*value);
}

uint8_t *dmc_msg_header_serialize(const DmcHeader *msg, uint8_t *buffer,
                                  uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  memcpy(cursor, msg->marker, sizeof(msg->marker));
  cursor += sizeof(msg->marker);
  cursor = serialize_u32(cursor, msg->id);
  cursor = serialize_u16(cursor, msg->type);
  cursor = serialize_u16(cursor, msg->length);
  return cursor;
}

const uint8_t *dmc_msg_header_deserialize(DmcHeader *msg, const uint8_t *buffer,
                                          uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  memcpy(msg->marker, cursor, sizeof(msg->marker));
  cursor += sizeof(msg->marker);
  cursor = deserialize_u32(cursor, &msg->id);
  cursor = deserialize_u16(cursor, &msg->type);
  cursor = deserialize_u16(cursor, &msg->length);
  return cursor;
}

uint8_t *dmc_msg_ack_serialize(const DmcAck *msg, uint8_t *buffer,
                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u16(cursor, msg->response_code);
  return cursor;
}
const uint8_t *dmc_msg_ack_deserialize(DmcAck *msg, const uint8_t *buffer,
                                       uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u16(cursor, &msg->response_code);
  return cursor;
}

uint8_t *dmc_msg_hi_request_serialize(const DmcHiRequest *msg, uint8_t *buffer,
                                      uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_hi_request_deserialize(DmcHiRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_hi_response_serialize(const DmcHiResponse *msg,
                                       uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  memcpy(cursor, msg->name, sizeof(msg->name));
  cursor += sizeof(msg->name);
  cursor = serialize_u8(cursor, msg->fw_major);
  cursor = serialize_u8(cursor, msg->fw_minor);
  cursor = serialize_u8(cursor, msg->fw_rev);
  cursor = serialize_u8(cursor, msg->motor_count);
  cursor = serialize_u16(cursor, msg->dmx_count);
  cursor = serialize_u8(cursor, msg->gio_out_count);
  cursor = serialize_u8(cursor, msg->gio_in_count);
  cursor = serialize_u8(cursor, msg->hw_limit_count);
  cursor = serialize_u32(cursor, msg->upload_frame_count);
  cursor = serialize_u32(cursor, msg->capabilities);
  cursor = serialize_u16(cursor, msg->protocol_version);
  return cursor;
}

const uint8_t *dmc_msg_hi_response_deserialize(DmcHiResponse *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  memcpy(msg->name, cursor, sizeof(msg->name));
  cursor += sizeof(msg->name);
  cursor = deserialize_u8(cursor, &msg->fw_major);
  cursor = deserialize_u8(cursor, &msg->fw_minor);
  cursor = deserialize_u8(cursor, &msg->fw_rev);
  cursor = deserialize_u8(cursor, &msg->motor_count);
  cursor = deserialize_u16(cursor, &msg->dmx_count);
  cursor = deserialize_u8(cursor, &msg->gio_out_count);
  cursor = deserialize_u8(cursor, &msg->gio_in_count);
  cursor = deserialize_u8(cursor, &msg->hw_limit_count);
  cursor = deserialize_u32(cursor, &msg->upload_frame_count);
  cursor = deserialize_u32(cursor, &msg->capabilities);
  cursor = deserialize_u16(cursor, &msg->protocol_version);
  return cursor;
}

uint8_t *dmc_msg_dmx_request_serialize(const DmcDmxRequest *msg,
                                       uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->ramp) + sizeof(msg->start_channel);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->ramp);
  cursor = serialize_u16(cursor, msg->start_channel);

  const uint32_t values_payload_length =
      msg->header.length - fixed_payload_length;
  if (values_payload_length % sizeof(msg->light_values[0]) != 0) {
    return NULL;
  }
  const uint32_t light_value_count =
      values_payload_length / sizeof(msg->light_values[0]);
  memcpy(cursor, msg->light_values,
         light_value_count * sizeof(msg->light_values[0]));
  return cursor + light_value_count * sizeof(msg->light_values[0]);
}

const uint8_t *dmc_msg_dmx_request_deserialize(DmcDmxRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header)) {
    return NULL;
  }
  const uint8_t *cursor =
      dmc_msg_header_deserialize(&msg->header, buffer, buffer_size);
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->ramp) + sizeof(msg->start_channel);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  cursor = deserialize_u8(cursor, &msg->ramp);
  cursor = deserialize_u16(cursor, &msg->start_channel);
  const uint32_t values_payload_length =
      msg->header.length - fixed_payload_length;
  if (values_payload_length % sizeof(msg->light_values[0]) != 0) {
    return NULL;
  }
  const uint32_t light_value_count =
      values_payload_length / sizeof(msg->light_values[0]);
  memcpy(msg->light_values, cursor,
         light_value_count * sizeof(msg->light_values[0]));
  return cursor + light_value_count * sizeof(msg->light_values[0]);
}

uint8_t *dmc_msg_gio_out_request_serialize(const DmcGioOutRequest *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->triggers);
  return cursor;
}

const uint8_t *dmc_msg_gio_out_request_deserialize(DmcGioOutRequest *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->triggers);
  return cursor;
}

uint8_t *dmc_msg_gio_in_request_serialize(const DmcGioInRequest *msg,
                                          uint8_t *buffer,
                                          uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_gio_in_request_deserialize(DmcGioInRequest *msg,
                                                  const uint8_t *buffer,
                                                  uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_gio_in_response_serialize(const DmcGioInResponse *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->triggers);
  return cursor;
}

const uint8_t *dmc_msg_gio_in_response_deserialize(DmcGioInResponse *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->triggers);
  return cursor;
}

uint8_t *dmc_msg_gio_cam_request_serialize(const DmcGioCamRequest *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->triggers);
  return cursor;
}

const uint8_t *dmc_msg_gio_cam_request_deserialize(DmcGioCamRequest *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->triggers);
  return cursor;
}

uint8_t *
dmc_msg_motor_status_request_serialize(const DmcMotorStatusRequest *msg,
                                       uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_motor_status_request_deserialize(
    DmcMotorStatusRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *
dmc_msg_motor_status_response_serialize(const DmcMotorStatusResponse *msg,
                                        uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->motor_status);
  cursor = serialize_u8(cursor, msg->dmx_status);
  return cursor;
}

const uint8_t *dmc_msg_motor_status_response_deserialize(
    DmcMotorStatusResponse *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->motor_status);
  cursor = deserialize_u8(cursor, &msg->dmx_status);
  return cursor;
}

uint8_t *dmc_msg_motor_move_request_serialize(const DmcMotorMoveRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_i32(cursor, msg->position);
  return cursor;
}

const uint8_t *dmc_msg_motor_move_request_deserialize(DmcMotorMoveRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_i32(cursor, &msg->position);
  return cursor;
}

uint8_t *dmc_msg_motor_move_response_serialize(const DmcMotorMoveResponse *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor_status);
  return cursor;
}

const uint8_t *dmc_msg_motor_move_response_deserialize(
    DmcMotorMoveResponse *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor_status);
  return cursor;
}

uint8_t *dmc_msg_motor_stop_request_serialize(const DmcMotorStopRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  return cursor;
}

const uint8_t *dmc_msg_motor_stop_request_deserialize(DmcMotorStopRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  return cursor;
}

uint8_t *dmc_msg_motor_stop_all_request_serialize(
    const DmcMotorStopAllRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->flags);
  return cursor;
}

const uint8_t *dmc_msg_motor_stop_all_request_deserialize(
    DmcMotorStopAllRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->flags);
  return cursor;
}

uint8_t *dmc_msg_motor_get_position_request_serialize(
    const DmcMotorGetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *
dmc_msg_motor_get_position_request_deserialize(DmcMotorGetPositionRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_motor_get_position_response_serialize(
    const DmcMotorGetPositionResponse *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->move_time);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = serialize_i32(cursor, msg->motor_positions[i]);
  }
  return cursor;
}

const uint8_t *dmc_msg_motor_get_position_response_deserialize(
    DmcMotorGetPositionResponse *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->move_time);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = deserialize_i32(cursor, &msg->motor_positions[i]);
  }
  return cursor;
}

uint8_t *dmc_msg_motor_reset_position_request_serialize(
    const DmcMotorResetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_i32(cursor, msg->position);
  return cursor;
}

const uint8_t *dmc_msg_motor_reset_position_request_deserialize(
    DmcMotorResetPositionRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_i32(cursor, &msg->position);
  return cursor;
}

uint8_t *dmc_msg_motor_jog_request_serialize(const DmcMotorJogRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u16(cursor, msg->speed);
  cursor = serialize_i32(cursor, msg->destination);
  return cursor;
}

const uint8_t *dmc_msg_motor_jog_request_deserialize(DmcMotorJogRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u16(cursor, &msg->speed);
  cursor = deserialize_i32(cursor, &msg->destination);
  return cursor;
}

uint8_t *
dmc_msg_motor_configure_request_serialize(const DmcMotorConfigureRequest *msg,
                                          uint8_t *buffer,
                                          uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u8(cursor, msg->flags);
  return cursor;
}

const uint8_t *
dmc_msg_motor_configure_request_deserialize(DmcMotorConfigureRequest *msg,
                                            const uint8_t *buffer,
                                            uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u8(cursor, &msg->flags);
  return cursor;
}

uint8_t *dmc_msg_motor_set_speed_request_serialize(
    const DmcMotorSetSpeedRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u32(cursor, msg->max_velocity);
  cursor = serialize_u32(cursor, msg->max_acceleration);
  return cursor;
}

const uint8_t *dmc_msg_motor_set_speed_request_deserialize(
    DmcMotorSetSpeedRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u32(cursor, &msg->max_velocity);
  cursor = deserialize_u32(cursor, &msg->max_acceleration);
  return cursor;
}

uint8_t *
dmc_msg_motor_set_limits_request_serialize(const DmcMotorSetLimitsRequest *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u8(cursor, msg->lower_enable);
  cursor = serialize_u32(cursor, msg->lower_limit);
  cursor = serialize_u8(cursor, msg->upper_enable);
  cursor = serialize_u32(cursor, msg->upper_limit);
  cursor = serialize_u8(cursor, msg->hw_set);
  return cursor;
}

const uint8_t *
dmc_msg_motor_set_limits_request_deserialize(DmcMotorSetLimitsRequest *msg,
                                             const uint8_t *buffer,
                                             uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u8(cursor, &msg->lower_enable);
  cursor = deserialize_u32(cursor, &msg->lower_limit);
  cursor = deserialize_u8(cursor, &msg->upper_enable);
  cursor = deserialize_u32(cursor, &msg->upper_limit);
  cursor = deserialize_u8(cursor, &msg->hw_set);
  return cursor;
}

uint8_t *dmc_msg_motor_hard_stop_request_serialize(
    const DmcMotorHardStopRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u16(cursor, msg->response_code);
  return cursor;
}

const uint8_t *dmc_msg_motor_hard_stop_request_deserialize(
    DmcMotorHardStopRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u16(cursor, &msg->response_code);
  return cursor;
}

uint8_t *
dmc_msg_motor_hard_stop_response_serialize(const DmcMotorHardStopResponse *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->reason);
  cursor = serialize_u8(cursor, msg->motor);
  return cursor;
}

const uint8_t *
dmc_msg_motor_hard_stop_response_deserialize(DmcMotorHardStopResponse *msg,
                                             const uint8_t *buffer,
                                             uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->reason);
  cursor = deserialize_u8(cursor, &msg->motor);
  return cursor;
}

uint8_t *dmc_msg_rt_upload_move_begin_request_serialize(
    const DmcRtUploadMoveBeginRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->start_frame);
  cursor = serialize_u32(cursor, msg->end_frame);
  return cursor;
}

const uint8_t *dmc_msg_rt_upload_move_begin_request_deserialize(
    DmcRtUploadMoveBeginRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->start_frame);
  cursor = deserialize_u32(cursor, &msg->end_frame);
  return cursor;
}

uint8_t *dmc_msg_rt_upload_move_axis_request_serialize(
    const DmcRtUploadMoveAxisRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->motor) + sizeof(msg->start_index);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u32(cursor, msg->start_index);

  const uint32_t positions_payload_length =
      msg->header.length - fixed_payload_length;
  if (positions_payload_length % sizeof(msg->positions[0]) != 0) {
    return NULL;
  }
  const uint32_t count_positions =
      positions_payload_length / sizeof(msg->positions[0]);
  for (uint32_t i = 0; i < count_positions; ++i) {
    cursor = serialize_u32(cursor, msg->positions[i]);
  }
  return cursor;
}

const uint8_t *
dmc_msg_rt_upload_move_axis_request_deserialize(DmcRtUploadMoveAxisRequest *msg,
                                                const uint8_t *buffer,
                                                uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header)) {
    return NULL;
  }
  const uint8_t *cursor =
      dmc_msg_header_deserialize(&msg->header, buffer, buffer_size);
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->motor) + sizeof(msg->start_index);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u32(cursor, &msg->start_index);

  const uint32_t positions_payload_length =
      msg->header.length - fixed_payload_length;
  if (positions_payload_length % sizeof(msg->positions[0]) != 0) {
    return NULL;
  }
  const uint32_t count_positions =
      positions_payload_length / sizeof(msg->positions[0]);
  for (uint32_t i = 0; i < count_positions; ++i) {
    cursor = deserialize_u32(cursor, &msg->positions[i]);
  }
  return cursor;
}

uint8_t *dmc_msg_rt_upload_move_request_serialize(
    const DmcRtUploadMoveRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->channel) + sizeof(msg->start_index);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u16(cursor, msg->channel);
  cursor = serialize_u32(cursor, msg->start_index);
  const uint32_t levels_payload_length =
      msg->header.length - fixed_payload_length;
  if (levels_payload_length % sizeof(msg->light_levels[0]) != 0) {
    return NULL;
  }
  const uint32_t count_light_levels =
      levels_payload_length / sizeof(msg->light_levels[0]);
  memcpy(cursor, msg->light_levels,
         count_light_levels * sizeof(msg->light_levels[0]));
  return cursor + count_light_levels * sizeof(msg->light_levels[0]);
}

const uint8_t *dmc_msg_rt_upload_move_request_deserialize(
    DmcRtUploadMoveRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header)) {
    return NULL;
  }
  const uint8_t *cursor =
      dmc_msg_header_deserialize(&msg->header, buffer, buffer_size);
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length =
      sizeof(msg->channel) + sizeof(msg->start_index);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }

  cursor = deserialize_u16(cursor, &msg->channel);
  cursor = deserialize_u32(cursor, &msg->start_index);
  const uint32_t levels_payload_length =
      msg->header.length - fixed_payload_length;
  if (levels_payload_length % sizeof(msg->light_levels[0]) != 0) {
    return NULL;
  }
  const uint32_t count_light_levels =
      levels_payload_length / sizeof(msg->light_levels[0]);
  memcpy(msg->light_levels, cursor,
         count_light_levels * sizeof(msg->light_levels[0]));
  return cursor + count_light_levels * sizeof(msg->light_levels[0]);
}

uint8_t *dmc_msg_rt_upload_move_triggers_request_serialize(
    const DmcRtUploadMoveTriggersRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length = sizeof(msg->mask);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }
  const uint32_t pairs_payload_length =
      msg->header.length - fixed_payload_length;
  const uint32_t pair_wire_size = sizeof(msg->move_frame_values[0].frame) +
                                  sizeof(msg->move_frame_values[0].values);
  if (pairs_payload_length % pair_wire_size != 0) {
    return NULL;
  }
  const uint32_t count = pairs_payload_length / pair_wire_size;
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->mask);
  for (uint32_t i = 0; i < count; ++i) {
    cursor = serialize_u32(cursor, msg->move_frame_values[i].frame);
    cursor = serialize_u32(cursor, msg->move_frame_values[i].values);
  }
  return cursor;
}

const uint8_t *dmc_msg_rt_upload_move_triggers_request_deserialize(
    DmcRtUploadMoveTriggersRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header)) {
    return NULL;
  }
  const uint8_t *cursor =
      dmc_msg_header_deserialize(&msg->header, buffer, buffer_size);
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length = sizeof(msg->mask);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }
  const uint32_t pairs_payload_length =
      msg->header.length - fixed_payload_length;
  const uint32_t pair_wire_size = sizeof(msg->move_frame_values[0].frame) +
                                  sizeof(msg->move_frame_values[0].values);
  if (pairs_payload_length % pair_wire_size != 0) {
    return NULL;
  }
  const uint32_t count = pairs_payload_length / pair_wire_size;
  cursor = deserialize_u32(cursor, &msg->mask);
  for (uint32_t i = 0; i < count; ++i) {
    cursor = deserialize_u32(cursor, &msg->move_frame_values[i].frame);
    cursor = deserialize_u32(cursor, &msg->move_frame_values[i].values);
  }
  return cursor;
}

uint8_t *dmc_msg_rt_upload_move_end_request_serialize(
    const DmcRtUploadMoveEndRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *
dmc_msg_rt_upload_move_end_request_deserialize(DmcRtUploadMoveEndRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_rt_position_frame_request_serialize(
    const DmcRtPositionFrameRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->frame);
  return cursor;
}

const uint8_t *
dmc_msg_rt_position_frame_request_deserialize(DmcRtPositionFrameRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->frame);
  return cursor;
}

uint8_t *dmc_msg_rt_run_move_request_serialize(const DmcRtRunMoveRequest *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->fps);
  cursor = serialize_u32(cursor, msg->start_frame);
  cursor = serialize_u32(cursor, msg->end_frame);
  cursor = serialize_u32(cursor, msg->pre_roll_time);
  cursor = serialize_u32(cursor, msg->post_roll_time);
  cursor = serialize_u8(cursor, msg->sync_dmx);
  cursor = serialize_u32(cursor, msg->bloop_location);
  cursor = serialize_u16(cursor, msg->bloop_dmx_channel);
  cursor = serialize_u16(cursor, msg->bloop_time);
  cursor = serialize_u16(cursor, msg->flags);
  return cursor;
}

const uint8_t *dmc_msg_rt_run_move_request_deserialize(DmcRtRunMoveRequest *msg,
                                                       const uint8_t *buffer,
                                                       uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->fps);
  cursor = deserialize_u32(cursor, &msg->start_frame);
  cursor = deserialize_u32(cursor, &msg->end_frame);
  cursor = deserialize_u32(cursor, &msg->pre_roll_time);
  cursor = deserialize_u32(cursor, &msg->post_roll_time);
  cursor = deserialize_u8(cursor, &msg->sync_dmx);
  cursor = deserialize_u32(cursor, &msg->bloop_location);
  cursor = deserialize_u16(cursor, &msg->bloop_dmx_channel);
  cursor = deserialize_u16(cursor, &msg->bloop_time);
  cursor = deserialize_u16(cursor, &msg->flags);
  return cursor;
}

uint8_t *dmc_msg_rt_shoot_frame_request_serialize(
    const DmcRtShootFrameRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->frame);
  cursor = serialize_u8(cursor, msg->direction);
  cursor = serialize_u32(cursor, msg->exposure_time);
  cursor = serialize_u16(cursor, msg->blur_percent);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = serialize_u8(cursor, msg->motor_blur[i].motor);
    cursor = serialize_u32(cursor, msg->motor_blur[i].position_A);
    cursor = serialize_u32(cursor, msg->motor_blur[i].position_B);
  }
  return cursor;
}

const uint8_t *dmc_msg_rt_shoot_frame_request_deserialize(
    DmcRtShootFrameRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->frame);
  cursor = deserialize_u8(cursor, &msg->direction);
  cursor = deserialize_u32(cursor, &msg->exposure_time);
  cursor = deserialize_u16(cursor, &msg->blur_percent);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = deserialize_u8(cursor, &msg->motor_blur[i].motor);
    cursor = deserialize_u32(cursor, &msg->motor_blur[i].position_A);
    cursor = deserialize_u32(cursor, &msg->motor_blur[i].position_B);
  }
  return cursor;
}

uint8_t *dmc_msg_rt_shoot_frame2_request_serialize(
    const DmcRtShootFrame2Request *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->frame);
  cursor = serialize_u32(cursor, msg->exposure_time);
  cursor = serialize_u16(cursor, msg->open_angle);
  cursor = serialize_u16(cursor, msg->close_angle);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = serialize_u8(cursor, msg->motor_blur[i].motor);
    cursor = serialize_u32(cursor, msg->motor_blur[i].position_A);
    cursor = serialize_u32(cursor, msg->motor_blur[i].position_B);
  }
  return cursor;
}

const uint8_t *dmc_msg_rt_shoot_frame2_request_deserialize(
    DmcRtShootFrame2Request *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->frame);
  cursor = deserialize_u32(cursor, &msg->exposure_time);
  cursor = deserialize_u16(cursor, &msg->open_angle);
  cursor = deserialize_u16(cursor, &msg->close_angle);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    cursor = deserialize_u8(cursor, &msg->motor_blur[i].motor);
    cursor = deserialize_u32(cursor, &msg->motor_blur[i].position_A);
    cursor = deserialize_u32(cursor, &msg->motor_blur[i].position_B);
  }
  return cursor;
}

uint8_t *dmc_msg_rt_go_request_serialize(const DmcRtGoRequest *msg,
                                         uint8_t *buffer,
                                         uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_rt_go_request_deserialize(DmcRtGoRequest *msg,
                                                 const uint8_t *buffer,
                                                 uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_rt_end_request_serialize(const DmcRtEndRequest *msg,
                                          uint8_t *buffer,
                                          uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_rt_end_request_deserialize(DmcRtEndRequest *msg,
                                                  const uint8_t *buffer,
                                                  uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_rt_jog_all_request_serialize(const DmcRtJogAllRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->fps);
  cursor = serialize_u32(cursor, msg->destination);
  return cursor;
}

const uint8_t *dmc_msg_rt_jog_all_request_deserialize(DmcRtJogAllRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->fps);
  cursor = deserialize_u32(cursor, &msg->destination);
  return cursor;
}

uint8_t *dmc_msg_rt_stop_loop_request_serialize(const DmcRtStopLoopRequest *msg,
                                                uint8_t *buffer,
                                                uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_rt_stop_loop_request_deserialize(
    DmcRtStopLoopRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_virt_config_request_serialize(const DmcVirtConfigRequest *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length = sizeof(msg->type);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }
  const uint32_t data_payload_length =
      msg->header.length - fixed_payload_length;
  if (data_payload_length % sizeof(msg->data[0]) != 0) {
    return NULL;
  }
  const uint32_t count_data = data_payload_length / sizeof(msg->data[0]);
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->type);
  memcpy(cursor, msg->data, count_data * sizeof(msg->data[0]));
  return cursor + count_data * sizeof(msg->data[0]);
}

const uint8_t *dmc_msg_virt_config_request_deserialize(
    DmcVirtConfigRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header)) {
    return NULL;
  }
  const uint8_t *cursor =
      dmc_msg_header_deserialize(&msg->header, buffer, buffer_size);
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }
  const uint32_t fixed_payload_length = sizeof(msg->type);
  if (msg->header.length < fixed_payload_length) {
    return NULL;
  }
  const uint32_t data_payload_length =
      msg->header.length - fixed_payload_length;
  if (data_payload_length % sizeof(msg->data[0]) != 0) {
    return NULL;
  }
  const uint32_t count_data = data_payload_length / sizeof(msg->data[0]);
  cursor = deserialize_u8(cursor, &msg->type);
  memcpy(msg->data, cursor, count_data * sizeof(msg->data[0]));
  return cursor + count_data * sizeof(msg->data[0]);
}

uint8_t *dmc_msg_virt_config_request_boom_swing_track_serialize(
    const DmcVirtConfigRequestBoomSwingTrack *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->type);
  cursor = serialize_u32(cursor, msg->boom_motor);
  cursor = serialize_u32(cursor, msg->boom_spu);
  cursor = serialize_u32(cursor, msg->boom_position);
  cursor = serialize_u32(cursor, msg->swing_motor);
  cursor = serialize_u32(cursor, msg->swing_spu);
  cursor = serialize_u32(cursor, msg->swing_position);
  cursor = serialize_u32(cursor, msg->track_motor);
  cursor = serialize_u32(cursor, msg->track_spu);
  cursor = serialize_u32(cursor, msg->track_position);
  cursor = serialize_u32(cursor, msg->pan_motor);
  cursor = serialize_u32(cursor, msg->pan_spu);
  cursor = serialize_u32(cursor, msg->pan_position);
  cursor = serialize_u32(cursor, msg->tilt_motor);
  cursor = serialize_u32(cursor, msg->tilt_spu);
  cursor = serialize_u32(cursor, msg->tilt_position);
  cursor = serialize_u32(cursor, msg->roll_motor);
  cursor = serialize_u32(cursor, msg->roll_spu);
  cursor = serialize_u32(cursor, msg->roll_position);
  cursor = serialize_u32(cursor, msg->boom_length);
  cursor = serialize_u32(cursor, msg->boom_extension);
  cursor = serialize_u32(cursor, msg->nodal_offset_x);
  cursor = serialize_u32(cursor, msg->nodal_offset_y);
  cursor = serialize_u32(cursor, msg->nodal_offset_z);
  for (uint32_t i = 0; i < DMC_NUM_BOOM_COMPENSATION_ANGLES; ++i) {
    cursor = serialize_u32(cursor, msg->boom_compensation[i]);
  }
  cursor = serialize_u32(cursor, msg->safe_distance);
  return cursor;
}

const uint8_t *dmc_msg_virt_config_request_boom_swing_track_deserialize(
    DmcVirtConfigRequestBoomSwingTrack *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->type);
  cursor = deserialize_u32(cursor, &msg->boom_motor);
  cursor = deserialize_u32(cursor, &msg->boom_spu);
  cursor = deserialize_u32(cursor, &msg->boom_position);
  cursor = deserialize_u32(cursor, &msg->swing_motor);
  cursor = deserialize_u32(cursor, &msg->swing_spu);
  cursor = deserialize_u32(cursor, &msg->swing_position);
  cursor = deserialize_u32(cursor, &msg->track_motor);
  cursor = deserialize_u32(cursor, &msg->track_spu);
  cursor = deserialize_u32(cursor, &msg->track_position);
  cursor = deserialize_u32(cursor, &msg->pan_motor);
  cursor = deserialize_u32(cursor, &msg->pan_spu);
  cursor = deserialize_u32(cursor, &msg->pan_position);
  cursor = deserialize_u32(cursor, &msg->tilt_motor);
  cursor = deserialize_u32(cursor, &msg->tilt_spu);
  cursor = deserialize_u32(cursor, &msg->tilt_position);
  cursor = deserialize_u32(cursor, &msg->roll_motor);
  cursor = deserialize_u32(cursor, &msg->roll_spu);
  cursor = deserialize_u32(cursor, &msg->roll_position);
  cursor = deserialize_u32(cursor, &msg->boom_length);
  cursor = deserialize_u32(cursor, &msg->boom_extension);
  cursor = deserialize_u32(cursor, &msg->nodal_offset_x);
  cursor = deserialize_u32(cursor, &msg->nodal_offset_y);
  cursor = deserialize_u32(cursor, &msg->nodal_offset_z);
  for (uint32_t i = 0; i < DMC_NUM_BOOM_COMPENSATION_ANGLES; ++i) {
    cursor = deserialize_u32(cursor, &msg->boom_compensation[i]);
  }
  cursor = deserialize_u32(cursor, &msg->safe_distance);
  return cursor;
}

uint8_t *dmc_msg_virt_config_request_swing_pan_serialize(
    const DmcVirtConfigRequestSwingPan *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->type);
  cursor = serialize_u32(cursor, msg->swing_motor);
  cursor = serialize_u32(cursor, msg->swing_spu);
  cursor = serialize_u32(cursor, msg->pan_motor);
  cursor = serialize_u32(cursor, msg->pan_spu);
  return cursor;
}

const uint8_t *dmc_msg_virt_config_request_swing_pan_deserialize(
    DmcVirtConfigRequestSwingPan *msg, const uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->type);
  cursor = deserialize_u32(cursor, &msg->swing_motor);
  cursor = deserialize_u32(cursor, &msg->swing_spu);
  cursor = deserialize_u32(cursor, &msg->pan_motor);
  cursor = deserialize_u32(cursor, &msg->pan_spu);
  return cursor;
}

uint8_t *dmc_msg_virt_move_request_serialize(const DmcVirtMoveRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  uint32_t float_bits;
  {
    memcpy(&float_bits, &msg->position, sizeof(float_bits));
    cursor = serialize_u32(cursor, float_bits);
  }
  return cursor;
}

const uint8_t *dmc_msg_virt_move_request_deserialize(DmcVirtMoveRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  {
    uint32_t float_bits;
    cursor = deserialize_u32(cursor, &float_bits);
    memcpy(&msg->position, &float_bits, sizeof(float_bits));
  }
  return cursor;
}

uint8_t *dmc_msg_virt_stop_request_serialize(const DmcVirtStopRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  return cursor;
}

const uint8_t *dmc_msg_virt_stop_request_deserialize(DmcVirtStopRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  return cursor;
}

uint8_t *dmc_msg_virt_jog_request_serialize(const DmcVirtJogRequest *msg,
                                            uint8_t *buffer,
                                            uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->motor);
  cursor = serialize_u16(cursor, msg->speed);
  cursor = serialize_i32(cursor, msg->destination);
  return cursor;
}

const uint8_t *dmc_msg_virt_jog_request_deserialize(DmcVirtJogRequest *msg,
                                                    const uint8_t *buffer,
                                                    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->motor);
  cursor = deserialize_u16(cursor, &msg->speed);
  cursor = deserialize_i32(cursor, &msg->destination);
  return cursor;
}

uint8_t *dmc_msg_virt_jog_on_line_request_serialize(
    const DmcVirtJogOnLineRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->axis);
  cursor = serialize_u16(cursor, msg->speed);
  return cursor;
}

const uint8_t *dmc_msg_virt_jog_on_line_request_deserialize(
    DmcVirtJogOnLineRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->axis);
  cursor = deserialize_u16(cursor, &msg->speed);
  return cursor;
}

uint8_t *dmc_msg_virt_get_position_request_serialize(
    const DmcVirtGetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *
dmc_msg_virt_get_position_request_deserialize(DmcVirtGetPositionRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_virt_get_position_response_serialize(
    const DmcVirtGetPositionResponse *msg, uint8_t *buffer,
    uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u32(cursor, msg->track);
  cursor = serialize_u32(cursor, msg->EW);
  cursor = serialize_u32(cursor, msg->NS);
  cursor = serialize_u32(cursor, msg->pan);
  cursor = serialize_u32(cursor, msg->tilt);
  cursor = serialize_u32(cursor, msg->roll);
  cursor = serialize_u8(cursor, msg->aim_point);
  cursor = serialize_u32(cursor, msg->aim_x);
  cursor = serialize_u32(cursor, msg->aim_y);
  cursor = serialize_u32(cursor, msg->aim_z);
  return cursor;
}

const uint8_t *
dmc_msg_virt_get_position_response_deserialize(DmcVirtGetPositionResponse *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u32(cursor, &msg->track);
  cursor = deserialize_u32(cursor, &msg->EW);
  cursor = deserialize_u32(cursor, &msg->NS);
  cursor = deserialize_u32(cursor, &msg->pan);
  cursor = deserialize_u32(cursor, &msg->tilt);
  cursor = deserialize_u32(cursor, &msg->roll);
  cursor = deserialize_u8(cursor, &msg->aim_point);
  cursor = deserialize_u32(cursor, &msg->aim_x);
  cursor = deserialize_u32(cursor, &msg->aim_y);
  cursor = deserialize_u32(cursor, &msg->aim_z);
  return cursor;
}

uint8_t *dmc_msg_virt_aim_point_request_serialize(
    const DmcVirtAimPointRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->enable);
  cursor = serialize_u32(cursor, msg->aim_x);
  cursor = serialize_u32(cursor, msg->aim_y);
  cursor = serialize_u32(cursor, msg->aim_z);
  return cursor;
}

const uint8_t *dmc_msg_virt_aim_point_request_deserialize(
    DmcVirtAimPointRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->enable);
  cursor = deserialize_u32(cursor, &msg->aim_x);
  cursor = deserialize_u32(cursor, &msg->aim_y);
  cursor = deserialize_u32(cursor, &msg->aim_z);
  return cursor;
}

uint8_t *dmc_msg_virt_aim_point_response_serialize(
    const DmcVirtAimPointResponse *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->enable);
  cursor = serialize_u32(cursor, msg->aim_x);
  cursor = serialize_u32(cursor, msg->aim_y);
  cursor = serialize_u32(cursor, msg->aim_z);
  return cursor;
}

const uint8_t *dmc_msg_virt_aim_point_response_deserialize(
    DmcVirtAimPointResponse *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->enable);
  cursor = deserialize_u32(cursor, &msg->aim_x);
  cursor = deserialize_u32(cursor, &msg->aim_y);
  cursor = deserialize_u32(cursor, &msg->aim_z);
  return cursor;
}
