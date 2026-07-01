// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_MOTORS_HPP
#define LIMNMOCO_CM4_MOTORS_HPP

#include <cstdint>

#include "config.hpp"

class Motors {
public:
    Motors();

    void update();
    void pulse_start();
    void pulse_end();

private:
    uint32_t direction;
    uint32_t velocity[LIMNMOCO_MOTOR_COUNT];
    uint32_t accumulator[LIMNMOCO_MOTOR_COUNT];
};

#endif // !LIMNMOCO_CM4_MOTORS_HPP

