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

/**
 * @file mat3x3.h
 * @brief Type Definition and public API for a 3 by 3 matrix
 *
 * @note A matrix is a 2 dimensional array from the perspective of 
 * the C programming language. Arrays are store d in row major order,
 * so in order to access the array with the most cache hits, we 
 * need to index the rightmost array in the inner loop.
 *
 * The STM32H747IH6 ARM Cortex M7 has 16KiB of Instruction Cache
 * and 16KiB of Data Cache. and from the Arm Cortex-M7 Technical Reference
 * Manual. Section 5.9 "The data cache is four-way set-associative, 
 * the instruction cache is two-way set-associative. Both caches use a 
 * line-length of 32-bytes. If all the cache lines in a set are valid, 
 * to allocate a different address to the cache, the cache controller 
 * must evict a line from the cache."
 *
 * sizeof(Scalar) == sizeof(double) == 8
 * 8 * 9 == 72
 * 32 * 2 == 64
 * 72 - 64 == 8
 *
 * So a single matrix fits on three cache lines, given that, we will incur 
 * a maximum of three cache misses per matrix, a, b, and c. when needing 
 * to iterate over all elements of each matrix. Once for each portion of the 
 * matrix which is read in on a single line.
 *
 * So, it is clear that passing a matrix by value is over the threshold
 * for what the ABI can support. (recall) [AAPCS32](https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst)
 * section 7.1.2.1 states that a homogenous aggregate with base type of
 * 64-bit with one to four elements, is a VFP co-processor register candidate.
 * That is, the structure may be passed in the FPU co-processor registers.
 * of which there are eight. Given that a 3x3 matrix has nine entries, a single 
 * matrix is too large to be passed in registers. So, the compiler has little room 
 * to decompose the matrix into it's constituent elements and pass them 
 * in registers. 
 *
 * We want to pass by pointer. Even still, does it make sense to inline the common 
 * matrix functions? With access to the implementation, would the compiler 
 * be able to optimize the implementation of the algorithms which we are building 
 * atop these fundamentals? Such as the Forward and Inverse Kinematics.
 * This question bears testing. 
 */

#ifndef LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H
#define LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H

#include "linear_algebra/scalar.h"

// Begin Public Type Definition

typedef struct _Mat3x3 {
    Scalar data[3][3];
} Mat3x3;

// End Public Type Definition

// Begin Public API

/**
 * @brief Fill a matrix with a single value
 */
void mat3x3_splat(Mat3x3 *m, Scalar s);

/**
 * @brief Assign matrix A to B
 */
void mat3x3_assign(Mat3x3 *B, Mat3x3 *A);

/**
 * @brief initializes the given matrix as an identity matrix
 *
 * @note
 *  | 1  0  0 |
 *  | 0  1  0 |
 *  | 0  0  1 |
 * 
 */
void mat3x3_identity(Mat3x3 *M);

/**
 * @brief compare two matricies for elementwise equality
 */
bool mat3x3_equal(Mat3x3 *A, Mat3x3 *B, Scalar near);

/**
 * @brief addition of two matricies
 */
void mat3x3_add(Mat3x3 *C, Mat3x3 *A, Mat3x3 *B);

/**
 * @brief Subtraction of two matricies
 */
void mat3x3_sub(Mat3x3 *C, Mat3x3 *A, Mat3x3 *B);

/**
 * @brief Multiplies two matricies
 *
 * `c = a * b`
 */
void mat3x3_mul(Mat3x3 *C, Mat3x3 *A, Mat3x3 *B);

/**
 * @brief viewing the vector as a column vector
 * does normal matrix multiplication.
 */
// void mat3x3_mul_vec3(Vec3 U, Mat3x3 *A, Vec3 V);

/**
 * @brief Scale matrix A by Scalar b
 *
 * elementwise multiplication by b
 */
void mat3x3_scale(Mat3x3 *C, Mat3x3 *A, Scalar b);

/**
 * @brief Transpose matrix A
 * 
 * B[j][i] = A[i][j] for each i < 3, j < 3
 */
void mat3x3_transpose(Mat3x3 *B, Mat3x3 *A);

// End Public API

#endif // !LIMNMOCO_CM7_MODULES_LINEAR_ALGEBRA_MAT3X3_H
