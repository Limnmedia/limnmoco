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

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(LIMNMOCO_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wformat=2
)

set(LIMNMOCO_CPU_FLAGS
    # we don't define CPU flags for the native build.
    # as these tests are run on the host machine.
)

set(LIMNMOCO_SYMBOLS
    # we don't define symbols for the native build.
    # as these tests are run on the host machine.
)

set(CMAKE_PROJECT_NAME limnmoco_test)
project(${CMAKE_PROJECT_NAME})

set_property(GLOBAL PROPERTY LIMNMOCO_MODULES "")
set_property(GLOBAL PROPERTY LIMNMOCO_TEST_SUITES "")

add_executable(${CMAKE_PROJECT_NAME})

# add external modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../External
    ${CMAKE_CURRENT_BINARY_DIR}/External
)

# add common modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
    ${CMAKE_CURRENT_BINARY_DIR}/Common
)

# add CM4 modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../CM4/Modules
    ${CMAKE_CURRENT_BINARY_DIR}/CM4/Modules
)

# add CM7 modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../CM7/Modules
    ${CMAKE_CURRENT_BINARY_DIR}/CM7/Modules
)

# add Common test modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/Modules
    ${CMAKE_CURRENT_BINARY_DIR}/Test/Common/Modules
)

# add CM4 test modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/CM4/Modules
    ${CMAKE_CURRENT_BINARY_DIR}/Test/CM4/Modules
)

# add CM7 test modules
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/CM7/Modules
    ${CMAKE_CURRENT_BINARY_DIR}/Test/CM7/Modules
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${LIMNMOCO_SYMBOLS}
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${LIMNMOCO_WARNINGS}
    ${LIMNMOCO_CPU_FLAGS}
)

enable_language(C ASM)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE

)

get_property(LIMNMOCO_MODULE_LIBS GLOBAL PROPERTY LIMNMOCO_MODULES)
get_property(LIMNMOCO_TEST_SUITE_LIBS GLOBAL PROPERTY LIMNMOCO_TEST_SUITES)

target_link_libraries(${CMAKE_PROJECT_NAME}
    # link to all modules
    ${LIMNMOCO_MODULE_LIBS}

    # link to all test suites
    ${LIMNMOCO_TEST_SUITE_LIBS}
)

