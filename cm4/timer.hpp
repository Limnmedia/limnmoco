// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_TIMER_HPP
#define LIMNMOCO_CM4_TIMER_HPP

#include <mbed.h>

struct Timer {
    static void begin();
    static void on_time();

    static Ticker timer;
};

#endif // !LIMNMOCO_CM4_TIMER_HPP

