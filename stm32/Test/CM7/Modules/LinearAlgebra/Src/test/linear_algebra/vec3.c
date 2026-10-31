/**
 * Copyright (C) 2026 cade
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

#include "test/linear_algebra/vec3.h"

#include "linear_algebra/vec3.h"

static MunitResult test_cm7_linear_algebra_vec3_init(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_add(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// U + (V + W) = (U + V) + W
static MunitResult test_cm7_linear_algebra_vec3_add_associativity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// U + V = V + U
static MunitResult test_cm7_linear_algebra_vec3_add_commutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// V + 0 = V 
static MunitResult test_cm7_linear_algebra_vec3_add_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// V + (-V) = 0
static MunitResult test_cm7_linear_algebra_vec3_add_inverse(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_sub(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_scale(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// a(U + V) = aU + aV
static MunitResult test_cm7_linear_algebra_vec3_scale_distributivity_vector(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// (a + b)V = aV + bV
static MunitResult test_cm7_linear_algebra_vec3_scale_distributivity_field(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// a(bV) = (ab)V
static MunitResult test_cm7_linear_algebra_vec3_scale_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// 1V = V
static MunitResult test_cm7_linear_algebra_vec3_scale_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_dot(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// dot(U, V) = dot(V, U)
static MunitResult test_cm7_linear_algebra_vec3_dot_commutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// a(dot(U, V)) = dot(aU, V) = dot(U, aV)
static MunitResult test_cm7_linear_algebra_vec3_dot_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// dot((aU + bV), (cW + dX)) = ac(dot(U, W)) + ad(dot(U, X)) + bc(dot(V, W)) + bd(dot(V, X)) 
static MunitResult test_cm7_linear_algebra_vec3_dot_bilinearity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_cross(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(V, V) = 0
static MunitResult test_cm7_linear_algebra_vec3_cross_self(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(U, V) = -(cross(V, U))
static MunitResult test_cm7_linear_algebra_vec3_cross_anticommutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(U, V + W) == cross(U, V) + cross(U, W)
static MunitResult test_cm7_linear_algebra_vec3_cross_distributivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(aU, V) = cross(U, aV) = a(cross(U, V))
static MunitResult test_cm7_linear_algebra_vec3_cross_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(U, cross(V, W)) + cross(V, cross(W, U)) + cross(W, cross(U, V)) = 0
static MunitResult test_cm7_linear_algebra_vec3_cross_jacobi(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// dot(U, cross(V, W)) = dot(V, cross(W, U)) = dot(W, cross(U, V))
static MunitResult test_cm7_linear_algebra_vec3_scalar_triple_product(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

// cross(U, cross(V, W)) = (dot(U, W))V - (dot(U, V))W
static MunitResult test_cm7_linear_algebra_vec3_vector_triple_product(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_length(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

static MunitResult test_cm7_linear_algebra_vec3_normalize(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data);

MunitTest cm7_linear_algebra_vec3_tests[] = {
    {"/vec3/init", test_cm7_linear_algebra_vec3_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/add", test_cm7_linear_algebra_vec3_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/add/associativity", test_cm7_linear_algebra_vec3_add_associativity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/add/commutivity", test_cm7_linear_algebra_vec3_add_commutivity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/add/identity", test_cm7_linear_algebra_vec3_add_identity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/add/inverse", test_cm7_linear_algebra_vec3_add_inverse, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/sub", test_cm7_linear_algebra_vec3_sub, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scale", test_cm7_linear_algebra_vec3_scale, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scale/distributivity/vector", test_cm7_linear_algebra_vec3_scale_distributivity_vector, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scale/distributivity/field", test_cm7_linear_algebra_vec3_scale_distributivity_field, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scale/compatibility", test_cm7_linear_algebra_vec3_scale_compatibility, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scale/identity", test_cm7_linear_algebra_vec3_scale_identity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/dot", test_cm7_linear_algebra_vec3_dot, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/dot/commutivity", test_cm7_linear_algebra_vec3_dot_commutivity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/dot/compatibility", test_cm7_linear_algebra_vec3_dot_compatibility, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/dot/bilinearity", test_cm7_linear_algebra_vec3_dot_bilinearity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross", test_cm7_linear_algebra_vec3_cross, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross/self", test_cm7_linear_algebra_vec3_cross_self, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross/anticommutivity", test_cm7_linear_algebra_vec3_cross_anticommutivity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross/distributivity", test_cm7_linear_algebra_vec3_cross_distributivity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross/compatibility", test_cm7_linear_algebra_vec3_cross_compatibility, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/cross/jacobi", test_cm7_linear_algebra_vec3_cross_jacobi, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/scalar_triple_product", test_cm7_linear_algebra_vec3_scalar_triple_product, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/vector_triple_product", test_cm7_linear_algebra_vec3_vector_triple_product, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/length", test_cm7_linear_algebra_vec3_length, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vec3/normalize", test_cm7_linear_algebra_vec3_normalize, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static Vec3 rand_vec3() {
    return vec3(munit_rand_double(), munit_rand_double(), munit_rand_double());
}

static MunitResult test_cm7_linear_algebra_vec3_init(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double d0 = munit_rand_double();
    double d1 = munit_rand_double();
    double d2 = munit_rand_double();
    Vec3 v0 = vec3(d0, d1, d2);
    munit_assert_double_equal(v0.x, d0, 9);
    munit_assert_double_equal(v0.y, d1, 9);
    munit_assert_double_equal(v0.z, d2, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_add(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    double b0 = munit_rand_double();
    double b1 = munit_rand_double();
    double b2 = munit_rand_double();
    Vec3 v1 = vec3(a0, a1, a2);
    Vec3 v2 = vec3(b0, b1, b2);
    Vec3 v0 = vec3_add(v1, v2);
    double c0 = a0 + b0;
    double c1 = a1 + b1;
    double c2 = a2 + b2;
    munit_assert_double_equal(v0.x, c0, 9);
    munit_assert_double_equal(v0.y, c1, 9);
    munit_assert_double_equal(v0.z, c2, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_add_associativity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // U + (V + W) = (U + V) + W
    Vec3 U  = rand_vec3();
    Vec3 V  = rand_vec3();
    Vec3 W  = rand_vec3();
    Vec3 P  = {};
    Vec3 Q  = {};
    // P = U + (V + W)
    P = vec3_add(U, vec3_add(V, W));
    // Q = (U + V) + W
    Q = vec3_add(vec3_add(U, V), W);
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_add_commutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // U + V = V + U
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    P = vec3_add(U, V);
    Q = vec3_add(V, U);
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_add_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // V + 0 = V
    Vec3 V = rand_vec3();
    Vec3 I = vec3(0.0, 0.0, 0.0);
    Vec3 P = {};
    P = vec3_add(V, I);
    munit_assert_true(vec3_equal(P, V, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_add_inverse(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // V + (-V) = 0
    Vec3 V = rand_vec3();
    Vec3 P = {};
    P = vec3_add(V, vec3_scale(V, -1.0));
    munit_assert_true(vec3_equal_scalar(P, 0.0, SCALAR_CLOSE));
    return MUNIT_OK;
}


static MunitResult test_cm7_linear_algebra_vec3_sub(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    double b0 = munit_rand_double();
    double b1 = munit_rand_double();
    double b2 = munit_rand_double();
    Vec3 v0 = vec3(0.0, 0.0, 0.0);
    Vec3 v1 = vec3(a0, a1, a2);
    Vec3 v2 = vec3(b0, b1, b2);
    v0 = vec3_sub(v1, v2);
    double c0 = a0 - b0;
    double c1 = a1 - b1;
    double c2 = a2 - b2;
    munit_assert_double_equal(v0.x, c0, 9);
    munit_assert_double_equal(v0.y, c1, 9);
    munit_assert_double_equal(v0.z, c2, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_scale(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    Vec3 v0 = vec3(0.0, 0.0, 0.0);
    Vec3 v1 = vec3(a0, a1, a2);
    double b = munit_rand_double();
    v0 = vec3_scale(v1, b);
    double c0 = a0 * b;
    double c1 = a1 * b;
    double c2 = a2 * b;
    munit_assert_double_equal(v0.x, c0, 9);
    munit_assert_double_equal(v0.y, c1, 9);
    munit_assert_double_equal(v0.z, c2, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_scale_distributivity_vector(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // a(U + V) = aU + aV
    double a = munit_rand_double();
    Vec3 V = rand_vec3();
    Vec3 U = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // a(U + V)
    P = vec3_scale(vec3_add(U, V), a);
    // aU + aV
    Q = vec3_add(vec3_scale(U, a), vec3_scale(V, a));
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_scale_distributivity_field(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // (a + b)V = aV + bV
    double a = munit_rand_double();
    double b = munit_rand_double();
    Vec3 V = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // (a + b)V
    P = (vec3_scale(V, a + b));
    // aV + bV
    Q = vec3_add(vec3_scale(V, a), vec3_scale(V, b));
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_scale_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // a(bV) = (ab)V
    double a = munit_rand_double();
    double b = munit_rand_double();
    Vec3 V = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // a(bV)
    P = vec3_scale(vec3_scale(V, b), a);
    // (ab)V
    Q = vec3_scale(V, a * b);
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_scale_identity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    // 1V = V
    Vec3 V = rand_vec3();
    Vec3 P = {};
    P = vec3_scale(V, 1.0);
    munit_assert_true(vec3_equal(P, V, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_dot(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    double b0 = munit_rand_double();
    double b1 = munit_rand_double();
    double b2 = munit_rand_double();
    Vec3 v0 = vec3(a0, a1, a2);
    Vec3 v1 = vec3(b0, b1, b2);
    double c0 = vec3_dot(v0, v1);
    double d0 = (a0 * b0) + (a1 * b1) + (a2 * b2);
    munit_assert_double_equal(c0, d0, 9);
    return MUNIT_OK;
}

// dot(U, V) = dot(V, U)
static MunitResult test_cm7_linear_algebra_vec3_dot_commutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    double P = vec3_dot(U, V);
    double Q = vec3_dot(V, U);
    munit_assert_double_equal(P, Q, 9);
    return MUNIT_OK;
}

// a(dot(U, V)) = dot(aU, V) = dot(U, aV)
static MunitResult test_cm7_linear_algebra_vec3_dot_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a = munit_rand_double();
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    // a(dot(U, V))
    double P = a * vec3_dot(U, V);
    // dot(aU, V)
    double Q = vec3_dot(vec3_scale(U, a), V);
    // dot(U, aV)
    double R = vec3_dot(U, vec3_scale(V, a));
    munit_assert_double_equal(P, Q, 9);
    munit_assert_double_equal(P, R, 9);
    munit_assert_double_equal(Q, R, 9);
    return MUNIT_OK;
}


// dot((aU + bV), (cW + dX)) = ac(dot(U, W)) + ad(dot(U, X)) + bc(dot(V, W)) + bd(dot(V, X)) 
static MunitResult test_cm7_linear_algebra_vec3_dot_bilinearity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a = munit_rand_double();
    double b = munit_rand_double();
    double c = munit_rand_double();
    double d = munit_rand_double();
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 W = rand_vec3();
    Vec3 X = rand_vec3();
    double P = 0.0;
    double Q = 0.0;
    // dot((aU + bV), (cW + dX))
    P = vec3_dot(vec3_add(vec3_scale(U, a), vec3_scale(V, b)),
                 vec3_add(vec3_scale(W, c), vec3_scale(X, d)));
    // ac(dot(U, W)) + ad(dot(U, X)) + bc(dot(V, W)) + bd(dot(V, X))
    Q = (a * c) * vec3_dot(U, W) 
      + (a * d) * vec3_dot(U, X) 
      + (b * c) * vec3_dot(V, W) 
      + (b * d) * vec3_dot(V, X);
    munit_assert_double_equal(P, Q, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_cross(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    double b0 = munit_rand_double();
    double b1 = munit_rand_double();
    double b2 = munit_rand_double();
    Vec3 v0 = vec3(0.0, 0.0, 0.0);
    Vec3 v1 = vec3(a0, a1, a2);
    Vec3 v2 = vec3(b0, b1, b2);
    v0 = vec3_cross(v1, v2);
    double c0 = a1 * b2 - a2 * b1;
    double c1 = a2 * b0 - a0 * b2;
    double c2 = a0 * b1 - a1 * b0;
    munit_assert_double_equal(v0.x, c0, 9);
    munit_assert_double_equal(v0.y, c1, 9);
    munit_assert_double_equal(v0.z, c2, 9);
    return MUNIT_OK;
}

// cross(V, V) = 0
static MunitResult test_cm7_linear_algebra_vec3_cross_self(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 V = rand_vec3();
    Vec3 P = {};
    P = vec3_cross(V, V);
    munit_assert_true(vec3_equal_scalar(P, 0.0, SCALAR_CLOSE));
    return MUNIT_OK;
}

// cross(U, V) = -(cross(V, U))
static MunitResult test_cm7_linear_algebra_vec3_cross_anticommutivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // cross(U, V)
    P = vec3_cross(U, V);
    // cross(V, U)
    Q = vec3_scale(vec3_cross(V, U), -1.0);
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

// cross(U, V + W) == cross(U, V) + cross(U, W)
static MunitResult test_cm7_linear_algebra_vec3_cross_distributivity(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 W = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // cross(U, V + W)
    P = vec3_cross(U, vec3_add(V, W));
    // cross(U, V) + cross(U, W)
    Q = vec3_add(vec3_cross(U, V), vec3_cross(U, W));
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

// cross(aU, V) = cross(U, aV) = a(cross(U, V))
static MunitResult test_cm7_linear_algebra_vec3_cross_compatibility(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a = munit_rand_double();
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    Vec3 R = {};
    // cross(aU, V)
    P = vec3_cross(vec3_scale(U, a), V);
    // cross(U, aV)
    Q = vec3_cross(U, vec3_scale(V, a));
    // a(cross(U, V))
    R = vec3_scale(vec3_cross(U, V), a);
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    munit_assert_true(vec3_equal(Q, R, SCALAR_CLOSE));
    munit_assert_true(vec3_equal(P, R, SCALAR_CLOSE));
    return MUNIT_OK;
}

// cross(U, cross(V, W)) + cross(V, cross(W, U)) + cross(W, cross(U, V)) = 0
static MunitResult test_cm7_linear_algebra_vec3_cross_jacobi(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 W = rand_vec3();
    Vec3 P = vec3_add(vec3_add(vec3_cross(U, vec3_cross(V, W)), 
                                 vec3_cross(V, vec3_cross(W, U))), 
                      vec3_cross(W, vec3_cross(U, V)));
    munit_assert_true(vec3_equal_scalar(P, 0.0, SCALAR_CLOSE));
    return MUNIT_OK;
}

// dot(U, cross(V, W)) = dot(V, cross(W, U)) = dot(W, cross(U, V))
static MunitResult test_cm7_linear_algebra_vec3_scalar_triple_product(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 W = rand_vec3();
    double P = 0.0;
    double Q = 0.0;
    double R = 0.0;
    // dot(U, cross(V, W))
    P = vec3_dot(U, vec3_cross(V, W));
    // dot(V, cross(W, U))
    Q = vec3_dot(V, vec3_cross(W, U));
    // dot(W, cross(U, V))
    R = vec3_dot(W, vec3_cross(U, V));
    munit_assert_true(scalar_equal(P, Q, SCALAR_CLOSE));
    munit_assert_true(scalar_equal(P, R, SCALAR_CLOSE));
    munit_assert_true(scalar_equal(Q, R, SCALAR_CLOSE));
    return MUNIT_OK;
}

// cross(U, cross(V, W)) = (dot(U, W))V - (dot(U, V))W
static MunitResult test_cm7_linear_algebra_vec3_vector_triple_product(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    Vec3 U = rand_vec3();
    Vec3 V = rand_vec3();
    Vec3 W = rand_vec3();
    Vec3 P = {};
    Vec3 Q = {};
    // cross(U, cross(V, W))
    P = vec3_cross(U, vec3_cross(V, W));
    // (dot(U, W))V - (dot(U, V))W
    Q = vec3_sub(vec3_scale(V, vec3_dot(U, W)), vec3_scale(W, vec3_dot(U, V)));
    munit_assert_true(vec3_equal(P, Q, SCALAR_CLOSE));
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_length(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    Vec3 v0 = vec3(a0, a1, a2);
    double b0 = vec3_length(v0);
    double c0 = sqrt(a0 * a0 + a1 * a1 + a2 * a2);
    munit_assert_double_equal(b0, c0, 9);
    return MUNIT_OK;
}

static MunitResult test_cm7_linear_algebra_vec3_normalize(
    [[maybe_unused]] const MunitParameter *parameter,
    [[maybe_unused]] void *data) {
    double a0 = munit_rand_double();
    double a1 = munit_rand_double();
    double a2 = munit_rand_double();
    Vec3 v0 = vec3(a0, a1, a2);
    Vec3 v1 = vec3(0.0, 0.0, 0.0);
    v1 = vec3_normalize(v0);
    double l = sqrt(a0 * a0 + a1 * a1 + a2 * a2);
    double b0 = a0 / l;
    double b1 = a1 / l;
    double b2 = a2 / l;
    munit_assert_double_equal(v1.x, b0, 9);
    munit_assert_double_equal(v1.y, b1, 9);
    munit_assert_double_equal(v1.z, b2, 9);
    return MUNIT_OK;
}

