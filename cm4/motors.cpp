// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "motors.hpp"
#include "shared.hpp"
#include "utility.hpp"

Motors::Motors() 
    : direction(0)
{
    memset(velocity,    0, sizeof(velocity));
    memset(accumulator, 0, sizeof(accumulator));

    for (uint8_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        pinMode(limnmoco_pin_step[index],      OUTPUT);
        pinMode(limnmoco_pin_direction[index], OUTPUT);

        digitalWrite(limnmoco_pin_step[index],      LOW);
        digitalWrite(limnmoco_pin_direction[index], LOW);
    }
}

void Motors::update() {
    SharedData *sp = SharedData::get();

    direction = sp->motor_direction;

    for (uint8_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        velocity[index] = sp->motor_velocity[index];
    }
}

void Motors::pulse_start() {
    for (uint8_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        // set direction
        digitalWrite(limnmoco_pin_direction[index], BIT(direction ,index) ? HIGH : LOW);

        // begin pulse
        accumulator[index] += velocity[index];
        if (accumulator[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            digitalWrite(limnmoco_pin_step[index], HIGH);
        }
    }
}

void Motors::pulse_end() {
    for (uint8_t index = 0; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        if (accumulator[index] >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
            // write the overflow back into the accumulator
            accumulator[index] -= LIMNMOCO_MOTOR_MAX_VELOCITY;
            digitalWrite(limnmoco_pin_step[index], LOW);
        }
    }
}


    // This pattern will pulse the motors at the maximum velocity possible
    // in the system. In order to pulse motors at a lower velocity we need to
    // delay starting a pulse by some multiple of 5us. (recall that we have a 
    // resolution of 5us, or 200kHz)
    // so we can half the velocity by going every other pulse, third the velocity 
    // by going every third pulse, etc...
    //
    // in general `(1 / N) * velocity`
    //
    // if we store the accumulated velocity in an accumulator, and we pulse each time
    // the accumulated velocity is equal to or greater than 200,000. then we can have
    // an even finer grained control over the velocity of the motor.
    // the maximum velocity would be equal to 200,000. 
    // half that would be equal to 100,000.
    // and we can have 3/4 with 150,000
    // etc...
    //
    // by storing the adding the current velocity to the accumulator, 
    // check if the accumulator is greater than 200,000.
    // if it is begin a pulse, otherwise don't.
    //
