// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_GLOBAL_HPP
#define LIMNMOCO_CM4_GLOBAL_HPP

#include "config.hpp"
#include "motors.hpp"

struct Global {
    void begin();

    void update();

    Motors motors;
};

extern Global global;

#endif // LIMNMOCO_CM4_GLOBAL_HPP

