// Copyright (C) 2026 cade
// 
// This file is part of Limnmoco.
// 
// Limnmoco is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Limnmoco is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Limnmoco.  If not, see <https://www.gnu.org/licenses/>.

/**
 * @file mat3x3.h
 * @brief Type Definition and public API for a 3 by 3 matrix
 *
 * @note A matrix is a 2 dimensional array from the perspective of 
 * the C programming language. Arrays are store d in row major order,
 * so in order to access the array with the most cache hits, we 
 * need to index the rightmost array in the inner loop.
 *
 * The STM32H747IH6 ARM Cortex M7 has 16KiB of Instruction Cache
 * and 16KiB of Data Cache. 
 */

#ifndef LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H
#define LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H

#include "linear_algebra/scalar.h"

// Begin Public Type Definition

typedef struct _Mat3x3 {
    Scalar data[3][3];
} Mat3x3;

// End Public Type Definition

// Begin Public API

/**
 * @brief initializes the given matrix as an identity matrix
 *
 * @note
 *  | 1  0  0 |
 *  | 0  1  0 |
 *  | 0  0  1 |
 * 
 */
void mat3x3_identity(Mat3x3 *m);

// End Public API

#endif // !LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H
