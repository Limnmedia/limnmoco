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

void vec3_init(Vec3* v, double x, double y, double z) {
    assert(v != NULL);
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3_add(Vec3* result, const Vec3* a, const Vec3* b) {
    assert(result != NULL);
    assert(a != NULL);
    assert(b != NULL);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void vec3_sub(Vec3* result, const Vec3* a, const Vec3* b) {
    assert(result != NULL);
    assert(a != NULL);
    assert(b != NULL);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void vec3_scale(Vec3* result, const Vec3* v, double scalar) {
    assert(result != NULL);
    assert(v != NULL);
    result->x = v->x * scalar;
    result->y = v->y * scalar;
    result->z = v->z * scalar;
}

double vec3_dot(const Vec3* a, const Vec3* b) {
    assert(a != NULL);
    assert(b != NULL);
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vec3_cross(Vec3* result, const Vec3* a, const Vec3* b) {
    assert(result != NULL);
    assert(a != NULL);
    assert(b != NULL);
    result->x = a->y * b->z - a->z * b->y;
    result->y = a->z * b->x - a->x * b->z;
    result->z = a->x * b->y - a->y * b->x;
}

double vec3_length(const Vec3* v) {
    assert(v != NULL);
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec3_normalize(Vec3* result, const Vec3* v) {
    assert(result != NULL);
    assert(v != NULL);
    double len = vec3_length(v);
    if (len > 0) {
        result->x = v->x / len;
        result->y = v->y / len;
        result->z = v->z / len;
    } else {
        result->x = 0;
        result->y = 0;
        result->z = 0;
    }
}

// End Public Function Definitions
