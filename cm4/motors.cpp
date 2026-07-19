// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>
#include <bitset>

#include <Arduino.h>
#include <drivers/Ticker.h>

#include "config.hpp"
#include "motors.hpp"

Motors::Motors() {
    m_motor_accumulators.fill(0);

    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        pinMode(m_step_pins[index],      OUTPUT);
        pinMode(m_direction_pins[index], OUTPUT);

        digitalWrite(m_step_pins[index],      LOW);
        digitalWrite(m_direction_pins[index], LOW);
    }
}

void Motors::begin() {
    m_heart.attach(&Motors::beat, std::chrono::microseconds{5});
}

Status Motors::update_motor_directions() {
    Shared *ptr   = Shared::ptr();
    Status status = STATUS_OK;
    Directions shared_directions = ptr->get_directions();

    if (shared_directions == m_directions) {
        return status; // #NOTE: early return since there is no change in direction.
    }

    m_directions = shared_directions;

    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        bool moving = false;
        status = is_motor_moving(index, moving);
        if (status != STATUS_OK) {
            break;
        }

        if (moving) {
            status = STATUS_ERR_MOVING;
            continue;
        }

        digitalWrite(m_direction_pin[index], m_directions[index] ? HIGH : LOW);
    }

    return status;
}

void Motors::beat() {
    up();
    delayMicroseconds(3);
    down();
}

void Motors::up() {
    Shared *ptr = Shared::ptr();
    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        uint32_t acceleration = 0;
        Status status = ptr->get_motor_acceleration(index, acceleration);
        if (status != STATUS_OK) {
            break;
        }

        uint32_t velocity = 0;
        status = ptr->get_motor_velocity(index, velocity);
        if (status != STATUS_OK) {
            break;
        }

        velocity += acceleration;
        status = ptr->set_motor_velocity(index, velocity);
        if (status != STATUS_OK) {
            break;
        }
        m_accumulators[index] += velocity;

        if (m_accumulators[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            digitalWrite(m_step_pins[index], HIGH);
        }
    }
}

void Motors::down() {
    Shared *ptr = Shared::ptr();
    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        if (m_accumulators[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            int32_t position = 0;
            Status status = ptr->get_motor_position(index, position);
            if (status != STATUS_OK) {
                break;
            }

            bool direction = false;
            status = ptr->get_motor_direction(index, direction);
            if (status != STATUS_OK) {
                break;
            }

            position += direction ? 1 : -1;
            m_accumulators[index] -= LIMNMOCO_MOTOR_MAX_VELOCITY;
            digitalWrite(m_step_pins[index], LOW);
        }

    }
}

