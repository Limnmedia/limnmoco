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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "linear_algebra/mat3x3.h"



void mat3x3_identity(Mat3x3 *m) {
    assert(m != NULL);
    m->data[0][0] = 1.0;
    m->data[0][1] = 0.0;
    m->data[0][2] = 0.0;
    m->data[1][0] = 0.0;
    m->data[1][1] = 1.0;
    m->data[1][2] = 0.0;
    m->data[2][0] = 0.0;
    m->data[2][1] = 0.0;
    m->data[2][2] = 1.0;
}

bool mat3x3_equal(Mat3x3 *A, Mat3x3 *B, Scalar near) {
    assert(A != NULL);
    assert(B != NULL);
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < 3; ++j) {
            if (!scalar_equal(A->data[i][j], B->data[i][j], near)) {
                return false;
            }
        }
    }
    return true;
}

void mat3x3_multiply(Mat3x3 *C, Mat3x3 *A, Mat3x3 *B) {
    assert(A != NULL);
    assert(B != NULL);
    assert(C != NULL);


    /*
     * c[i][j] is the dot product of the ith row of A with 
     * the jth column of B. 
     * 
     * 1. c[0][0] = (a[0][0] * b[0][0]) + (a[0][1] * b[1][0]) + (a[0][2] * b[2][0])
     * 2. c[0][1] = (a[0][0] * b[0][1]) + (a[0][1] * b[1][1]) + (a[0][2] * b[2][1])
     * 3. c[0][2] = (a[0][0] * b[0][2]) + (a[0][1] * b[1][2]) + (a[0][2] * b[2][2])
     *
     * 4. c[1][0] = (a[1][0] * b[0][0]) + (a[1][1] * b[1][0]) + (a[1][2] * b[2][0])
     * 5. c[1][1] = (a[1][0] * b[0][1]) + (a[1][1] * b[1][1]) + (a[1][2] * b[2][1])
     * 6. c[1][2] = (a[1][0] * b[0][2]) + (a[1][1] * b[1][2]) + (a[1][2] * b[2][2])
     *
     * 7. c[2][0] = (a[2][0] * b[0][0]) + (a[2][1] * b[1][0]) + (a[2][2] * b[2][0])
     * 8. c[2][1] = (a[2][0] * b[0][1]) + (a[2][1] * b[1][1]) + (a[2][2] * b[2][1])
     * 9. c[2][2] = (a[2][0] * b[0][2]) + (a[2][1] * b[1][2]) + (a[2][2] * b[2][2])
     * 
     * This first miss will occur when we access the first row of a and b.
     * so, step 1 will incur 2 cache misses on the first multiply (a[0][0] * b[0][0])
     * then the next column of b we access will miss the cache again, (a[0][1] * b[1][0])
     * then the third column of b will also miss. (a[0][2] * b[2][0]),
     * then when we attempt the store into C, we also miss (c[0][0] =)
     * Notice that after that, the entire matrix b must be within the cache. So 
     * unless the rows of b are evicted from the cache, we won't miss when we attempt
     * access of the data of b during the rest of the matrix multiply. The next two 
     * stores into C are on the same row, so they must be in the same cache line
     * and the next 6 reads of A are all the same cache line. So the next cache miss 
     * will be step 4, where we miss when we attempt to access the next row of A,
     * (a[1][0] * b[0][0]), and then when we store into the next row of C (c[1][0] =)
     * Then, we don't miss until we get to step 7, where the last row of A is read
     * and the last row of C is written. The cache is 32 bytes, so the first row, plus 
     * one element of the second is pulled in on a cache line, so it would be more accurate
     * to say the miss occurs on step 4, on this access (a[1][1] * b[1][0]), and 
     * similarly for C, where it's this access (c[1][1] =), then, we pull in 4 more 
     * elements from A and C into the cache, and so we don't miss again until we attempt 
     * to access the last elements of those matrices. here (a[2][2] * b[2][0]) and 
     * here (c[2][2] =) on steps 7 and 9 respectively.
     *
     * zooming out a bit, if we consider the larger picture of our program, I am curious 
     * if we could see just how much of the math portion fits into cache all at once.
     * I am noticing that 72 bytes, is much smaller than 16KiB (16,384 Bytes) we can 
     * theoretically hold 227 (= 16384 / 72) full Mat3x3s in cache simultaneously.
     * I need to look up ho many matrices are involved in the Kinematics. 
     * I am just curious exactly how many cache misses we will incur once we are past 
     * the compulsary cache misses of bringing the data into live memory to do work on.
     * Said another way, how much of a bottleneck will the cache be once the program is 
     * up and running and processing the real and virtual positions.
     *
     */

    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; ++j) {
            Scalar s = 0.0;
            for (uint32_t k = 0; k < 3; ++k) {
                s += A->data[i][k] * B->data[k][j];
            }
            C->data[i][j] = s;
        }
    }
}

