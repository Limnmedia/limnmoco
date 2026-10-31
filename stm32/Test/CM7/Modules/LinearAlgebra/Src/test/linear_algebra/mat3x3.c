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
#include "test/linear_algebra/vec3.h"

static MunitResult test_cm7_linear_algebra_mat3x3_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_equal(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_add(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_add_associativity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_add_commutivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_add_identity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_add_inverse(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_sub(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_mul(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_mul_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data);

static MunitResult test_cm7_linear_algebra_mat3x3_mul_associativity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_mul_anticommutivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_mul_distributivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_scale(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_scale_consistent_sub(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_transpose(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_compatible_add(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_compatible_mul(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_transpose(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *);

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
        "/mat3x3/add",
        test_cm7_linear_algebra_mat3x3_add,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/add/associativity",
        test_cm7_linear_algebra_mat3x3_add_associativity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/add/commutivity",
        test_cm7_linear_algebra_mat3x3_add_commutivity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/add/identity",
        test_cm7_linear_algebra_mat3x3_add_identity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/add/inverse",
        test_cm7_linear_algebra_mat3x3_add_inverse,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/sub",
        test_cm7_linear_algebra_mat3x3_sub,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply",
        test_cm7_linear_algebra_mat3x3_mul,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply/identity",
        test_cm7_linear_algebra_mat3x3_mul_identity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply/associativity",
        test_cm7_linear_algebra_mat3x3_mul_associativity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
        {
        "/mat3x3/multiply/anticommutivity",
        test_cm7_linear_algebra_mat3x3_mul_anticommutivity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/multiply/distributivity",
        test_cm7_linear_algebra_mat3x3_mul_distributivity,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/scale",
        test_cm7_linear_algebra_mat3x3_scale,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/scale/consistent/sub",
        test_cm7_linear_algebra_mat3x3_scale_consistent_sub,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/transpose",
        test_cm7_linear_algebra_mat3x3_transpose,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/transpose/compatible/add",
        test_cm7_linear_algebra_mat3x3_transpose_compatible_add,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/transpose/compatible/mul",
        test_cm7_linear_algebra_mat3x3_transpose_compatible_mul,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL,
    },
    {
        "/mat3x3/transpose/transpose",
        test_cm7_linear_algebra_mat3x3_transpose_transpose,
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

static MunitResult test_cm7_linear_algebra_mat3x3_add(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 P;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_add(&C, &A, &B);

    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            P.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }

    munit_assert_true(mat3x3_equal(&C, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_add_associativity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // (A + B) + C == A + (B + C)
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 t0;
    Mat3x3 t1;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);
    rand_mat3x3(&C);

    mat3x3_add(&t0, &A, &B);
    mat3x3_add(&P, &t0, &C);

    mat3x3_add(&t1, &B, &C);
    mat3x3_add(&Q, &t1, &A);

    munit_assert_true(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_add_commutivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // A + B == B + A
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_add(&P, &A, &B);
    mat3x3_add(&Q, &B, &A);
    
    munit_assert_true(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_add_identity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // A + 0 == A
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 P;
    rand_mat3x3(&A);
    mat3x3_splat(&B, 0.0);

    mat3x3_add(&P, &A, &B);

    munit_assert_true(mat3x3_equal(&P, &A, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_add_inverse(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // A + (-A) = 0
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 P;
    rand_mat3x3(&A);
    mat3x3_scale(&B, &A, -1.0);
    mat3x3_splat(&C, 0.0);

    mat3x3_add(&P, &A, &B);

    munit_assert_true(mat3x3_equal(&P, &C, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_sub(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 P;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_sub(&C, &A, &B);

    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            P.data[i][j] = A.data[i][j] - B.data[i][j];
        }
    }

    munit_assert_true(mat3x3_equal(&C, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_mul(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 P;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_mul(&C, &A, &B);

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

static MunitResult test_cm7_linear_algebra_mat3x3_mul_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void * data) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    rand_mat3x3(&A);
    mat3x3_identity(&B);
    mat3x3_mul(&C, &A, &B);
    munit_assert(mat3x3_equal(&A, &C, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_mul_associativity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // (AB)C == A(BC)
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 T0;
    Mat3x3 T1;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);
    rand_mat3x3(&C);

    mat3x3_mul(&T0, &A, &B);
    mat3x3_mul(&P, &T0, &C);

    mat3x3_mul(&T1, &B, &C);
    mat3x3_mul(&Q, &A, &T1);

    munit_assert(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_mul_anticommutivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_mul(&P, &A, &B);
    mat3x3_mul(&Q, &B, &A);

    munit_assert_false(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_mul_distributivity(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // (A + B)C == AC + BC
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 C;
    Mat3x3 T0;
    Mat3x3 T1;
    Mat3x3 T2;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);
    rand_mat3x3(&C);

    mat3x3_add(&T0, &A, &B);
    mat3x3_mul(&P, &T0, &C);

    mat3x3_mul(&T1, &A, &C);
    mat3x3_mul(&T2, &B, &C);
    mat3x3_add(&Q, &T1, &T2);

    munit_assert(mat3x3_equal(&P, &Q, SCALAR_CLOSE));

    // C(A + B) == CA + CB
    mat3x3_add(&T0, &A, &B);
    mat3x3_mul(&P, &C, &T0);

    mat3x3_mul(&T1, &C, &A);
    mat3x3_mul(&T2, &C, &B);
    mat3x3_add(&Q, &T1, &T2);

    munit_assert(mat3x3_equal(&P, &Q, SCALAR_CLOSE));

    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_scale(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Scalar b;
    Mat3x3 C;
    Mat3x3 P;

    rand_mat3x3(&A);
    b = munit_rand_double();

    mat3x3_scale(&C, &A, b);

    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            P.data[i][j] = A.data[i][j] * b;
        }
    }

    munit_assert_true(mat3x3_equal(&C, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_scale_consistent_sub(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // A - B == A + (-1) * B
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 T0;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_sub(&P, &A, &B);

    mat3x3_scale(&T0, &B, -1.0);
    mat3x3_add(&Q, &A, &T0);

    munit_assert_true(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_transpose(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 P;
    rand_mat3x3(&A);

    mat3x3_transpose(&B, &A);

    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            P.data[j][i] = A.data[i][j];
        }
    }

    munit_assert_true(mat3x3_equal(&B, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_compatible_add(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    // T(A + B) == T(A) + T(B)
    Mat3x3 A;
    Mat3x3 B;
    Mat3x3 T0;
    Mat3x3 T1;
    Mat3x3 T2;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    rand_mat3x3(&B);

    mat3x3_add(&T0, &A, &B);
    mat3x3_transpose(&P, &T0);

    mat3x3_transpose(&T1, &A);
    mat3x3_transpose(&T2, &B);
    mat3x3_add(&Q, &T1, &T2);

    munit_assert_true(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_compatible_mul(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
        // T(cA) == cT(A)
    Mat3x3 A;
    Mat3x3 T0;
    Mat3x3 T1;
    Scalar s;
    Mat3x3 P;
    Mat3x3 Q;
    rand_mat3x3(&A);
    s = munit_rand_double();

    mat3x3_scale(&T0, &A, s);
    mat3x3_transpose(&P, &T0);

    mat3x3_transpose(&T1, &A);
    mat3x3_scale(&Q, &T1, s);

    munit_assert_true(mat3x3_equal(&P, &Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_mat3x3_transpose_transpose(
    [[maybe_unused]] const MunitParameter *,
    [[maybe_unused]] void *) {
    Mat3x3 A;
    Mat3x3 T0;
    Mat3x3 P;
    rand_mat3x3(&A);

    mat3x3_transpose(&T0, &A);
    mat3x3_transpose(&P, &T0);

    munit_assert_true(mat3x3_equal(&A, &P, SCALAR_CLOSE));
    return MUNIT_OK;
}

