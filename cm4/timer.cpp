// SPDX-License-Identifier: GPL-3.0-or-later

#include "timer.hpp"
#include "global.hpp"

Ticker Timer::timer;

void Timer::begin() {
    timer.attach(&on_time, std::chrono::microseconds{5});
}

void Timer::on_time() {
    global.motors.pulse_start();

    delayMicroseconds(3);

    global.motors.pulse_end();
}

