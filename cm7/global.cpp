// SPDX-License-Identifier: GPL-3.0-or-later

#include "global.hpp"

Global global;

Global::Global()
  : gio()
{

}


void Global::update() {
  gio.update();
}

