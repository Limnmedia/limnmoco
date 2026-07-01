// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_GIO_HPP
#define LIMNMOCO_CM7_GIO_HPP

#include <cstdint>

#include "config.hpp"

class Gio {
public:
  Gio();

  void update();

  bool in_changed();
  uint32_t get_in();
  uint32_t get_out();
  void set_out(uint32_t out);

private:
  void read_input(uint8_t which);
  void read_input();

  void write_output(uint8_t which);
  void write_output();

  uint32_t out;

  uint32_t in;
  int8_t   in_debounce[LIMNMOCO_GIO_IN];
  bool     change;
};

#endif // !LIMNMOCO_CM7_GIO_HPP
