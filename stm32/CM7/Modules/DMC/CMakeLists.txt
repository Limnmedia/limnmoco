# Copyright (C) 2026 Cade Weinberg
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

enable_language(C ASM)

set(LIBRARY_NAME limnmoco_dmc)
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
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/dmc/protocol.h
)

target_sources(${LIBRARY_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/dmc/protocol.c
)


