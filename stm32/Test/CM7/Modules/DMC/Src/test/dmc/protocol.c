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

#include <stdio.h>

#include "dmc/protocol.h"
#include "test/dmc/protocol.h"

enum {
  DMC_TEST_RANDOM_LENGTH_CASES_MIN = 1,
  DMC_TEST_RANDOM_LENGTH_CASES_MAX = 10,
  DMC_TEST_DMX_CHANNEL_COUNT = 512
};

static uint32_t random_length_case_count(void) {
  return DMC_TEST_RANDOM_LENGTH_CASES_MIN +
         (munit_rand_uint32() % (DMC_TEST_RANDOM_LENGTH_CASES_MAX -
                                 DMC_TEST_RANDOM_LENGTH_CASES_MIN + 1));
}

static uint32_t select_dynamic_element_count(uint32_t case_index,
                                             uint32_t minimum,
                                             uint32_t maximum) {
  if (case_index == 0) {
    return minimum;
  }
  if (case_index == 1) {
    return maximum;
  }
  return minimum + (munit_rand_uint32() % (maximum - minimum + 1));
}

static bool header_eq(const DmcHeader *lhs, const DmcHeader *rhs) {
  return lhs->marker[0] == rhs->marker[0] && lhs->marker[1] == rhs->marker[1] &&
         lhs->id == rhs->id && lhs->type == rhs->type &&
         lhs->length == rhs->length;
}

static void fill_test_storage(uint8_t *storage, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    storage[i] = (uint8_t)(i * 37u + 11u);
  }
}

static MunitResult
test_dmc_msg_header_roundtrip([[maybe_unused]] const MunitParameter params[],
                              [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcHeader *original = (DmcHeader *)source;
  dmc_msg_header_init(original, 0x12345678u, DMC_MSG_HI, 0u);

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_header_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcHeader *decoded = (DmcHeader *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_header_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_memory_equal(sizeof(original->marker), original->marker,
                            decoded->marker);
  munit_assert_memory_equal(sizeof(original->id), &original->id, &decoded->id);
  munit_assert_memory_equal(sizeof(original->type), &original->type,
                            &decoded->type);
  munit_assert_memory_equal(sizeof(original->length), &original->length,
                            &decoded->length);
  return MUNIT_OK;
}

static MunitResult
test_dmc_msg_ack_roundtrip([[maybe_unused]] const MunitParameter params[],
                           [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcAck *original = (DmcAck *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_MOTOR_MOVE,
                      (uint16_t)(sizeof(((DmcAck *)0)->response_code)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_ack_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcAck *decoded = (DmcAck *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_ack_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->response_code),
                            &original->response_code, &decoded->response_code);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_hi_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcHiRequest *original = (DmcHiRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_HI,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_hi_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcHiRequest *decoded = (DmcHiRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_hi_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_hi_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcHiResponse *original = (DmcHiResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_HI,
      (uint16_t)(sizeof(((DmcHiResponse *)0)->name) +
                 sizeof(((DmcHiResponse *)0)->fw_major) +
                 sizeof(((DmcHiResponse *)0)->fw_minor) +
                 sizeof(((DmcHiResponse *)0)->fw_rev) +
                 sizeof(((DmcHiResponse *)0)->motor_count) +
                 sizeof(((DmcHiResponse *)0)->dmx_count) +
                 sizeof(((DmcHiResponse *)0)->gio_out_count) +
                 sizeof(((DmcHiResponse *)0)->gio_in_count) +
                 sizeof(((DmcHiResponse *)0)->hw_limit_count) +
                 sizeof(((DmcHiResponse *)0)->upload_frame_count) +
                 sizeof(((DmcHiResponse *)0)->capabilities) +
                 sizeof(((DmcHiResponse *)0)->protocol_version)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_hi_response_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcHiResponse *decoded = (DmcHiResponse *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_hi_response_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->name), original->name,
                            decoded->name);
  munit_assert_memory_equal(sizeof(original->fw_major), &original->fw_major,
                            &decoded->fw_major);
  munit_assert_memory_equal(sizeof(original->fw_minor), &original->fw_minor,
                            &decoded->fw_minor);
  munit_assert_memory_equal(sizeof(original->fw_rev), &original->fw_rev,
                            &decoded->fw_rev);
  munit_assert_memory_equal(sizeof(original->motor_count),
                            &original->motor_count, &decoded->motor_count);
  munit_assert_memory_equal(sizeof(original->dmx_count), &original->dmx_count,
                            &decoded->dmx_count);
  munit_assert_memory_equal(sizeof(original->gio_out_count),
                            &original->gio_out_count, &decoded->gio_out_count);
  munit_assert_memory_equal(sizeof(original->gio_in_count),
                            &original->gio_in_count, &decoded->gio_in_count);
  munit_assert_memory_equal(sizeof(original->hw_limit_count),
                            &original->hw_limit_count,
                            &decoded->hw_limit_count);
  munit_assert_memory_equal(sizeof(original->upload_frame_count),
                            &original->upload_frame_count,
                            &decoded->upload_frame_count);
  munit_assert_memory_equal(sizeof(original->capabilities),
                            &original->capabilities, &decoded->capabilities);
  munit_assert_memory_equal(sizeof(original->protocol_version),
                            &original->protocol_version,
                            &decoded->protocol_version);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_dmx_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  const uint32_t random_cases = random_length_case_count();
  for (uint32_t case_index = 0; case_index < random_cases + 2; ++case_index) {
    const uint32_t count =
        select_dynamic_element_count(case_index, 1, DMC_TEST_DMX_CHANNEL_COUNT);
    uint8_t source[DMC_MSG_MAX_LENGTH] = {};
    fill_test_storage(source, sizeof(source));
    DmcDmxRequest *original = (DmcDmxRequest *)source;
    original->ramp = 0;
    original->start_channel = 1;
    dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_DMX,
                        (uint16_t)(sizeof(original->ramp) +
                                   sizeof(original->start_channel) +
                                   count * sizeof(original->light_values[0])));

    uint8_t wire[DMC_MSG_MAX_LENGTH] = {};
    uint8_t *wire_end =
        dmc_msg_dmx_request_serialize(original, wire, sizeof(wire));
    munit_assert_true(wire_end != NULL);
    const size_t wire_length = (size_t)(wire_end - wire);

    uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
    DmcDmxRequest *decoded = (DmcDmxRequest *)decoded_storage;
    const uint8_t *read_end =
        dmc_msg_dmx_request_deserialize(decoded, wire, DMC_MSG_MAX_LENGTH);
    munit_assert_true(read_end != NULL);
    munit_assert_true((size_t)(read_end - wire) == wire_length);
    munit_assert_true(header_eq(&original->header, &decoded->header));
    munit_assert_memory_equal(sizeof(original->ramp), &original->ramp,
                              &decoded->ramp);
    munit_assert_memory_equal(sizeof(original->start_channel),
                              &original->start_channel,
                              &decoded->start_channel);
    munit_assert_memory_equal(count * sizeof(original->light_values[0]),
                              original->light_values, decoded->light_values);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_gio_out_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcGioOutRequest *original = (DmcGioOutRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_GIO_OUT,
                      (uint16_t)(sizeof(((DmcGioOutRequest *)0)->triggers)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_gio_out_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcGioOutRequest *decoded = (DmcGioOutRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_gio_out_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->triggers), &original->triggers,
                            &decoded->triggers);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_gio_in_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcGioInRequest *original = (DmcGioInRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_GIO_IN,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_gio_in_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcGioInRequest *decoded = (DmcGioInRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_gio_in_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_gio_in_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcGioInResponse *original = (DmcGioInResponse *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_GIO_IN,
                      (uint16_t)(sizeof(((DmcGioInResponse *)0)->triggers)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_gio_in_response_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcGioInResponse *decoded = (DmcGioInResponse *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_gio_in_response_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->triggers), &original->triggers,
                            &decoded->triggers);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_gio_cam_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcGioCamRequest *original = (DmcGioCamRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_GIO_CAM,
                      (uint16_t)(sizeof(((DmcGioCamRequest *)0)->triggers)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_gio_cam_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcGioCamRequest *decoded = (DmcGioCamRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_gio_cam_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->triggers), &original->triggers,
                            &decoded->triggers);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_status_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorStatusRequest *original = (DmcMotorStatusRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_MOTOR_STATUS,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_status_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorStatusRequest *decoded = (DmcMotorStatusRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_status_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_status_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorStatusResponse *original = (DmcMotorStatusResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_STATUS,
      (uint16_t)(sizeof(((DmcMotorStatusResponse *)0)->motor_status) +
                 sizeof(((DmcMotorStatusResponse *)0)->dmx_status)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_status_response_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorStatusResponse *decoded = (DmcMotorStatusResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_status_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor_status),
                            &original->motor_status, &decoded->motor_status);
  munit_assert_memory_equal(sizeof(original->dmx_status), &original->dmx_status,
                            &decoded->dmx_status);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_move_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorMoveRequest *original = (DmcMotorMoveRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_MOTOR_MOVE,
                      (uint16_t)(sizeof(((DmcMotorMoveRequest *)0)->motor) +
                                 sizeof(((DmcMotorMoveRequest *)0)->position)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_move_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorMoveRequest *decoded = (DmcMotorMoveRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_move_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->position), &original->position,
                            &decoded->position);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_move_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorMoveResponse *original = (DmcMotorMoveResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_MOVE,
      (uint16_t)(sizeof(((DmcMotorMoveResponse *)0)->motor_status)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_move_response_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorMoveResponse *decoded = (DmcMotorMoveResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_move_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor_status),
                            &original->motor_status, &decoded->motor_status);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_stop_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorStopRequest *original = (DmcMotorStopRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_MOTOR_STOP,
                      (uint16_t)(sizeof(((DmcMotorStopRequest *)0)->motor)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_stop_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorStopRequest *decoded = (DmcMotorStopRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_stop_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_stop_all_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorStopAllRequest *original = (DmcMotorStopAllRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_MOTOR_STOP_ALL,
                      (uint16_t)(sizeof(((DmcMotorStopAllRequest *)0)->flags)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_stop_all_request_serialize(original, wire_a,
                                                               sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorStopAllRequest *decoded = (DmcMotorStopAllRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_stop_all_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->flags), &original->flags,
                            &decoded->flags);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_get_position_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorGetPositionRequest *original = (DmcMotorGetPositionRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u,
                      DMC_MSG_MOTOR_GET_POSITION, (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_get_position_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorGetPositionRequest *decoded =
      (DmcMotorGetPositionRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_get_position_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_get_position_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorGetPositionResponse *original = (DmcMotorGetPositionResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_GET_POSITION,
      (uint16_t)(sizeof(((DmcMotorGetPositionResponse *)0)->move_time) +
                 sizeof(((DmcMotorGetPositionResponse *)0)->motor_positions)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_get_position_response_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorGetPositionResponse *decoded =
      (DmcMotorGetPositionResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_get_position_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->move_time), &original->move_time,
                            &decoded->move_time);
  munit_assert_memory_equal(sizeof(original->motor_positions),
                            original->motor_positions,
                            decoded->motor_positions);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_reset_position_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorResetPositionRequest *original =
      (DmcMotorResetPositionRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_RESET_POSITION,
      (uint16_t)(sizeof(((DmcMotorResetPositionRequest *)0)->motor) +
                 sizeof(((DmcMotorResetPositionRequest *)0)->position)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_reset_position_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorResetPositionRequest *decoded =
      (DmcMotorResetPositionRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_reset_position_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->position), &original->position,
                            &decoded->position);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_jog_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorJogRequest *original = (DmcMotorJogRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_JOG,
      (uint16_t)(sizeof(((DmcMotorJogRequest *)0)->motor) +
                 sizeof(((DmcMotorJogRequest *)0)->speed) +
                 sizeof(((DmcMotorJogRequest *)0)->destination)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_motor_jog_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorJogRequest *decoded = (DmcMotorJogRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_jog_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->speed), &original->speed,
                            &decoded->speed);
  munit_assert_memory_equal(sizeof(original->destination),
                            &original->destination, &decoded->destination);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_configure_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorConfigureRequest *original = (DmcMotorConfigureRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_CONFIGURE,
      (uint16_t)(sizeof(((DmcMotorConfigureRequest *)0)->motor) +
                 sizeof(((DmcMotorConfigureRequest *)0)->flags)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_configure_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorConfigureRequest *decoded =
      (DmcMotorConfigureRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_configure_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->flags), &original->flags,
                            &decoded->flags);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_set_speed_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorSetSpeedRequest *original = (DmcMotorSetSpeedRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_SET_SPEED,
      (uint16_t)(sizeof(((DmcMotorSetSpeedRequest *)0)->motor) +
                 sizeof(((DmcMotorSetSpeedRequest *)0)->max_velocity) +
                 sizeof(((DmcMotorSetSpeedRequest *)0)->max_acceleration)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_set_speed_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorSetSpeedRequest *decoded = (DmcMotorSetSpeedRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_set_speed_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->max_velocity),
                            &original->max_velocity, &decoded->max_velocity);
  munit_assert_memory_equal(sizeof(original->max_acceleration),
                            &original->max_acceleration,
                            &decoded->max_acceleration);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_set_limits_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorSetLimitsRequest *original = (DmcMotorSetLimitsRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_SET_LIMITS,
      (uint16_t)(sizeof(((DmcMotorSetLimitsRequest *)0)->motor) +
                 sizeof(((DmcMotorSetLimitsRequest *)0)->lower_enable) +
                 sizeof(((DmcMotorSetLimitsRequest *)0)->lower_limit) +
                 sizeof(((DmcMotorSetLimitsRequest *)0)->upper_enable) +
                 sizeof(((DmcMotorSetLimitsRequest *)0)->upper_limit) +
                 sizeof(((DmcMotorSetLimitsRequest *)0)->hw_set)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_set_limits_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorSetLimitsRequest *decoded =
      (DmcMotorSetLimitsRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_set_limits_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->lower_enable),
                            &original->lower_enable, &decoded->lower_enable);
  munit_assert_memory_equal(sizeof(original->lower_limit),
                            &original->lower_limit, &decoded->lower_limit);
  munit_assert_memory_equal(sizeof(original->upper_enable),
                            &original->upper_enable, &decoded->upper_enable);
  munit_assert_memory_equal(sizeof(original->upper_limit),
                            &original->upper_limit, &decoded->upper_limit);
  munit_assert_memory_equal(sizeof(original->hw_set), &original->hw_set,
                            &decoded->hw_set);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_hard_stop_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorHardStopRequest *original = (DmcMotorHardStopRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_HARD_STOP,
      (uint16_t)(sizeof(((DmcMotorHardStopRequest *)0)->response_code)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_hard_stop_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorHardStopRequest *decoded = (DmcMotorHardStopRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_hard_stop_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->response_code),
                            &original->response_code, &decoded->response_code);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_motor_hard_stop_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcMotorHardStopResponse *original = (DmcMotorHardStopResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_MOTOR_HARD_STOP,
      (uint16_t)(sizeof(((DmcMotorHardStopResponse *)0)->reason) +
                 sizeof(((DmcMotorHardStopResponse *)0)->motor)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_motor_hard_stop_response_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcMotorHardStopResponse *decoded =
      (DmcMotorHardStopResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_motor_hard_stop_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->reason), &original->reason,
                            &decoded->reason);
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_upload_move_begin_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtUploadMoveBeginRequest *original = (DmcRtUploadMoveBeginRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_UPLOAD_MOVE_BEGIN,
      (uint16_t)(sizeof(((DmcRtUploadMoveBeginRequest *)0)->start_frame) +
                 sizeof(((DmcRtUploadMoveBeginRequest *)0)->end_frame)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_rt_upload_move_begin_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtUploadMoveBeginRequest *decoded =
      (DmcRtUploadMoveBeginRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_upload_move_begin_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->start_frame),
                            &original->start_frame, &decoded->start_frame);
  munit_assert_memory_equal(sizeof(original->end_frame), &original->end_frame,
                            &decoded->end_frame);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_upload_move_axis_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  const uint32_t fixed_payload_length =
      sizeof(((DmcRtUploadMoveAxisRequest *)0)->motor) +
      sizeof(((DmcRtUploadMoveAxisRequest *)0)->start_index);
  const uint32_t element_size =
      sizeof(((DmcRtUploadMoveAxisRequest *)0)->positions[0]);
  const uint32_t storage_max =
      (DMC_MSG_MAX_LENGTH - offsetof(DmcRtUploadMoveAxisRequest, positions)) /
      element_size;
  const uint32_t wire_max =
      (DMC_MSG_MAX_LENGTH - (uint32_t)sizeof(DmcHeader) - fixed_payload_length) /
      element_size;
  const uint32_t maximum = storage_max < wire_max ? storage_max : wire_max;
  const uint32_t random_cases = random_length_case_count();

  for (uint32_t case_index = 0; case_index < random_cases + 2; ++case_index) {
    const uint32_t count = select_dynamic_element_count(case_index, 1, maximum);
    uint8_t source[DMC_MSG_MAX_LENGTH] = {};
    fill_test_storage(source, sizeof(source));
    DmcRtUploadMoveAxisRequest *original = (DmcRtUploadMoveAxisRequest *)source;
    original->motor = 1;
    original->start_index = 0;
    dmc_msg_header_init(
        &original->header, 0x12345678u, DMC_MSG_RT_UPLOAD_MOVE_AXIS,
        (uint16_t)(fixed_payload_length + count * element_size));

    uint8_t wire[DMC_MSG_MAX_LENGTH] = {};
    uint8_t *wire_end = dmc_msg_rt_upload_move_axis_request_serialize(
        original, wire, sizeof(wire));
    munit_assert_true(wire_end != NULL);
    const size_t wire_length = (size_t)(wire_end - wire);

    uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
    DmcRtUploadMoveAxisRequest *decoded =
        (DmcRtUploadMoveAxisRequest *)decoded_storage;
    const uint8_t *read_end = dmc_msg_rt_upload_move_axis_request_deserialize(
        decoded, wire, DMC_MSG_MAX_LENGTH);
    munit_assert_true(read_end != NULL);
    munit_assert_true((size_t)(read_end - wire) == wire_length);
    munit_assert_true(header_eq(&original->header, &decoded->header));
    munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                              &decoded->motor);
    munit_assert_memory_equal(sizeof(original->start_index),
                              &original->start_index, &decoded->start_index);
    munit_assert_memory_equal(count * element_size, original->positions,
                              decoded->positions);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_upload_move_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  const uint32_t fixed_payload_length =
      sizeof(((DmcRtUploadMoveRequest *)0)->channel) +
      sizeof(((DmcRtUploadMoveRequest *)0)->start_index);
  const uint32_t element_size =
      sizeof(((DmcRtUploadMoveRequest *)0)->light_levels[0]);
  const uint32_t storage_max =
      DMC_MSG_MAX_LENGTH - offsetof(DmcRtUploadMoveRequest, light_levels);
  const uint32_t wire_max =
      DMC_MSG_MAX_LENGTH - sizeof(DmcHeader) - fixed_payload_length;
  const uint32_t maximum = storage_max < wire_max ? storage_max : wire_max;
  const uint32_t random_cases = random_length_case_count();

  for (uint32_t case_index = 0; case_index < random_cases + 2; ++case_index) {
    const uint32_t count = select_dynamic_element_count(case_index, 1, maximum);
    uint8_t source[DMC_MSG_MAX_LENGTH] = {};
    fill_test_storage(source, sizeof(source));
    DmcRtUploadMoveRequest *original = (DmcRtUploadMoveRequest *)source;
    original->channel = 1;
    original->start_index = 0;
    dmc_msg_header_init(
        &original->header, 0x12345678u, DMC_MSG_RT_UPLOAD_MOVE_DMX,
        (uint16_t)(fixed_payload_length + count * element_size));

    uint8_t wire[DMC_MSG_MAX_LENGTH] = {};
    uint8_t *wire_end =
        dmc_msg_rt_upload_move_request_serialize(original, wire, sizeof(wire));
    munit_assert_true(wire_end != NULL);
    const size_t wire_length = (size_t)(wire_end - wire);

    uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
    DmcRtUploadMoveRequest *decoded = (DmcRtUploadMoveRequest *)decoded_storage;
    const uint8_t *read_end = dmc_msg_rt_upload_move_request_deserialize(
        decoded, wire, DMC_MSG_MAX_LENGTH);
    munit_assert_true(read_end != NULL);
    munit_assert_true((size_t)(read_end - wire) == wire_length);
    munit_assert_true(header_eq(&original->header, &decoded->header));
    munit_assert_memory_equal(sizeof(original->channel), &original->channel,
                              &decoded->channel);
    munit_assert_memory_equal(sizeof(original->start_index),
                              &original->start_index, &decoded->start_index);
    munit_assert_memory_equal(count * element_size, original->light_levels,
                              decoded->light_levels);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_upload_move_triggers_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  const uint32_t fixed_payload_length =
      sizeof(((DmcRtUploadMoveTriggersRequest *)0)->mask);
  const uint32_t pair_wire_size =
      sizeof(
          ((DmcRtUploadMoveTriggersRequest *)0)->move_frame_values[0].frame) +
      sizeof(
          ((DmcRtUploadMoveTriggersRequest *)0)->move_frame_values[0].values);
  const uint32_t storage_max =
      (DMC_MSG_MAX_LENGTH -
       offsetof(DmcRtUploadMoveTriggersRequest, move_frame_values)) /
      pair_wire_size;
  const uint32_t wire_max =
      (DMC_MSG_MAX_LENGTH - (uint32_t)sizeof(DmcHeader) - fixed_payload_length) /
      pair_wire_size;
  const uint32_t maximum = storage_max < wire_max ? storage_max : wire_max;
  const uint32_t random_cases = random_length_case_count();

  for (uint32_t case_index = 0; case_index < random_cases + 2; ++case_index) {
    const uint32_t count = select_dynamic_element_count(case_index, 1, maximum);
    uint8_t source[DMC_MSG_MAX_LENGTH] = {};
    fill_test_storage(source, sizeof(source));
    DmcRtUploadMoveTriggersRequest *original =
        (DmcRtUploadMoveTriggersRequest *)source;
    original->mask = 1;
    for (uint32_t i = 0; i < count; ++i) {
      original->move_frame_values[i].frame = i;
      original->move_frame_values[i].values = 1;
    }
    dmc_msg_header_init(
        &original->header, 0x12345678u, DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS,
        (uint16_t)(fixed_payload_length + count * pair_wire_size));

    uint8_t wire[DMC_MSG_MAX_LENGTH] = {};
    uint8_t *wire_end = dmc_msg_rt_upload_move_triggers_request_serialize(
        original, wire, sizeof(wire));
    munit_assert_true(wire_end != NULL);
    const size_t wire_length = (size_t)(wire_end - wire);

    uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
    DmcRtUploadMoveTriggersRequest *decoded =
        (DmcRtUploadMoveTriggersRequest *)decoded_storage;
    const uint8_t *read_end =
        dmc_msg_rt_upload_move_triggers_request_deserialize(decoded, wire,
                                                            DMC_MSG_MAX_LENGTH);
    munit_assert_true(read_end != NULL);
    munit_assert_true((size_t)(read_end - wire) == wire_length);
    munit_assert_true(header_eq(&original->header, &decoded->header));
    munit_assert_memory_equal(sizeof(original->mask), &original->mask,
                              &decoded->mask);
    for (uint32_t i = 0; i < count; ++i) {
      munit_assert_memory_equal(sizeof(original->move_frame_values[i].frame),
                                &original->move_frame_values[i].frame,
                                &decoded->move_frame_values[i].frame);
      munit_assert_memory_equal(sizeof(original->move_frame_values[i].values),
                                &original->move_frame_values[i].values,
                                &decoded->move_frame_values[i].values);
    }
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_upload_move_end_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtUploadMoveEndRequest *original = (DmcRtUploadMoveEndRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u,
                      DMC_MSG_RT_UPLOAD_MOVE_END, (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_rt_upload_move_end_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtUploadMoveEndRequest *decoded =
      (DmcRtUploadMoveEndRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_upload_move_end_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_position_frame_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtPositionFrameRequest *original = (DmcRtPositionFrameRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_POSITION_FRAME,
      (uint16_t)(sizeof(((DmcRtPositionFrameRequest *)0)->frame)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_rt_position_frame_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtPositionFrameRequest *decoded =
      (DmcRtPositionFrameRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_position_frame_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->frame), &original->frame,
                            &decoded->frame);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_run_move_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtRunMoveRequest *original = (DmcRtRunMoveRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_RUN_MOVE,
      (uint16_t)(sizeof(((DmcRtRunMoveRequest *)0)->fps) +
                 sizeof(((DmcRtRunMoveRequest *)0)->start_frame) +
                 sizeof(((DmcRtRunMoveRequest *)0)->end_frame) +
                 sizeof(((DmcRtRunMoveRequest *)0)->pre_roll_time) +
                 sizeof(((DmcRtRunMoveRequest *)0)->post_roll_time) +
                 sizeof(((DmcRtRunMoveRequest *)0)->sync_dmx) +
                 sizeof(((DmcRtRunMoveRequest *)0)->bloop_location) +
                 sizeof(((DmcRtRunMoveRequest *)0)->bloop_dmx_channel) +
                 sizeof(((DmcRtRunMoveRequest *)0)->bloop_time) +
                 sizeof(((DmcRtRunMoveRequest *)0)->flags)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_rt_run_move_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtRunMoveRequest *decoded = (DmcRtRunMoveRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_run_move_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->fps), &original->fps,
                            &decoded->fps);
  munit_assert_memory_equal(sizeof(original->start_frame),
                            &original->start_frame, &decoded->start_frame);
  munit_assert_memory_equal(sizeof(original->end_frame), &original->end_frame,
                            &decoded->end_frame);
  munit_assert_memory_equal(sizeof(original->pre_roll_time),
                            &original->pre_roll_time, &decoded->pre_roll_time);
  munit_assert_memory_equal(sizeof(original->post_roll_time),
                            &original->post_roll_time,
                            &decoded->post_roll_time);
  munit_assert_memory_equal(sizeof(original->sync_dmx), &original->sync_dmx,
                            &decoded->sync_dmx);
  munit_assert_memory_equal(sizeof(original->bloop_location),
                            &original->bloop_location,
                            &decoded->bloop_location);
  munit_assert_memory_equal(sizeof(original->bloop_dmx_channel),
                            &original->bloop_dmx_channel,
                            &decoded->bloop_dmx_channel);
  munit_assert_memory_equal(sizeof(original->bloop_time), &original->bloop_time,
                            &decoded->bloop_time);
  munit_assert_memory_equal(sizeof(original->flags), &original->flags,
                            &decoded->flags);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_shoot_frame_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtShootFrameRequest *original = (DmcRtShootFrameRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_SHOOT_FRAME,
      (uint16_t)(sizeof(((DmcRtShootFrameRequest *)0)->frame) +
                 sizeof(((DmcRtShootFrameRequest *)0)->direction) +
                 sizeof(((DmcRtShootFrameRequest *)0)->exposure_time) +
                 sizeof(((DmcRtShootFrameRequest *)0)->blur_percent) +
                 (LIMNMOCO_MOTOR_COUNT) *
                     (sizeof(
                          ((DmcRtShootFrameRequest *)0)->motor_blur[0].motor) +
                      sizeof(((DmcRtShootFrameRequest *)0)
                                 ->motor_blur[0]
                                 .position_A) +
                      sizeof(((DmcRtShootFrameRequest *)0)
                                 ->motor_blur[0]
                                 .position_B))));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_rt_shoot_frame_request_serialize(original, wire_a,
                                                               sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtShootFrameRequest *decoded = (DmcRtShootFrameRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_shoot_frame_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->frame), &original->frame,
                            &decoded->frame);
  munit_assert_memory_equal(sizeof(original->direction), &original->direction,
                            &decoded->direction);
  munit_assert_memory_equal(sizeof(original->exposure_time),
                            &original->exposure_time, &decoded->exposure_time);
  munit_assert_memory_equal(sizeof(original->blur_percent),
                            &original->blur_percent, &decoded->blur_percent);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    munit_assert_memory_equal(sizeof(original->motor_blur[i].motor),
                              &original->motor_blur[i].motor,
                              &decoded->motor_blur[i].motor);
    munit_assert_memory_equal(sizeof(original->motor_blur[i].position_A),
                              &original->motor_blur[i].position_A,
                              &decoded->motor_blur[i].position_A);
    munit_assert_memory_equal(sizeof(original->motor_blur[i].position_B),
                              &original->motor_blur[i].position_B,
                              &decoded->motor_blur[i].position_B);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_shoot_frame2_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtShootFrame2Request *original = (DmcRtShootFrame2Request *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_SHOOT_FRAME2,
      (uint16_t)(sizeof(((DmcRtShootFrame2Request *)0)->frame) +
                 sizeof(((DmcRtShootFrame2Request *)0)->exposure_time) +
                 sizeof(((DmcRtShootFrame2Request *)0)->open_angle) +
                 sizeof(((DmcRtShootFrame2Request *)0)->close_angle) +
                 (LIMNMOCO_MOTOR_COUNT) *
                     (sizeof(
                          ((DmcRtShootFrame2Request *)0)->motor_blur[0].motor) +
                      sizeof(((DmcRtShootFrame2Request *)0)
                                 ->motor_blur[0]
                                 .position_A) +
                      sizeof(((DmcRtShootFrame2Request *)0)
                                 ->motor_blur[0]
                                 .position_B))));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_rt_shoot_frame2_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtShootFrame2Request *decoded = (DmcRtShootFrame2Request *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_shoot_frame2_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->frame), &original->frame,
                            &decoded->frame);
  munit_assert_memory_equal(sizeof(original->exposure_time),
                            &original->exposure_time, &decoded->exposure_time);
  munit_assert_memory_equal(sizeof(original->open_angle), &original->open_angle,
                            &decoded->open_angle);
  munit_assert_memory_equal(sizeof(original->close_angle),
                            &original->close_angle, &decoded->close_angle);
  for (uint32_t i = 0; i < LIMNMOCO_MOTOR_COUNT; ++i) {
    munit_assert_memory_equal(sizeof(original->motor_blur[i].motor),
                              &original->motor_blur[i].motor,
                              &decoded->motor_blur[i].motor);
    munit_assert_memory_equal(sizeof(original->motor_blur[i].position_A),
                              &original->motor_blur[i].position_A,
                              &decoded->motor_blur[i].position_A);
    munit_assert_memory_equal(sizeof(original->motor_blur[i].position_B),
                              &original->motor_blur[i].position_B,
                              &decoded->motor_blur[i].position_B);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_go_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtGoRequest *original = (DmcRtGoRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_RT_GO,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_rt_go_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtGoRequest *decoded = (DmcRtGoRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_rt_go_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_end_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtEndRequest *original = (DmcRtEndRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_RT_END,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_rt_end_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtEndRequest *decoded = (DmcRtEndRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_rt_end_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_jog_all_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtJogAllRequest *original = (DmcRtJogAllRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_RT_JOG_ALL,
      (uint16_t)(sizeof(((DmcRtJogAllRequest *)0)->fps) +
                 sizeof(((DmcRtJogAllRequest *)0)->destination)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_rt_jog_all_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtJogAllRequest *decoded = (DmcRtJogAllRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_jog_all_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->fps), &original->fps,
                            &decoded->fps);
  munit_assert_memory_equal(sizeof(original->destination),
                            &original->destination, &decoded->destination);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_rt_stop_loop_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcRtStopLoopRequest *original = (DmcRtStopLoopRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_RT_STOP_LOOP,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_rt_stop_loop_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcRtStopLoopRequest *decoded = (DmcRtStopLoopRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_rt_stop_loop_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_config_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  const size_t swing_pan_data_length =
      sizeof(((DmcVirtConfigRequestSwingPan *)0)->swing_motor) +
      sizeof(((DmcVirtConfigRequestSwingPan *)0)->swing_spu) +
      sizeof(((DmcVirtConfigRequestSwingPan *)0)->pan_motor) +
      sizeof(((DmcVirtConfigRequestSwingPan *)0)->pan_spu);
  const size_t boom_base_data_length =
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->swing_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->swing_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->swing_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->track_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->track_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->track_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_motor) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_spu) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_position) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_length) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_extension) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->nodal_offset_x) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->nodal_offset_y) +
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->nodal_offset_z);
  const size_t boom_compensation_length =
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_compensation);
  const size_t safe_distance_length =
      sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->safe_distance);
  const size_t valid_data_lengths[] = {
      0u, swing_pan_data_length, boom_base_data_length,
      boom_base_data_length + boom_compensation_length,
      boom_base_data_length + boom_compensation_length + safe_distance_length};
  const uint32_t random_cases = random_length_case_count();

  for (uint32_t case_index = 0; case_index < random_cases + 2; ++case_index) {
    size_t data_length;
    if (case_index == 0) {
      data_length = valid_data_lengths[0];
    } else if (case_index == 1) {
      data_length = valid_data_lengths[4];
    } else {
      const size_t random_index =
          munit_rand_uint32() %
          (sizeof(valid_data_lengths) / sizeof(valid_data_lengths[0]));
      data_length = valid_data_lengths[random_index];
    }

    uint8_t source[DMC_MSG_MAX_LENGTH] = {};
    fill_test_storage(source, sizeof(source));
    DmcVirtConfigRequest *original = (DmcVirtConfigRequest *)source;
    if (data_length == 0u) {
      original->type = DMC_VIRT_TYPE_NONE;
    } else if (data_length == swing_pan_data_length) {
      original->type = DMC_VIRT_TYPE_SWING_PAN;
    } else {
      original->type = DMC_VIRT_TYPE_BOOM_SWING_TRACK;
    }
    dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_VIRT_CONFIG,
                        (uint16_t)(sizeof(original->type) + data_length));

    uint8_t wire[DMC_MSG_MAX_LENGTH] = {};
    uint8_t *wire_end =
        dmc_msg_virt_config_request_serialize(original, wire, sizeof(wire));
    munit_assert_true(wire_end != NULL);
    const size_t wire_length = (size_t)(wire_end - wire);

    uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
    DmcVirtConfigRequest *decoded = (DmcVirtConfigRequest *)decoded_storage;
    const uint8_t *read_end = dmc_msg_virt_config_request_deserialize(
        decoded, wire, DMC_MSG_MAX_LENGTH);
    munit_assert_true(read_end != NULL);
    munit_assert_true((size_t)(read_end - wire) == wire_length);
    munit_assert_true(header_eq(&original->header, &decoded->header));
    munit_assert_memory_equal(sizeof(original->type), &original->type,
                              &decoded->type);
    munit_assert_memory_equal(data_length, original->data, decoded->data);
  }
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_config_request_boom_swing_track_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtConfigRequestBoomSwingTrack *original =
      (DmcVirtConfigRequestBoomSwingTrack *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_CONFIG,
      (uint16_t)(sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->type) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_spu) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_position) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->swing_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->swing_spu) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->swing_position) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->track_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->track_spu) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->track_position) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_spu) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->pan_position) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_spu) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->tilt_position) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_motor) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_spu) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->roll_position) +
                 sizeof(
                     ((DmcVirtConfigRequestBoomSwingTrack *)0)->boom_length) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->boom_extension) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->nodal_offset_x) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->nodal_offset_y) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->nodal_offset_z) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->boom_compensation) +
                 sizeof(((DmcVirtConfigRequestBoomSwingTrack *)0)
                            ->safe_distance)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_config_request_boom_swing_track_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtConfigRequestBoomSwingTrack *decoded =
      (DmcVirtConfigRequestBoomSwingTrack *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_virt_config_request_boom_swing_track_deserialize(
          decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->type), &original->type,
                            &decoded->type);
  munit_assert_memory_equal(sizeof(original->boom_motor), &original->boom_motor,
                            &decoded->boom_motor);
  munit_assert_memory_equal(sizeof(original->boom_spu), &original->boom_spu,
                            &decoded->boom_spu);
  munit_assert_memory_equal(sizeof(original->boom_position),
                            &original->boom_position, &decoded->boom_position);
  munit_assert_memory_equal(sizeof(original->swing_motor),
                            &original->swing_motor, &decoded->swing_motor);
  munit_assert_memory_equal(sizeof(original->swing_spu), &original->swing_spu,
                            &decoded->swing_spu);
  munit_assert_memory_equal(sizeof(original->swing_position),
                            &original->swing_position,
                            &decoded->swing_position);
  munit_assert_memory_equal(sizeof(original->track_motor),
                            &original->track_motor, &decoded->track_motor);
  munit_assert_memory_equal(sizeof(original->track_spu), &original->track_spu,
                            &decoded->track_spu);
  munit_assert_memory_equal(sizeof(original->track_position),
                            &original->track_position,
                            &decoded->track_position);
  munit_assert_memory_equal(sizeof(original->pan_motor), &original->pan_motor,
                            &decoded->pan_motor);
  munit_assert_memory_equal(sizeof(original->pan_spu), &original->pan_spu,
                            &decoded->pan_spu);
  munit_assert_memory_equal(sizeof(original->pan_position),
                            &original->pan_position, &decoded->pan_position);
  munit_assert_memory_equal(sizeof(original->tilt_motor), &original->tilt_motor,
                            &decoded->tilt_motor);
  munit_assert_memory_equal(sizeof(original->tilt_spu), &original->tilt_spu,
                            &decoded->tilt_spu);
  munit_assert_memory_equal(sizeof(original->tilt_position),
                            &original->tilt_position, &decoded->tilt_position);
  munit_assert_memory_equal(sizeof(original->roll_motor), &original->roll_motor,
                            &decoded->roll_motor);
  munit_assert_memory_equal(sizeof(original->roll_spu), &original->roll_spu,
                            &decoded->roll_spu);
  munit_assert_memory_equal(sizeof(original->roll_position),
                            &original->roll_position, &decoded->roll_position);
  munit_assert_memory_equal(sizeof(original->boom_length),
                            &original->boom_length, &decoded->boom_length);
  munit_assert_memory_equal(sizeof(original->boom_extension),
                            &original->boom_extension,
                            &decoded->boom_extension);
  munit_assert_memory_equal(sizeof(original->nodal_offset_x),
                            &original->nodal_offset_x,
                            &decoded->nodal_offset_x);
  munit_assert_memory_equal(sizeof(original->nodal_offset_y),
                            &original->nodal_offset_y,
                            &decoded->nodal_offset_y);
  munit_assert_memory_equal(sizeof(original->nodal_offset_z),
                            &original->nodal_offset_z,
                            &decoded->nodal_offset_z);
  munit_assert_memory_equal(sizeof(original->boom_compensation),
                            original->boom_compensation,
                            decoded->boom_compensation);
  munit_assert_memory_equal(sizeof(original->safe_distance),
                            &original->safe_distance, &decoded->safe_distance);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_config_request_swing_pan_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtConfigRequestSwingPan *original =
      (DmcVirtConfigRequestSwingPan *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_CONFIG,
      (uint16_t)(sizeof(((DmcVirtConfigRequestSwingPan *)0)->type) +
                 sizeof(((DmcVirtConfigRequestSwingPan *)0)->swing_motor) +
                 sizeof(((DmcVirtConfigRequestSwingPan *)0)->swing_spu) +
                 sizeof(((DmcVirtConfigRequestSwingPan *)0)->pan_motor) +
                 sizeof(((DmcVirtConfigRequestSwingPan *)0)->pan_spu)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_config_request_swing_pan_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtConfigRequestSwingPan *decoded =
      (DmcVirtConfigRequestSwingPan *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_config_request_swing_pan_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->type), &original->type,
                            &decoded->type);
  munit_assert_memory_equal(sizeof(original->swing_motor),
                            &original->swing_motor, &decoded->swing_motor);
  munit_assert_memory_equal(sizeof(original->swing_spu), &original->swing_spu,
                            &decoded->swing_spu);
  munit_assert_memory_equal(sizeof(original->pan_motor), &original->pan_motor,
                            &decoded->pan_motor);
  munit_assert_memory_equal(sizeof(original->pan_spu), &original->pan_spu,
                            &decoded->pan_spu);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_move_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtMoveRequest *original = (DmcVirtMoveRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_VIRT_MOVE,
                      (uint16_t)(sizeof(((DmcVirtMoveRequest *)0)->motor) +
                                 sizeof(((DmcVirtMoveRequest *)0)->position)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_virt_move_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtMoveRequest *decoded = (DmcVirtMoveRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_move_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->position), &original->position,
                            &decoded->position);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_stop_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtStopRequest *original = (DmcVirtStopRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_VIRT_STOP,
                      (uint16_t)(sizeof(((DmcVirtStopRequest *)0)->motor)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_virt_stop_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtStopRequest *decoded = (DmcVirtStopRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_stop_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_jog_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtJogRequest *original = (DmcVirtJogRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_JOG,
      (uint16_t)(sizeof(((DmcVirtJogRequest *)0)->motor) +
                 sizeof(((DmcVirtJogRequest *)0)->speed) +
                 sizeof(((DmcVirtJogRequest *)0)->destination)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end =
      dmc_msg_virt_jog_request_serialize(original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtJogRequest *decoded = (DmcVirtJogRequest *)decoded_storage;
  const uint8_t *read_end =
      dmc_msg_virt_jog_request_deserialize(decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->motor), &original->motor,
                            &decoded->motor);
  munit_assert_memory_equal(sizeof(original->speed), &original->speed,
                            &decoded->speed);
  munit_assert_memory_equal(sizeof(original->destination),
                            &original->destination, &decoded->destination);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_jog_on_line_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtJogOnLineRequest *original = (DmcVirtJogOnLineRequest *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_JOG_ON_LINE,
      (uint16_t)(sizeof(((DmcVirtJogOnLineRequest *)0)->axis) +
                 sizeof(((DmcVirtJogOnLineRequest *)0)->speed)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_jog_on_line_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtJogOnLineRequest *decoded = (DmcVirtJogOnLineRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_jog_on_line_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->axis), &original->axis,
                            &decoded->axis);
  munit_assert_memory_equal(sizeof(original->speed), &original->speed,
                            &decoded->speed);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_get_position_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtGetPositionRequest *original = (DmcVirtGetPositionRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_VIRT_GET_POSITION,
                      (uint16_t)(0u));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_get_position_request_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtGetPositionRequest *decoded =
      (DmcVirtGetPositionRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_get_position_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_get_position_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtGetPositionResponse *original = (DmcVirtGetPositionResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_GET_POSITION,
      (uint16_t)(sizeof(((DmcVirtGetPositionResponse *)0)->track) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->EW) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->NS) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->pan) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->tilt) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->roll) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->aim_point) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->aim_x) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->aim_y) +
                 sizeof(((DmcVirtGetPositionResponse *)0)->aim_z)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_get_position_response_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtGetPositionResponse *decoded =
      (DmcVirtGetPositionResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_get_position_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->track), &original->track,
                            &decoded->track);
  munit_assert_memory_equal(sizeof(original->EW), &original->EW, &decoded->EW);
  munit_assert_memory_equal(sizeof(original->NS), &original->NS, &decoded->NS);
  munit_assert_memory_equal(sizeof(original->pan), &original->pan,
                            &decoded->pan);
  munit_assert_memory_equal(sizeof(original->tilt), &original->tilt,
                            &decoded->tilt);
  munit_assert_memory_equal(sizeof(original->roll), &original->roll,
                            &decoded->roll);
  munit_assert_memory_equal(sizeof(original->aim_point), &original->aim_point,
                            &decoded->aim_point);
  munit_assert_memory_equal(sizeof(original->aim_x), &original->aim_x,
                            &decoded->aim_x);
  munit_assert_memory_equal(sizeof(original->aim_y), &original->aim_y,
                            &decoded->aim_y);
  munit_assert_memory_equal(sizeof(original->aim_z), &original->aim_z,
                            &decoded->aim_z);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_aim_point_request_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtAimPointRequest *original = (DmcVirtAimPointRequest *)source;
  dmc_msg_header_init(&original->header, 0x12345678u, DMC_MSG_VIRT_AIM_POINT,
                      (uint16_t)(sizeof(((DmcVirtAimPointRequest *)0)->enable) +
                                 sizeof(((DmcVirtAimPointRequest *)0)->aim_x) +
                                 sizeof(((DmcVirtAimPointRequest *)0)->aim_y) +
                                 sizeof(((DmcVirtAimPointRequest *)0)->aim_z)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_aim_point_request_serialize(original, wire_a,
                                                               sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtAimPointRequest *decoded = (DmcVirtAimPointRequest *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_aim_point_request_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->enable), &original->enable,
                            &decoded->enable);
  munit_assert_memory_equal(sizeof(original->aim_x), &original->aim_x,
                            &decoded->aim_x);
  munit_assert_memory_equal(sizeof(original->aim_y), &original->aim_y,
                            &decoded->aim_y);
  munit_assert_memory_equal(sizeof(original->aim_z), &original->aim_z,
                            &decoded->aim_z);
  return MUNIT_OK;
}

static MunitResult test_dmc_msg_virt_aim_point_response_roundtrip(
    [[maybe_unused]] const MunitParameter params[],
    [[maybe_unused]] void *user_data) {
  uint8_t source[DMC_MSG_MAX_LENGTH] = {};
  fill_test_storage(source, sizeof(source));
  DmcVirtAimPointResponse *original = (DmcVirtAimPointResponse *)source;
  dmc_msg_header_init(
      &original->header, 0x12345678u, DMC_MSG_VIRT_AIM_POINT,
      (uint16_t)(sizeof(((DmcVirtAimPointResponse *)0)->enable) +
                 sizeof(((DmcVirtAimPointResponse *)0)->aim_x) +
                 sizeof(((DmcVirtAimPointResponse *)0)->aim_y) +
                 sizeof(((DmcVirtAimPointResponse *)0)->aim_z)));

  uint8_t wire_a[DMC_MSG_MAX_LENGTH] = {};
  uint8_t *wire_end = dmc_msg_virt_aim_point_response_serialize(
      original, wire_a, sizeof(wire_a));
  munit_assert_true(wire_end != NULL);
  const size_t wire_length = (size_t)(wire_end - wire_a);

  uint8_t decoded_storage[DMC_MSG_MAX_LENGTH] = {};
  DmcVirtAimPointResponse *decoded = (DmcVirtAimPointResponse *)decoded_storage;
  const uint8_t *read_end = dmc_msg_virt_aim_point_response_deserialize(
      decoded, wire_a, DMC_MSG_MAX_LENGTH);
  munit_assert_true(read_end != NULL);
  munit_assert_true((size_t)(read_end - wire_a) == wire_length);
  munit_assert_true(header_eq(&original->header, &decoded->header));
  munit_assert_memory_equal(sizeof(original->enable), &original->enable,
                            &decoded->enable);
  munit_assert_memory_equal(sizeof(original->aim_x), &original->aim_x,
                            &decoded->aim_x);
  munit_assert_memory_equal(sizeof(original->aim_y), &original->aim_y,
                            &decoded->aim_y);
  munit_assert_memory_equal(sizeof(original->aim_z), &original->aim_z,
                            &decoded->aim_z);
  return MUNIT_OK;
}

MunitTest cm7_dmc_protocol_tests[] = {
    {"/header/roundtrip", test_dmc_msg_header_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/ack/roundtrip", test_dmc_msg_ack_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/hi/request/roundtrip", test_dmc_msg_hi_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/hi/response/roundtrip", test_dmc_msg_hi_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/dmx/request/roundtrip", test_dmc_msg_dmx_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/gio/out/request/roundtrip", test_dmc_msg_gio_out_request_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/gio/in/request/roundtrip", test_dmc_msg_gio_in_request_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/gio/in/response/roundtrip", test_dmc_msg_gio_in_response_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/gio/cam/request/roundtrip", test_dmc_msg_gio_cam_request_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/status/request/roundtrip",
     test_dmc_msg_motor_status_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/status/response/roundtrip",
     test_dmc_msg_motor_status_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/move/request/roundtrip", test_dmc_msg_motor_move_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/move/response/roundtrip",
     test_dmc_msg_motor_move_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/stop/request/roundtrip", test_dmc_msg_motor_stop_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/stop/all/request/roundtrip",
     test_dmc_msg_motor_stop_all_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/get/position/request/roundtrip",
     test_dmc_msg_motor_get_position_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/get/position/response/roundtrip",
     test_dmc_msg_motor_get_position_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/reset/position/request/roundtrip",
     test_dmc_msg_motor_reset_position_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/jog/request/roundtrip", test_dmc_msg_motor_jog_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/configure/request/roundtrip",
     test_dmc_msg_motor_configure_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/set/speed/request/roundtrip",
     test_dmc_msg_motor_set_speed_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/set/limits/request/roundtrip",
     test_dmc_msg_motor_set_limits_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/hard/stop/request/roundtrip",
     test_dmc_msg_motor_hard_stop_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/motor/hard/stop/response/roundtrip",
     test_dmc_msg_motor_hard_stop_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/upload/move/begin/request/roundtrip",
     test_dmc_msg_rt_upload_move_begin_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/upload/move/axis/request/roundtrip",
     test_dmc_msg_rt_upload_move_axis_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/upload/move/request/roundtrip",
     test_dmc_msg_rt_upload_move_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/upload/move/triggers/request/roundtrip",
     test_dmc_msg_rt_upload_move_triggers_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/upload/move/end/request/roundtrip",
     test_dmc_msg_rt_upload_move_end_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/position/frame/request/roundtrip",
     test_dmc_msg_rt_position_frame_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/run/move/request/roundtrip",
     test_dmc_msg_rt_run_move_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/shoot/frame/request/roundtrip",
     test_dmc_msg_rt_shoot_frame_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/shoot/frame2/request/roundtrip",
     test_dmc_msg_rt_shoot_frame2_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/go/request/roundtrip", test_dmc_msg_rt_go_request_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/end/request/roundtrip", test_dmc_msg_rt_end_request_roundtrip, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/jog/all/request/roundtrip", test_dmc_msg_rt_jog_all_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/rt/stop/loop/request/roundtrip",
     test_dmc_msg_rt_stop_loop_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/config/request/roundtrip",
     test_dmc_msg_virt_config_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/config/request/boom/swing/track/roundtrip",
     test_dmc_msg_virt_config_request_boom_swing_track_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/config/request/swing/pan/roundtrip",
     test_dmc_msg_virt_config_request_swing_pan_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/move/request/roundtrip", test_dmc_msg_virt_move_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/stop/request/roundtrip", test_dmc_msg_virt_stop_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/jog/request/roundtrip", test_dmc_msg_virt_jog_request_roundtrip,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/jog/on/line/request/roundtrip",
     test_dmc_msg_virt_jog_on_line_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/get/position/request/roundtrip",
     test_dmc_msg_virt_get_position_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/get/position/response/roundtrip",
     test_dmc_msg_virt_get_position_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/aim/point/request/roundtrip",
     test_dmc_msg_virt_aim_point_request_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/virt/aim/point/response/roundtrip",
     test_dmc_msg_virt_aim_point_response_roundtrip, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};
