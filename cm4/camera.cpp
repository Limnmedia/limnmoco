// SPDX-License-Identifier: GPL-3.0-or-later

#include "camera.hpp"
#include "config.hpp"
#include "global.hpp"
#include "shared.hpp"

Camera::Camera()
    : velocity(0)
    , accumulator(0)
    , angle_open(0)
    , angle_close(0)
    , value(0)
{
    pinMode(LIMNMOCO_PIN_CAMERA_METER,   OUTPUT);
    pinMode(LIMNMOCO_PIN_CAMERA_SHUTTER, OUTPUT);

    digitalWrite(LIMNMOCO_PIN_CAMERA_METER,   LOW);
    digitalWrite(LIMNMOCO_PIN_CAMERA_SHUTTER, LOW);
}

void Camera::update() {
    SharedData *sp = SharedData::get();

    velocity    = sp->camera_velocity;
    angle_open  = sp->camera_angle_open;
    angle_close = sp->camera_angle_close;
    value       = sp->camera_value;
}

void Camera::pulse_start() {
    accumulator += velocity;
    if (accumulator >= LIMNMOCO_MOTOR_MAX_VELOCITY) {
        digitalWrite(
    }
}



