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


#ifndef LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H
#define LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H

#include <math.h>

typedef double Scalar;

constexpr double SCALAR_CLOSE = (1e-9);

static inline bool scalar_equal(Scalar a, Scalar b, Scalar near) {
    Scalar delta = fabs(a - b);
    return delta <= near;
}

#endif // !LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H