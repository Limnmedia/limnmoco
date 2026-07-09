// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "shared.hpp"

namespace shared {
Data *ptr;

void begin() {
    ptr = (Data *)0x3800FD00;
    memset(ptr, 0, sizeof(Data));
}
} // namespace shared

