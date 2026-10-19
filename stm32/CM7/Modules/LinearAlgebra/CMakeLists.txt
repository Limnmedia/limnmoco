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

# We are defining a base library for math functions,
# so it should not be tied to any specific target,
# however, it must be built such that it can be linked 
# into the larger project. And we want to make sure that 
# the library is built with the same compiler flags as the 
# main project. so we know gcc is emitting the fpu instructions
# for math acceleration.

enable_language(C ASM)

set(LIBRARY_NAME limnmoco_linear_algebra)
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
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/Inc/linear_algebra/vec3.h
)

target_sources(${LIBRARY_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/linear_algebra/vec3.c
)

set_property(GLOBAL APPEND PROPERTY LIMNMOCO_MODULES ${LIBRARY_NAME})
