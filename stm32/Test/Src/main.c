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

static MunitSuite suites[] = {
    {"/cm7/linear_algebra", cm7_linear_algebra_vec3_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    {NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE}
};

static MunitSuite suite = {
    "limnmoco",
    NULL,
    suites,
    1,
    MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* const *argv) {
    return munit_suite_main(&suite, NULL, argc, argv);
}

