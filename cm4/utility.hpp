// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_UTILITY_HPP
#define LIMNMOCO_CM7_UTILITY_HPP

#define BIT_SET(n, b) ((n) |= (1 << (b)))
#define BIT_CLR(n, b) ((n) &= !(1 << (b)))
#define BIT(n, b) (((n) >> (b)) & 0x1)

#define CLAMP(n, l, h) ((n) < (l) ? (l) : ((n) > (h) ? (h) : (n)))
#define IN_RANGE(n, l, h) ((l) <= (n) && (n) <= (h))


#endif // !LIMNMOCO_CM7_UTILITY_HPP

