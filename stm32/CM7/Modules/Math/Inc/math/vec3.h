// Copyright 2026 Cade Weinberg
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     https://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file vec3.h
 * @brief Type definition and Function declarations for a 3D vector.
 *
 * @note The STM32H747IH6 MCU has a hard FPU, with support for single 
 * and double precision. The considerations for this project:
 * 1. We need to compute distances, paths, angles, and rotations in 3D space.
 * 2. We need to compute these values fast enough to meet our timing requirements.
 * 3. We need to be cognizant of the memory footprint of our data structures and algorithms.
 * 4. We need to be cognizant of the precision of our calculations.
 *
 * The choice here is a fundamental one: single or double precision?
 * single precision uses half the memory of double precision. 
 * single precision uses less cpu cylces than double precision. (!!!assumtion!!!)
 *
 * The crux of the choice comes down to two questions:
 * 1. do the errors of 32 bit float arithmetic accumulate
 *    over time to the point where the camera position and 
 *    orientation is measurably off from the target position
 *    and orientation?
 * 2. is the memory and speed overhead of using double precision
 *    such that we can no longer fit the program into the available 
 *    memory, or we can no longer meet the timing requirements of 
 *    the program?
 *
 * I am going to make an estimated decision, we will use double prescision,
 * and we will error on the side of precision over speed and memory footprint.
 * 
 * with the caveat that we will need to measure the speed and memory footprint of 
 * the math library, and forthcoming kinematics library, and ensure that we can 
 * meet the timing and memory requirements of the program.
 *
 * This math library is specialized to our current processor, the STM32H747IH6.
 * we aren't planning on porting this library to other processors. and even when
 * considering the CM7 and CM4, the CM7 has support for single and double precision,
 * and the CM4 only supports single precision. Whether or not it makes sense to 
 * compile the Math lirary for the CM4 is highly dependent on the work 
 * that we are delegating to the CM4, and the work that we are delegating to the CM7.
 * if we are asking the CM4 to compute the number of steps to take to reach a target 
 * position, as well as to generate the stepper motor control signals, then it makes 
 * sense to pass floating point values to the CM4 as target distances and angles,
 * and to have the CM4 compute the number of steps to take. However, this might 
 * get in the way of the CM4's ability to meet the timing requirements of the stepper 
 * motor control signals.
 *
 * If we are asking the CM4 to only generate the stepper motor control signals, then we can 
 * pass in step counts as integers, and there is no need for the CM4 to use this floating point
 * math library. Which then implies that the math library should be a module under the CM7 
 * and not a common module.
 *
 * in order to ease the implementation of the CM4 control code, we want to error on the side of 
 * giving the CM4 less work to do, and delegate more of the work to the CM7. 
 * 
 * So I don't think we need to consider compiling the math library for the CM4.
 */

#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include <stdint.h>

// Public Typedef

/**
 * @brief A 3D vector.
 *
 * This struct represents a 3D vector with double precision floating point components.
 *
 * We support the usual vector operations: addition, subtraction, scalar multiplication,
 * dot product, cross product, normalization, and length.
 */
typedef struct _Vec3 {
    double x;
    double y;
    double z;
} Vec3;

// End Public Typedef

// Public Function Declarations

void   vec3_init(Vec3* v, double x, double y, double z);
void   vec3_add(Vec3* result, const Vec3* a, const Vec3* b);
void   vec3_sub(Vec3* result, const Vec3* a, const Vec3* b);
void   vec3_scale(Vec3* result, const Vec3* v, double scalar);
double vec3_dot(const Vec3* a, const Vec3* b);
void   vec3_cross(Vec3* result, const Vec3* a, const Vec3* b);
double vec3_length(const Vec3* v);
void   vec3_normalize(Vec3* result, const Vec3* v);

// End Public Function Declarations

#endif // !MATH_VEC3_H
