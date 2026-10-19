# Copyright 2026 Cade Weinberg
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     https://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.22)

enable_language(C ASM)

set(LIBRARY_NAME limnmoco_linear_algebra_tests)
add_library(${LIBRARY_NAME})

target_compile_definitions(${LIBRARY_NAME} PRIVATE
    ${LIMNMOCO_SYMBOLS}
)

target_compile_options(${LIBRARY_NAME} PRIVATE
    ${LIMNMOCO_WARNINGS}
    ${LIMNMOCO_CPU_FLAGS}
)

target_sources(${LIBRARY_NAME} PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/Inc/test/linear_algebra/vec3.h
)

target_sources(${LIBRARY_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/test/linear_algebra/vec3.c
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
    munit
    limnmoco_linear_algebra
)

set_property(GLOBAL APPEND PROPERTY LIMNMOCO_TEST_SUITES ${LIBRARY_NAME})


