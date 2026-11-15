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

#include "dmc/protocol.h"
#include "config/config.h"

void dmc_msg_header_init(DmcHeader *msg, uint32_t id, uint16_t type, uint16_t length) {
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
  memcpy(msg->name, "limnmoco", sizeof("limnmoco") - 1);
  msg->fw_major = LIMNMOCO_VERSION_MAJOR;
  msg->fw_minor = LIMNMOCO_VERSION_MINOR;
  msg->fw_rev = LIMNMOCO_VERSION_PATCH;
  msg->motor_count = LIMNMOCO_MOTOR_COUNT;
  msg->dmx_count   = LIMNMOCO_DMX_COUNT;
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
  return cursor + 2;
}

static uint8_t *serialize_u32(uint8_t *cursor, uint32_t value) {
  memcpy(cursor, &value, sizeof(value));
  return cursor + 4;
}

static uint8_t *serialize_i32(uint8_t *cursor, int32_t value) {
  memcpy(cursor, &value, sizeof(value));
  return cursor + 4;
}

static const uint8_t *deserialize_u8(const uint8_t *cursor, uint8_t *value) {
  *value = *cursor++;
  return cursor;
}

static const uint8_t *deserialize_u16(const uint8_t *cursor, uint16_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + 2;
}

static const uint8_t *deserialize_u32(const uint8_t *cursor, uint32_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + 4;
}

static const uint8_t *deserialize_i32(const uint8_t *cursor, int32_t *value) {
  memcpy(value, cursor, sizeof(*value));
  return cursor + 4;
}

uint8_t *dmc_msg_header_serialize(const DmcHeader *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  uint8_t *cursor = buffer;
  cursor = serialize_u8(cursor, msg->marker[0]);
  cursor = serialize_u8(cursor, msg->marker[1]);
  cursor = serialize_u32(cursor, msg->id);
  cursor = serialize_u16(cursor, msg->type);
  cursor = serialize_u16(cursor, msg->length);
  return cursor;
}

const uint8_t *dmc_msg_header_deserialize(DmcHeader *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }
  const uint8_t *cursor = buffer;
  cursor = deserialize_u8(cursor, &msg->marker[0]);
  cursor = deserialize_u8(cursor, &msg->marker[1]);
  cursor = deserialize_u32(cursor, &msg->id);
  cursor = deserialize_u16(cursor, &msg->type);
  cursor = deserialize_u16(cursor, &msg->length);
  return cursor;
}

uint8_t *dmc_msg_ack_serialize(const DmcAck *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u16(cursor, msg->response_code);
  return cursor;
}
const uint8_t *dmc_msg_ack_deserialize(DmcAck *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u16(cursor, &msg->response_code);
  return cursor;
}

uint8_t *dmc_msg_hi_request_serialize(const DmcHiRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  return cursor;
}

const uint8_t *dmc_msg_hi_request_deserialize(DmcHiRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(*msg)) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  return cursor;
}

uint8_t *dmc_msg_hi_response_serialize(const DmcHiResponse *msg, uint8_t *buffer, uint32_t buffer_size) {
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

const uint8_t *dmc_msg_hi_response_deserialize(DmcHiResponse *msg, const uint8_t *buffer, uint32_t buffer_size) {
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

uint8_t *dmc_msg_dmx_request_serialize(const DmcDmxRequest *msg, uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }

  uint8_t *cursor = buffer;
  cursor = dmc_msg_header_serialize(&msg->header, cursor, buffer_size);
  cursor = serialize_u8(cursor, msg->ramp);
  cursor = serialize_u16(cursor, msg->start_channel);
  // the number of light values given is determined from the length
  // given in the header. The header.length field holds the "length
  // of the data section". And we have fixed payload size of 1 for 
  // ramp, and 2 for start_channel. Given that each light value is
  // 1 byte, the number of light values is header.length - 3.
  uint32_t light_value_count = msg->header.length - 3;
  memcpy(cursor, msg->light_values, light_value_count);
  cursor += light_value_count;
  return cursor;
}

const uint8_t *dmc_msg_dmx_request_deserialize(DmcDmxRequest *msg, const uint8_t *buffer, uint32_t buffer_size) {
  if (buffer_size < sizeof(msg->header) + msg->header.length) {
    return NULL;
  }

  const uint8_t *cursor = buffer;
  cursor = dmc_msg_header_deserialize(&msg->header, cursor, buffer_size);
  cursor = deserialize_u8(cursor, &msg->ramp);
  cursor = deserialize_u16(cursor, &msg->start_channel);
  uint32_t light_value_count = msg->header.length - 3;
  memcpy(msg->light_values, cursor, light_value_count);
  cursor += light_value_count;
  return cursor;
}