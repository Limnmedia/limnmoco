// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_MOTORS_HPP
#define LIMNMOCO_CM4_MOTORS_HPP

#include <array>
#include <cstring>

#include <Arduino.h>
#include <drivers/Ticker.h>

#include "config.hpp"
#include "common.hpp"
#include "shared.hpp"
#include "status.hpp"

class Motors {
public:
    using Pins         = std::array<uint8_t, LIMNMOCO_MOTOR_COUNT>;
    using Accumulators = std::array<uint32_t, LIMNMOCO_MOTOR_COUNT>;

    Motors();

    void begin();

    Status update_motor_directions();

private:
    void beat();
    void up();
    void down();

    static constexpr Pins m_step_pins = {
        LIMNMOCO_PIN_STEP_0,
        LIMNMOCO_PIN_STEP_1,
        LIMNMOCO_PIN_STEP_2,
        LIMNMOCO_PIN_STEP_3,
        LIMNMOCO_PIN_STEP_4,
        LIMNMOCO_PIN_STEP_5,
        LIMNMOCO_PIN_STEP_6,
        LIMNMOCO_PIN_STEP_7,
    };

    static constexpr Pins m_direction_pins = {
        LIMNMOCO_PIN_DIRECTION_0,
        LIMNMOCO_PIN_DIRECTION_1,
        LIMNMOCO_PIN_DIRECTION_2,
        LIMNMOCO_PIN_DIRECTION_3,
        LIMNMOCO_PIN_DIRECTION_4,
        LIMNMOCO_PIN_DIRECTION_5,
        LIMNMOCO_PIN_DIRECTION_6,
        LIMNMOCO_PIN_DIRECTION_7,
    };


    mbed::Ticker m_heart;
    Directions   m_directions;
    Accumulators m_motor_accumulators;
};

#endif // !LIMNMOCO_CM4_MOTORS_HPP

