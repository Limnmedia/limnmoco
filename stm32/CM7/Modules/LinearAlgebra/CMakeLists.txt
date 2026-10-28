# Copyright (C) 2026 cade
# 
# This file is part of Limnmoco.
# 
# Limnmoco is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Limnmoco is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Limnmoco.  If not, see <https://www.gnu.org/licenses/>.



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
    FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/linear_algebra/vec3.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/linear_algebra/scalar.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/linear_algebra/mat3x3.h
)

target_sources(${LIBRARY_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/linear_algebra/vec3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/linear_algebra/scalar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/linear_algebra/mat3x3.c
)

set_property(GLOBAL APPEND PROPERTY LIMNMOCO_MODULES ${LIBRARY_NAME})
