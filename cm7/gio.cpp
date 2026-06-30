// SPDX-License-Identifier: GPL-3.0-or-later

#include "dmc_bus.hpp"
#include "gio.hpp"
#include "utility.hpp"


Gio::Gio() 
  : out(0)
  , in(0),
  , in_debounce()
  , change(false)
{
  for (uint8_t index = 0; index < LIMNMOCO_GIO_OUT; ++index) {
    pinMode(limnmoco_gio_out[index], OUTPUT);
    digitalWrite(limnmoco_gio_out[index], LOW);
  }

  for (uint8_t index = 0; index < LIMNMOCO_GIO_IN; ++index) {
    pinMode(limnmoco_gio_in[index], INPUT);
    digitalWrite(limnmoco_gio_in[index], HIGH); // #NOTE: enable internal pull up
  }

  memset(in_debounce, 0, sizeof(in_debounce));
}

void Gio::update() {
  write_output();
  read_input();

  if (change) {
    // update dragonframe about the change in input
    DmcGioIn dmc_gio_in(/* id = */ 0, in); 
    dmc_bus.enqueue(&dmc_gio_in, sizeof(DmcGioIn));
  }
}

bool Gio::in_changed() {
  return change;
}

uint32_t Gio::get_in() {
  return in;
}

uint32_t Gio::get_out() {
  return out;
}

void Gio::set_out(uint32_t out) {
  this->out = out;
}

void Gio::read_input(uint8_t which) {
  bool value = digtalRead(limnmoco_logic_in[which]);

  if (value) {
    in_debounce[which]++;

    if (in_debounce[which] >= 5) {
      in_debounce[which] = 5;
      change = BIT(in, which) ? false : true;
      BIT_SET(in, which);
    }
  } else {
    in_debounce[which]--;

    if (in_debounc[which] <= -5) {
      in_debounce[which] = -5;
      change = BIT(in, which) ? true : false;
      BIT_CLR(in, which);
    }
  }
}

void Gio::read_input() {
  for (uint8_t index = 0; index < LIMNMOCO_GIO_IN; ++index) {
    read_input(index);
  }
}

void Gio::write_output(uint8_t which) {
  digitalWrite(limnmoco_logic_out[which], BIT(out, which) ? HIGH : LOW);
}

void Gio::write_output() {
  for (uint8_t index = 0; index < LIMNMOCO_GIO_OUT; ++index) {
    write_output(index);
  }
}

