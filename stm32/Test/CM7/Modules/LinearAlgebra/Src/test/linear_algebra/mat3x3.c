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
#include <stdlib.h>

#include "test/linear_algebra/mat3x3.h"

#include "linear_algebra/mat3x3.h"
#include "linear_algebra/scalar.h"

static MunitResult test_cm7_linear_algebra_mat3x3_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_equal(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_multiply(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_multiply_identity(
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
        "/mat3x3/equal",
        test_cm7_linear_algebra_mat3x3_equal,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply",
        test_cm7_linear_algebra_mat3x3_multiply,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply/identity",
        test_cm7_linear_algebra_mat3x3_multiply_identity,
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

static void rand_mat3x3(Mat3x3 *M) {
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            M->data[i][j] = munit_rand_double();
        }
    }
}

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

static MunitResult test_cm7_linear_algebra_mat3x3_equal(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 D;
    rand_mat3x3(&A);
    rand_mat3x3(&B);
    mat3x3_identity(&C);
    mat3x3_identity(&D);

    munit_assert_true(mat3x3_equal(&A, &A, SCALAR_CLOSE));
    munit_assert_true(mat3x3_equal(&C, &C, SCALAR_CLOSE));
    munit_assert_true(mat3x3_equal(&C, &D, SCALAR_CLOSE));

    munit_assert_false(mat3x3_equal(&A, &B, SCALAR_CLOSE));
    munit_assert_false(mat3x3_equal(&A, &C, SCALAR_CLOSE));

    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_multiply(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 P;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_multiply(&C, &A, &B);

    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            Scalar s = 0.0;
            for (uint32_t k = 0; k < 3; ++k) {
                s += A.data[i][k] * B.data[k][j];
            }
            P.data[i][j] = s;
        }
    }

    munit_assert_true(mat3x3_equal(&C, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_multiply_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    rand_mat3x3(&A);
    mat3x3_identity(&B);
    mat3x3_multiply(&C, &A, &B);
    munit_assert(mat3x3_equal(&A, &C, SCALAR_CLOSE));
    return MUNIT_OK;
}
