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

set(LIMNMOCO_VERSION_MAJOR 0)
set(LIMNMOCO_VERSION_MINOR 4)
set(LIMNMOCO_VERSION_PATCH 0)

execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE LIMNMOCO_GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(TIMESTAMP LIMNMOCO_BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

set(LIMNMOCO_BUILD_STRING "")
string(APPEND LIMNMOCO_BUILD_STRING
    "Limnmoco version ("
    ${LIMNMOCO_VERSION_MAJOR}
    "."
    ${LIMNMOCO_VERSION_MINOR}
    "."
    ${LIMNMOCO_VERSION_PATCH}
    ") built at ("
    ${LIMNMOCO_BUILD_TIMESTAMP}
    ") using commit ("
    ${LIMNMOCO_GIT_COMMIT}
    ")"
)

set(LIMNMOCO_MOTOR_COUNT 8)
set(LIMNMOCO_DMX_COUNT 0)
set(LIMNMOCO_GIO_OUT_COUNT 0)
set(LIMNMOCO_GIO_IN_COUNT 0)
set(LIMNMOCO_HW_LIMIT_COUNT 0)

set(LIMNMOCO_FRAME_COUNT 1440) # 1min of playback
# NOTE: 2min 2880
# 3min 4320
# 4min 5760
# 5min 7200
# 6min 8640
# 7min 10080
# 8min 11520
# 9min 12960
# 10min 14400
# and the dmc-lite sketch had 10000 frames. for ~7min of playback


set(LIMNMOCO_PROTOCOL_VERSION 2)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/config/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/config/config.h
)

set(LIBRARY_NAME limnmoco_config)
add_library(${LIBRARY_NAME} INTERFACE)

target_compile_definitions(${LIBRARY_NAME} INTERFACE
    ${LIMNMOCO_SYMBOLS}
)

target_compile_options(${LIBRARY_NAME} INTERFACE
    ${LIMNMOCO_WARNINGS}
    ${LIMNMOCO_CPU_FLAGS}
)

target_sources(${LIBRARY_NAME} INTERFACE FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc/config/config.h
)

set_property(GLOBAL APPEND PROPERTY LIMNMOCO_MODULES ${LIBRARY_NAME})
