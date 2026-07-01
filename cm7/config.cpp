// SPDX-License-Identifier: GPL-3.0-or-later

#include <Arduino.h>

#include "config.hpp"

uint8_t limnmoco_gio_out_buffer[LIMNMOCO_GIO_OUT] = {
  D40,
  D41,
};

uint8_t limnmoco_gio_in_buffer[LIMNMOCO_GIO_IN] = {
  D49,
};

uint8_t *limnmoco_gio_out = limnmoco_gio_out_buffer;
uint8_t *limnmoco_gio_in  = limnmoco_gio_in_buffer;

