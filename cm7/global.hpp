// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_GLOBAL_HPP
#define LIMNMOCO_CM7_GLOBAL_HPP

#include "gio.hpp"

struct Global {
  Global();

  void update();

  Gio gio;
};

extern Global global;

#endif // !LIMNMOCO_CM7_GLOBAL_HPP

