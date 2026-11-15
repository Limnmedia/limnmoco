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

#include "test/dmc/protocol.h"
#include "dmc/protocol.h"

static MunitResult
test_dmc_msg_header_roundtrip([[maybe_unused]] const MunitParameter params[],
                              [[maybe_unused]] void *user_data);

static MunitResult
test_dmc_msg_ack_roundtrip([[maybe_unused]] const MunitParameter params[],
                           [[maybe_unused]] void *user_data);

static MunitResult
test_dmc_msg_hi_request_roundtrip([[maybe_unused]] const MunitParameter params[],
                                  [[maybe_unused]] void *user_data);

static MunitResult
test_dmc_msg_hi_response_roundtrip([[maybe_unused]] const MunitParameter params[],
                                   [[maybe_unused]] void *user_data);

static MunitResult
test_dmc_msg_dmx_request_roundtrip([[maybe_unused]] const MunitParameter params[],
                                   [[maybe_unused]] void *user_data);

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
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static void rand_header(DmcHeader *header) {
  header->marker[0] = 'D';
  header->marker[1] = 'F';
  header->id = munit_rand_uint32();
  header->type = (uint16_t)munit_rand_uint32();
  header->length = (uint16_t)munit_rand_uint32();
}

static void rand_ack(DmcAck *ack) {
  rand_header(&ack->header);
  ack->response_code = (uint16_t)munit_rand_uint32();
}

static void rand_hi_request(DmcHiRequest *request) {
  rand_header(&request->header);
}

static bool header_eq(const DmcHeader *lhs, const DmcHeader *rhs) {
  return lhs->marker[0] == rhs->marker[0] &&
         lhs->marker[1] == rhs->marker[1] &&
         lhs->id == rhs->id &&
         lhs->type == rhs->type &&
         lhs->length == rhs->length;
}

static MunitResult
test_dmc_msg_header_roundtrip([[maybe_unused]] const MunitParameter params[],
                              [[maybe_unused]] void *user_data) {
  DmcHeader header;
  rand_header(&header);

  uint8_t buffer[sizeof(header)];
  dmc_msg_header_serialize(&header, buffer, sizeof(buffer));

  DmcHeader decoded;
  dmc_msg_header_deserialize(&decoded,  buffer,sizeof(buffer));

  munit_assert_true(header_eq(&header, &decoded));
  return MUNIT_OK;
}

static MunitResult
test_dmc_msg_ack_roundtrip([[maybe_unused]] const MunitParameter params[],
                           [[maybe_unused]] void *user_data) {

  DmcAck ack;
  rand_ack(&ack);

  uint8_t buffer[sizeof(ack)];
  dmc_msg_ack_serialize(&ack, buffer, sizeof(buffer));

  DmcAck decoded;
  dmc_msg_ack_deserialize(&decoded, buffer, sizeof(buffer));

  munit_assert_true(header_eq(&ack.header, &decoded.header));
  munit_assert_uint16(ack.response_code, ==, decoded.response_code);
  return MUNIT_OK;
}

static MunitResult
test_dmc_msg_hi_request_roundtrip([[maybe_unused]] const MunitParameter params[],
                                  [[maybe_unused]] void *user_data) {
  DmcHiRequest request;
  rand_hi_request(&request);

  uint8_t buffer[sizeof(request)];
  dmc_msg_hi_request_serialize(&request, buffer, sizeof(buffer));

  DmcHiRequest decoded;
  dmc_msg_hi_request_deserialize(&decoded, buffer, sizeof(buffer));

  munit_assert_true(header_eq(&request.header, &decoded.header));
  return MUNIT_OK;
}

static MunitResult
test_dmc_msg_hi_response_roundtrip([[maybe_unused]] const MunitParameter params[],
                                   [[maybe_unused]] void *user_data) {
  DmcHiResponse response;
  dmc_msg_hi_response_init(&response);

  uint8_t buffer[sizeof(response)];
  dmc_msg_hi_response_serialize(&response, buffer, sizeof(buffer));

  DmcHiResponse decoded;
  dmc_msg_hi_response_deserialize(&decoded, buffer, sizeof(buffer));

  munit_assert_true(header_eq(&response.header, &decoded.header));
  munit_assert_memory_equal(sizeof(response.name), response.name, decoded.name);
  munit_assert_uint8(response.fw_major, ==, decoded.fw_major);
  munit_assert_uint8(response.fw_minor, ==, decoded.fw_minor);
  munit_assert_uint8(response.fw_rev, ==, decoded.fw_rev);
  munit_assert_uint8(response.motor_count, ==, decoded.motor_count);
  munit_assert_uint16(response.dmx_count, ==, decoded.dmx_count);
  munit_assert_uint8(response.gio_out_count, ==, decoded.gio_out_count);
  munit_assert_uint8(response.gio_in_count, ==, decoded.gio_in_count);
  munit_assert_uint8(response.hw_limit_count, ==, decoded.hw_limit_count);
  munit_assert_uint32(response.upload_frame_count, ==, decoded.upload_frame_count);
  munit_assert_uint32(response.capabilities, ==, decoded.capabilities);
  munit_assert_uint16(response.protocol_version, ==, decoded.protocol_version);
  return MUNIT_OK;
}

static MunitResult
test_dmc_msg_dmx_request_roundtrip([[maybe_unused]] const MunitParameter params[],
                                   [[maybe_unused]] void *user_data) {

                                   }

