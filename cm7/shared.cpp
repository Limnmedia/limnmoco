// SPDX-License-Identifier: GPL-3.0-or-later

#include "shared.hpp"

SharedData *SharedData::ptr;

void SharedData::begin() {
  ptr = (SharedData *)0x3800FD00;
  memset(ptr, 0, sizeof(SharedData));
}

SharedData *SharedData::get() {
  return ptr;
}

uintptr_t get_shared_data() {
  return (uintptr_t)SharedData::get();
}

