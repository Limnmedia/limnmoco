/**
 * Copyright (C) 2026 Cade Weinberg
 * 
 * This file is part of Limnmoco.
 * 
 * Limnmoco is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Limnmoco is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Limnmoco.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stddef.h>

#include "linear_algebra/mat3x3.h"

void mat3x3_identity(Mat3x3 *m) {
    assert(m != NULL);
    m->data[0][0] = 1.0;
    m->data[0][1] = 0.0;
    m->data[0][2] = 0.0;
    m->data[1][0] = 0.0;
    m->data[1][1] = 1.0;
    m->data[1][2] = 0.0;
    m->data[2][0] = 0.0;
    m->data[2][1] = 0.0;
    m->data[2][2] = 1.0;
}

