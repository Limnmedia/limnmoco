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

#include <math.h>

#include "test/linear_algebra/mat3x3.h"

#include "linear_algebra/mat3x3.h"

static MunitResult test_cm7_linear_algebra_mat3x3_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);


MunitTest cm7_linear_algebra_mat3x3_tests[] = {
    {
        "/mat3x3/identity",
        test_cm7_linear_algebra_mat3x3_identity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        NULL,
        NULL,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    }
};

static MunitResult test_cm7_linear_algebra_mat3x3_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 m;
    mat3x3_identity(&m);
    // identity matrix:
    //
    munit_assert_double_equal(m.data[0][0], 1.0, 9);
    munit_assert_double_equal(m.data[0][1], 0.0, 9);
    munit_assert_double_equal(m.data[0][2], 0.0, 9);
    munit_assert_double_equal(m.data[1][0], 0.0, 9);
    munit_assert_double_equal(m.data[1][1], 1.0, 9);
    munit_assert_double_equal(m.data[1][2], 0.0, 9);
    munit_assert_double_equal(m.data[2][0], 0.0, 9);
    munit_assert_double_equal(m.data[2][1], 0.0, 9);
    munit_assert_double_equal(m.data[2][2], 1.0, 9);

    return MUNIT_OK;
}