/**
 * Copyright 2026 Cade Weinberg
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     https://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
