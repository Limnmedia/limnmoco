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
 *
 * on the topic of the api of Vec3. pass-by-value or pass-by-pointer?
 * if we refer to the [AAPCS32](https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst)
 * section 7.1.2.1 states that a homogenous aggregate with base type of 64-bit with 
 * one to four elements, is a VFP co-processor register candidate.
 * That is, the structure may be passed in the FPU co-processor registers. of which there are eight.
 * so vec3 operations on two pass-by-value vec3's may have all elements passed by register.
 * This could introduce a meaningful performance benefit over having to access the elements
 * through a pointer.
 *
 * as a point of comparison: stm32cube/bundles/gnu-tools-for-stm32/14.3.1+st.2/bin/arm-none-eabi-gcc
 * ```C
    typedef double Scalar;

    typedef struct _Vec3 {
        Scalar x;
        Scalar y;
        Scalar z;
    } Vec3;

    // ...

    void vec3_add(Vec3* result, const Vec3* a, const Vec3* b) {
        result->x = a->x + b->x;
        result->y = a->y + b->y;
        result->z = a->z + b->z;
    }

    // ...

   ```
   compiling -O0 -g emitted:
   ```asm
    00000000 <vec3_add>:
    0:	b580      	push	{r7, lr}
    2:	b084      	sub	sp, #16
    4:	af00      	add	r7, sp, #0
    6:	60f8      	str	r0, [r7, #12]
    8:	60b9      	str	r1, [r7, #8]
    a:	607a      	str	r2, [r7, #4]
    c:	68fb      	ldr	r3, [r7, #12]
    e:	2b00      	cmp	r3, #0
    10:	d105      	bne.n	1e <vec3_add+0x1e>
    12:	4b1e      	ldr	r3, [pc, #120]	@ (8c <vec3_add+0x8c>)
    14:	4a1e      	ldr	r2, [pc, #120]	@ (90 <vec3_add+0x90>)
    16:	2125      	movs	r1, #37	@ 0x25
    18:	481e      	ldr	r0, [pc, #120]	@ (94 <vec3_add+0x94>)
    1a:	f7ff fffe 	bl	0 <__assert_func>
    1e:	68bb      	ldr	r3, [r7, #8]
    20:	2b00      	cmp	r3, #0
    22:	d105      	bne.n	30 <vec3_add+0x30>
    24:	4b1c      	ldr	r3, [pc, #112]	@ (98 <vec3_add+0x98>)
    26:	4a1a      	ldr	r2, [pc, #104]	@ (90 <vec3_add+0x90>)
    28:	2126      	movs	r1, #38	@ 0x26
    2a:	481a      	ldr	r0, [pc, #104]	@ (94 <vec3_add+0x94>)
    2c:	f7ff fffe 	bl	0 <__assert_func>
    30:	687b      	ldr	r3, [r7, #4]
    32:	2b00      	cmp	r3, #0
    34:	d105      	bne.n	42 <vec3_add+0x42>
    36:	4b19      	ldr	r3, [pc, #100]	@ (9c <vec3_add+0x9c>)
    38:	4a15      	ldr	r2, [pc, #84]	@ (90 <vec3_add+0x90>)
    3a:	2127      	movs	r1, #39	@ 0x27
    3c:	4815      	ldr	r0, [pc, #84]	@ (94 <vec3_add+0x94>)
    3e:	f7ff fffe 	bl	0 <__assert_func>
    42:	68bb      	ldr	r3, [r7, #8]
    44:	ed93 6b00 	vldr	d6, [r3]
    48:	687b      	ldr	r3, [r7, #4]
    4a:	ed93 7b00 	vldr	d7, [r3]
    4e:	ee36 7b07 	vadd.f64	d7, d6, d7
    52:	68fb      	ldr	r3, [r7, #12]
    54:	ed83 7b00 	vstr	d7, [r3]
    58:	68bb      	ldr	r3, [r7, #8]
    5a:	ed93 6b02 	vldr	d6, [r3, #8]
    5e:	687b      	ldr	r3, [r7, #4]
    60:	ed93 7b02 	vldr	d7, [r3, #8]
    64:	ee36 7b07 	vadd.f64	d7, d6, d7
    68:	68fb      	ldr	r3, [r7, #12]
    6a:	ed83 7b02 	vstr	d7, [r3, #8]
    6e:	68bb      	ldr	r3, [r7, #8]
    70:	ed93 6b04 	vldr	d6, [r3, #16]
    74:	687b      	ldr	r3, [r7, #4]
    76:	ed93 7b04 	vldr	d7, [r3, #16]
    7a:	ee36 7b07 	vadd.f64	d7, d6, d7
    7e:	68fb      	ldr	r3, [r7, #12]
    80:	ed83 7b04 	vstr	d7, [r3, #16]
    84:	bf00      	nop
    86:	3710      	adds	r7, #16
    88:	46bd      	mov	sp, r7
    8a:	bd80      	pop	{r7, pc}
    8c:	00000070 	.word	0x00000070
    90:	00000000 	.word	0x00000000
    94:	0000000c 	.word	0x0000000c
    98:	00000000 	.word	0x00000000
    9c:	00000080 	.word	0x00000080
    ```
 * and when we static inline vec3_add:
   ```C
    int main(void) {
        Vec3 a = {1.0, 1.0, 1.0};
        Vec3 b = {1.0, 1.0, 1.0};
        // ...
        while (true) {
            a = vec3_add(a, b);
        }
    //...
    }
   ```

   ```asm
    00000000 <main>:
    0:	b5b0      	push	{r4, r5, r7, lr}
    2:	b090      	sub	sp, #64	@ 0x40
    4:	af00      	add	r7, sp, #0
    6:	4b3f      	ldr	r3, [pc, #252]	@ (104 <main+0x104>)
    8:	f107 0420 	add.w	r4, r7, #32
    c:	461d      	mov	r5, r3
    e:	cd0f      	ldmia	r5!, {r0, r1, r2, r3}
    10:	c40f      	stmia	r4!, {r0, r1, r2, r3}
    12:	e895 0003 	ldmia.w	r5, {r0, r1}
    16:	e884 0003 	stmia.w	r4, {r0, r1}
    1a:	4b3a      	ldr	r3, [pc, #232]	@ (104 <main+0x104>)
    1c:	f107 0408 	add.w	r4, r7, #8
    20:	461d      	mov	r5, r3
    22:	cd0f      	ldmia	r5!, {r0, r1, r2, r3}
    24:	c40f      	stmia	r4!, {r0, r1, r2, r3}
    26:	e895 0003 	ldmia.w	r5, {r0, r1}
    2a:	e884 0003 	stmia.w	r4, {r0, r1}
    2e:	f7ff fffe 	bl	0 <main>
    32:	f64f 73ff 	movw	r3, #65535	@ 0xffff
    36:	63fb      	str	r3, [r7, #60]	@ 0x3c
    38:	bf00      	nop
    3a:	4b33      	ldr	r3, [pc, #204]	@ (108 <main+0x108>)
    3c:	681b      	ldr	r3, [r3, #0]
    3e:	f403 4300 	and.w	r3, r3, #32768	@ 0x8000
    42:	2b00      	cmp	r3, #0
    44:	d004      	beq.n	50 <main+0x50>
    46:	6bfb      	ldr	r3, [r7, #60]	@ 0x3c
    48:	1e5a      	subs	r2, r3, #1
    4a:	63fa      	str	r2, [r7, #60]	@ 0x3c
    4c:	2b00      	cmp	r3, #0
    4e:	dcf4      	bgt.n	3a <main+0x3a>
    50:	6bfb      	ldr	r3, [r7, #60]	@ 0x3c
    52:	2b00      	cmp	r3, #0
    54:	da01      	bge.n	5a <main+0x5a>
    56:	f7ff fffe 	bl	0 <main>
    5a:	f7ff fffe 	bl	0 <HAL_Init>
    5e:	f7ff fffe 	bl	0 <main>
    62:	4b29      	ldr	r3, [pc, #164]	@ (108 <main+0x108>)
    64:	f8d3 30e0 	ldr.w	r3, [r3, #224]	@ 0xe0
    68:	4a27      	ldr	r2, [pc, #156]	@ (108 <main+0x108>)
    6a:	f043 7300 	orr.w	r3, r3, #33554432	@ 0x2000000
    6e:	f8c2 30e0 	str.w	r3, [r2, #224]	@ 0xe0
    72:	4b25      	ldr	r3, [pc, #148]	@ (108 <main+0x108>)
    74:	f8d3 30e0 	ldr.w	r3, [r3, #224]	@ 0xe0
    78:	f003 7300 	and.w	r3, r3, #33554432	@ 0x2000000
    7c:	607b      	str	r3, [r7, #4]
    7e:	687b      	ldr	r3, [r7, #4]
    80:	2000      	movs	r0, #0
    82:	f7ff fffe 	bl	0 <HAL_HSEM_FastTake>
    86:	2100      	movs	r1, #0
    88:	2000      	movs	r0, #0
    8a:	f7ff fffe 	bl	0 <HAL_HSEM_Release>
    8e:	f64f 73ff 	movw	r3, #65535	@ 0xffff
    92:	63fb      	str	r3, [r7, #60]	@ 0x3c
    94:	bf00      	nop
    96:	4b1c      	ldr	r3, [pc, #112]	@ (108 <main+0x108>)
    98:	681b      	ldr	r3, [r3, #0]
    9a:	f403 4300 	and.w	r3, r3, #32768	@ 0x8000
    9e:	2b00      	cmp	r3, #0
    a0:	d104      	bne.n	ac <main+0xac>
    a2:	6bfb      	ldr	r3, [r7, #60]	@ 0x3c
    a4:	1e5a      	subs	r2, r3, #1
    a6:	63fa      	str	r2, [r7, #60]	@ 0x3c
    a8:	2b00      	cmp	r3, #0
    aa:	dcf4      	bgt.n	96 <main+0x96>
    ac:	6bfb      	ldr	r3, [r7, #60]	@ 0x3c
    ae:	2b00      	cmp	r3, #0
    b0:	da01      	bge.n	b6 <main+0xb6>
    b2:	f7ff fffe 	bl	0 <main>
    b6:	f7ff fffe 	bl	0 <main>
    ba:	f7ff fffe 	bl	0 <MX_USB_DEVICE_Init>
    be:	ed97 3b02 	vldr	d3, [r7, #8]
    c2:	ed97 4b04 	vldr	d4, [r7, #16]
    c6:	ed97 5b06 	vldr	d5, [r7, #24]
    ca:	ed97 2b08 	vldr	d2, [r7, #32]
    ce:	ed97 6b0a 	vldr	d6, [r7, #40]	@ 0x28
    d2:	ed97 7b0c 	vldr	d7, [r7, #48]	@ 0x30
    d6:	eeb0 0b42 	vmov.f64	d0, d2
    da:	eeb0 1b46 	vmov.f64	d1, d6
    de:	eeb0 2b47 	vmov.f64	d2, d7
    e2:	f7ff fffe 	bl	0 <main>
    e6:	eeb0 5b40 	vmov.f64	d5, d0
    ea:	eeb0 6b41 	vmov.f64	d6, d1
    ee:	eeb0 7b42 	vmov.f64	d7, d2
    f2:	ed87 5b08 	vstr	d5, [r7, #32]
    f6:	ed87 6b0a 	vstr	d6, [r7, #40]	@ 0x28
    fa:	ed87 7b0c 	vstr	d7, [r7, #48]	@ 0x30
    fe:	bf00      	nop
    100:	e7dd      	b.n	be <main+0xbe>
    102:	bf00      	nop
    104:	00000000 	.word	0x00000000
    108:	58024400 	.word	0x58024400
    ```

    we can see the relevant section of main, that implements the inlined version of vec3_add

    ```asm
    be:	ed97 3b02 	vldr	d3, [r7, #8]
    c2:	ed97 4b04 	vldr	d4, [r7, #16]
    c6:	ed97 5b06 	vldr	d5, [r7, #24]
    ca:	ed97 2b08 	vldr	d2, [r7, #32]
    ce:	ed97 6b0a 	vldr	d6, [r7, #40]	@ 0x28
    d2:	ed97 7b0c 	vldr	d7, [r7, #48]	@ 0x30
    d6:	eeb0 0b42 	vmov.f64	d0, d2
    da:	eeb0 1b46 	vmov.f64	d1, d6
    de:	eeb0 2b47 	vmov.f64	d2, d7
    e2:	f7ff fffe 	bl	0 <main>
    e6:	eeb0 5b40 	vmov.f64	d5, d0
    ea:	eeb0 6b41 	vmov.f64	d6, d1
    ee:	eeb0 7b42 	vmov.f64	d7, d2
    f2:	ed87 5b08 	vstr	d5, [r7, #32]
    f6:	ed87 6b0a 	vstr	d6, [r7, #40]	@ 0x28
    fa:	ed87 7b0c 	vstr	d7, [r7, #48]	@ 0x30
    fe:	bf00      	nop
    100:	e7dd      	b.n	be <main+0xbe>
    ```

    the inlined version is much shorter. (asserts notwithstanding)
    This, to me, is a good indicator of the performance of the inline static version of vec3 api.
    I think we can expect the API to take less cycles on average than the equivalent API expressed 
    using pointers. (pass-by-pointer). Though, I am interested in using the Data watchpoint and 
    Trace unit. It looks like this unit can be used to count execution cycles.
    We can use this to establish a lower bound on function execution time.
    We can use that lower bound to estimate how long tasks will take, and we can use that 
    to establish our metting of the real-time constraints of the system.
    This is particularly important for the CM4 core, which has the task of emitting motor signals
    at bounded time intervals.  

    And, if we consider the code size overhead. "Perhaps inlining the routines everywhere will 
    cause the program to become too large to fit every feature we need." 
    I would council that with "The inline is just a suggestion, the compiler is free to emit a 
    program global definition in vec3.c.obj for the linker to keep in the emitted '.elf'.
    additionally, if we observe the flags used by the build system for Release, we can see that 
    -Os is selected, not -O2. (see the toolchain file: "gcc-arm-none-eabi.cmake:36") so we are 
    optimizing for size over speed. Therefore I would expect the compiler to err on the side 
    of size over speed if inlining everywhere because an issue. And the size of the by-value 
    routine is simply smaller than the size of the by-pointer routine.
    (I imagine gcc has heuristics built in that consider overall code size when considering 
    inlining in an Os context or an O2 context?)"
 */

#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include <stdint.h>
#include <math.h>

#include "linear_algebra/scalar.h"

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
    Scalar x;
    Scalar y;
    Scalar z;
} Vec3;

// End Public Typedef

// Public Function Declarations

static inline Vec3 vec3_init(Scalar x, Scalar y, Scalar z) {
    return (Vec3){x, y, z};
}

static inline Vec3 vec3_splat(Scalar x) {
    return (Vec3){x, x, x};
}

static inline bool vec3_equal(Vec3 a, Vec3 b, Scalar near) {
    return scalar_equal(a.x, b.x, near)
        && scalar_equal(a.y, b.y, near)
        && scalar_equal(a.z, b.z, near);
}

static inline bool vec3_equal_scalar(Vec3 a, Scalar b, Scalar near) {
    return scalar_equal(a.x, b, near)
        && scalar_equal(a.y, b, near)
        && scalar_equal(a.z, b, near);
}

static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vec3 vec3_scale(Vec3 v, Scalar s) {
    return (Vec3){v.x * s, v.y * s, v.z * s};
}

static inline Scalar vec3_dot(Vec3 a, Vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3){(a.y * b.z) - (a.z * b.y),
                  (a.z * b.x) - (a.x * b.z),
                  (a.x * b.y) - (a.y * b.x)};
}

static inline Scalar vec3_length(Vec3 v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static inline Vec3 vec3_normalize(Vec3 v) {
    Scalar length = vec3_length(v);
    if (length > 0) {
        return (Vec3){v.x / length,
                      v.y / length,
                      v.z / length};
    } else {
        return (Vec3){0.0, 0.0, 0.0};
    }
}

// End Public Function Declarations

#endif // !MATH_VEC3_H
