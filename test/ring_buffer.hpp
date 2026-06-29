// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_RING_BUFFER_HPP
#define LIMNMOCO_RING_BUFFER_HPP

#include <cstdint>

namespace lm {
//template <size_t LENGTH = 1024>
#define RING_BUFFER_LENGTH 2048

struct RingBuffer {
  uint8_t  buffer[RING_BUFFER_LENGTH];
  uint32_t head;
  uint32_t tail;

  RingBuffer();

  bool write(uint8_t data);
  bool write(uint8_t *data, uint32_t length);

  bool read(uint8_t *data);
  uint32_t read(uint8_t *data, uint32_t length);
};

} // namespace lm

#endif // !LIMNMOCO_RING_BUFFER_HPP

