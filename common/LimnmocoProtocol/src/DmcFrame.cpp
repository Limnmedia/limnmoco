// SPDX-License-Identifier: BSD-3-Clause

#include "DmcFrame.h"

namespace limnmoco {
namespace {
constexpr std::size_t kHeaderSize = 10;
constexpr std::size_t kChecksumSize = 2;

void append_u16(std::vector<uint8_t> *bytes, uint16_t value) {
  bytes->push_back(static_cast<uint8_t>(value));
  bytes->push_back(static_cast<uint8_t>(value >> 8));
}
void append_u32(std::vector<uint8_t> *bytes, uint32_t value) {
  for (int index = 0; index < 4; ++index) bytes->push_back(value >> (index * 8));
}
uint16_t read_u16(const std::vector<uint8_t> &bytes, std::size_t offset) {
  return static_cast<uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
}
uint32_t read_u32(const std::vector<uint8_t> &bytes, std::size_t offset) {
  return static_cast<uint32_t>(bytes[offset]) | (static_cast<uint32_t>(bytes[offset + 1]) << 8) |
      (static_cast<uint32_t>(bytes[offset + 2]) << 16) | (static_cast<uint32_t>(bytes[offset + 3]) << 24);
}
} // namespace

uint16_t dmc_checksum(const uint8_t *data, std::size_t size) {
  uint16_t sum1 = 0, sum2 = 0;
  while (size) {
    const std::size_t chunk = size > 20 ? 20 : size;
    size -= chunk;
    for (std::size_t index = 0; index < chunk; ++index) sum2 += (sum1 += *data++);
    sum1 %= 0xff;
    sum2 %= 0xff;
  }
  return static_cast<uint16_t>((sum2 << 8) | sum1);
}

bool dmc_encode_frame(const DmcFrame &frame, std::vector<uint8_t> *bytes,
                      std::string *error) {
  if (!bytes || !error || frame.payload.size() > 2036) return false;
  bytes->clear(); error->clear();
  bytes->push_back('D'); bytes->push_back('F'); append_u32(bytes, frame.id);
  append_u16(bytes, frame.type); append_u16(bytes, static_cast<uint16_t>(frame.payload.size()));
  bytes->insert(bytes->end(), frame.payload.begin(), frame.payload.end());
  const uint16_t sum = dmc_checksum(bytes->data(), bytes->size());
  const uint8_t c0 = static_cast<uint8_t>(0xff - (((sum & 0xff) + (sum >> 8)) % 0xff));
  const uint8_t c1 = static_cast<uint8_t>(0xff - (((sum & 0xff) + c0) % 0xff));
  bytes->push_back(c0); bytes->push_back(c1);
  return true;
}

bool dmc_decode_frame(const std::vector<uint8_t> &bytes, DmcFrame *frame,
                      std::string *error) {
  if (!frame || !error) return false;
  error->clear();
  if (bytes.size() < kHeaderSize + kChecksumSize || bytes[0] != 'D' || bytes[1] != 'F') {
    *error = "invalid DMC frame header"; return false;
  }
  const uint16_t length = read_u16(bytes, 8);
  if (bytes.size() != kHeaderSize + length + kChecksumSize) {
    *error = "DMC frame length does not match payload"; return false;
  }
  if (dmc_checksum(bytes.data(), bytes.size()) != 0) {
    *error = "DMC frame checksum is invalid"; return false;
  }
  frame->id = read_u32(bytes, 2); frame->type = read_u16(bytes, 6);
  frame->payload.assign(bytes.begin() + kHeaderSize, bytes.end() - kChecksumSize);
  return true;
}
} // namespace limnmoco
