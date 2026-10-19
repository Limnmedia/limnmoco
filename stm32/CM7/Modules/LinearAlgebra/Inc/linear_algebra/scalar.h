// Copyright 2026 cade
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

#ifndef LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H
#define LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H

#include <math.h>

typedef double Scalar;

constexpr double SCALAR_CLOSE = (1e-9);

static inline bool scalar_equal(Scalar a, Scalar b, Scalar near) {
    Scalar delta = fabs(a - b);
    return delta <= near;
}

#endif // !LIMNMOCO_CM7_LINEAR_ALGEBRA_SCALAR_H