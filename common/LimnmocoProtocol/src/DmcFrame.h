// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_DMC_FRAME_H_
#define LIMNMOCO_DMC_FRAME_H_

#include <cstdint>
#include <string>
#include <vector>

namespace limnmoco {

struct DmcFrame {
  uint32_t id;
  uint16_t type;
  std::vector<uint8_t> payload;
};

uint16_t dmc_checksum(const uint8_t *data, std::size_t size);
bool dmc_encode_frame(const DmcFrame &frame, std::vector<uint8_t> *bytes,
                      std::string *error);
bool dmc_decode_frame(const std::vector<uint8_t> &bytes, DmcFrame *frame,
                      std::string *error);

} // namespace limnmoco

#endif // LIMNMOCO_DMC_FRAME_H_
