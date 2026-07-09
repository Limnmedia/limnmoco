// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_STATUS_HPP
#define LIMNMOCO_CM4_STATUS_HPP

#include <cstdint>

enum Status : int32_t {
    STATUS_ERR_MOVING = -1,
    STATUS_OK         = 0,
};

#endif // !LIMNMOCO_CM4_STATUS_HPP

