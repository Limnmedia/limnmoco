// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_CAMERA_HPP
#define LIMNMOCO_CM4_CAMERA_HPP

#include <cstdint>

class Camera {
public:
    Camera();

    void update();
    void pulse_start();
    void pulse_end();

private:
    uint32_t velocity;
    uint32_t accumulator;
    uint16_t angle_open;
    uint16_t angle_close;
    uint8_t  value;
};

#endif // !LIMNMOCO_CM4_CAMERA_HPP

