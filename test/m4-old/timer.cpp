// SPDX-License-Identifier: GPL-3.0-or-later

#include "timer.hpp"
#include "global.hpp"
#include "motors.hpp"

namespace Timer {

Ticker motor_pulse_timer;

void on_motor_pulse();

Status begin() {
    motor_pulse_timer.attach(&on_motor_pulse, std::chrono::microseconds{5});
    return STATUS_OK;
}

void on_motor_pulse() {
    Motors::pulse_start();

    delayMicroseconds(3);

    Motors::pulse_end();
}

} // namespace Timer

