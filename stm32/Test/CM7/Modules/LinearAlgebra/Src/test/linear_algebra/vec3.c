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

#include "test/linear_algebra/vec3.h"
#include "linear_algebra/vec3.h"

static MunitResult test_cm7_linear_algebra_vec3_init(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

MunitTest cm7_linear_algebra_vec3_tests[] = {
    {"/vec3/init", test_cm7_linear_algebra_vec3_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static MunitResult test_cm7_linear_algebra_vec3_init(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 v0 = {};
    vec3_init(&v0, 1.0, 2.0, 3.0);
    munit_assert_double_equal(v0.x, 1.0, 9);
    munit_assert_double_equal(v0.y, 2.0, 9);
    munit_assert_double_equal(v0.z, 3.0, 9);
    return MUNIT_OK;
}

