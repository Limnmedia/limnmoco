// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>
#include <bitset>

#include <Arduino.h>
//#include <RPC.h>
//#include <mbed/mbed.h>
#include "drivers/Ticker.h"

#include "config.hpp"
#include "motors.hpp"
#include "shared.hpp"
#include "status.hpp"

namespace motors {

using Direction = std::bitset<LIMNMOCO_MOTOR_COUNT>;

mbed::Ticker _pulse;
//Direction    _direction;
//uint32_t     _velocity[LIMNMOCO_MOTOR_COUNT];
uint32_t     _accumulator[LIMNMOCO_MOTOR_COUNT];

uint8_t _step_pin[LIMNMOCO_MOTOR_COUNT] = {
    LIMNMOCO_PIN_STEP_0,
    LIMNMOCO_PIN_STEP_1,
    LIMNMOCO_PIN_STEP_2,
    LIMNMOCO_PIN_STEP_3,
    LIMNMOCO_PIN_STEP_4,
    LIMNMOCO_PIN_STEP_5,
    LIMNMOCO_PIN_STEP_6,
    LIMNMOCO_PIN_STEP_7,
};

uint8_t _direction_pin[LIMNMOCO_MOTOR_COUNT] = {
    LIMNMOCO_PIN_DIRECTION_0,
    LIMNMOCO_PIN_DIRECTION_1,
    LIMNMOCO_PIN_DIRECTION_2,
    LIMNMOCO_PIN_DIRECTION_3,
    LIMNMOCO_PIN_DIRECTION_4,
    LIMNMOCO_PIN_DIRECTION_5,
    LIMNMOCO_PIN_DIRECTION_6,
    LIMNMOCO_PIN_DIRECTION_7,
};

static void on_pulse();
static void pulse_start();
static void pulse_end();

void begin() {
    _pulse.attach(&on_pulse, std::chrono::microseconds{5});
    memset(_accumulator, 0, sizeof(_accumulator));

    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        pinMode(_step_pin[index],      OUTPUT);
        pinMode(_direction_pin[index], OUTPUT);

        digitalWrite(_step_pin[index],      LOW);
        digitalWrite(_direction_pin[index], LOW);
    }
}

// #WARN: it is not safe to change direction while moving the motor.
//        the cm7 control code must be careful to slow the motor to 
//        a complete stop before changing direction.
//        so is it even safe to set the direction pin in this loop?
        //        yes. even if we separate the code into a different subroutine
        //        the cm7 would still be able to change direction without slowing.
        //        we have to introduce more logic to handle "cannot change direction
        //        while motor is in motion"
        //        given that, is it necessary to update the direction at 200kHz?
        //        probably not. it's only necessary while the motor is at rest.
        //        so what does the api look like which allows one to change the motor 
        //        direction? we can register some functions into the RPC, one of which allows
        //        updating the motor direction. and it can return a status code which 
        //        indicates that the motor is in motion.
        //        RPC is a syncronous operation, will it block the evaluation of the ticker interrupt?
        //        if it does it will get in the way of the step pulse generation, if it breaks the 
        //        m4 when it is about to send a step, that step will happen later, which if the motor
        //        is moving, could cause the motor to lose position due to the torque on the system.
        //        if it happens while a pulse is high, the pulse will be high for longer than usual.
        //        this requires RPC to block code for longer than 5us as far as i'm aware, if the step 
        //        generation is blocked for longer than it takes for a pulse to be generated, the motor 
        //        will have time to move to the next expected step position due to inertia and the load
        //        of the system before the next step pulse is generated.
        //        its not as obvious an API, though we could hold a copy of the current direction of the motor
        //        and take it as a suggestion to update the direction. only applying the actual update when 
        //        the motor in question has 0 velocity. (it's stopped)
        //        RPC is the proper way  to do this, as it makes explicit the desire to the m4 and m7 cores.
int32_t set_direction(uint32_t direction) {
    Direction d = direction;
    Status status = STATUS_OK;
    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        // set direction
        if (shared::ptr->motor_velocity[index] == 0) {
            digitalWrite(_direction_pin[index], d[index] ? HIGH : LOW);
        } else {
            status = STATUS_ERR_MOVING;
        }
    }

    return status;
}

static void on_pulse() {
    pulse_start();
    delayMicroseconds(3);
    pulse_end();
}

static void pulse_start() {
    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        // begin pulse
        shared::ptr->motor_velocity[index] += shared::ptr->motor_acceleration[index];
        _accumulator[index]                += shared::ptr->motor_velocity[index];
        if (_accumulator[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            digitalWrite(_step_pin[index], HIGH);
        }
    }
}

static void pulse_end() {
    for (uint32_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        if (_accumulator[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            // update position of motor
            shared::ptr->motor_position[index] += shared::ptr->motor_direction[index] ? 1 : -1;
            // write the overflow back into the accumulator
            _accumulator[index] -= LIMNMOCO_MOTOR_MAX_VELOCITY;
            digitalWrite(_step_pin[index], LOW);
        }
    }
}



} // namespace Motors

