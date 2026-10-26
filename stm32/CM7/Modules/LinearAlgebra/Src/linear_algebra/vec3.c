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


#include <assert.h>
#include <stddef.h>
#include <math.h>

#include "linear_algebra/vec3.h"

// Public Function Definitions

extern inline Vec3 vec3_init(Scalar x, Scalar y, Scalar z);
extern inline Vec3 vec3_splat(Scalar x);
extern inline bool vec3_equal(Vec3 a, Vec3 b, Scalar near);
extern inline bool vec3_equal_scalar(Vec3 a, Scalar b, Scalar near);
extern inline Vec3 vec3_add(Vec3 a, Vec3 b);
extern inline Vec3 vec3_sub(Vec3 a, Vec3 b);
extern inline Vec3 vec3_scale(Vec3 v, Scalar s);
extern inline Scalar vec3_dot(Vec3 a, Vec3 b);
extern inline Vec3 vec3_cross(Vec3 a, Vec3 b);
extern inline Scalar vec3_length(Vec3 v);
extern inline Vec3 vec3_normalize(Vec3 v);

// End Public Function Definitions
