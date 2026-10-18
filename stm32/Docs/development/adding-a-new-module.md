<!--
 Copyright 2026 Cade Weinberg
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     https://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-->

# Adding a New Module

When adding a new module to either the CM4, CM7 projects, or the Common libraries subfolder
a particular pattern has emerged for ease of integration with the cmake build system.

consider adding a new module "Foo" to the project.

1. Select the "Modules" subdirectory of the chosen component:
    - `CM4/Modules/Foo`
    - `CM7/Modules/Foo`
    - `Common/Modules/Foo`
2. Append the appropriate `add_subdirectory(...)` command to the 'CMakeLists.txt' file within the 
   Modules subdirectory:
    - for instance, within 'CM4/Modules/CMakeLists.txt' append `add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Foo)`
3. Create the Modules 'CMakeLists.txt' file to orchestrate it's build, the pattern is generally:
    ```CMake
    cmake_minimum_required(VERSION 3.22)

    enable_language(C ASM)

    set(LIBRARY_NAME limnmoco_foo)
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
            ${CMAKE_CURRENT_SOURCE_DIR}/Inc/foo/...
            ...
    )

    target_sources(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/...
        ...
    )

    set_property(GLOBAL APPEND PROPERTY LIMNMOCO_MODULES ${LIBRARY_NAME})
    ```
    - ${LIMNMOCO_SYMBOLS} is defined with either CORE_CM4 or CORE_CM7 or is empty
        depending on the compilation context. CM4, CM7, ExternalProjects, or the Test suite.
    - ${LIMNMOCO_WARNINGS} contains the warnings that shall be enabled project wide
    - ${LIMNMOCO_CPU_FLAGS} is defined with the STM_CPU_FLAGS from either the CM4 or 
        CM7 ExternalProjects, or empty for the Test suite
    - adding the headers to the FILE_SET allows code which desires to link with the new module
        to add it's include path to the build commands of the code. allowing the modules header
        files to be visible in the client code.
    - adding the library to the global property LIMNMOCO_MODULES allows the CM4, CM7, and test suites
        to be configured dynamically, by having to modify less files. and avoiding the brittle name
        link between two files.
4. Select the Test "Modules" subdirectory for the test suite for the new module:
    - `Test/CM4/Modules/Foo`
    - `Test/CM7/Modules/Foo`
    - `Test/Common/Modules/Foo`
5. Append the appropriate `add_subdirectory(...)` command to the CMakeLists.txt in the
    Modules Test suite subdirectory
    - for instance, within 'Test/CM4/Modules/CMakeLists.txt' append `add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Foo)`
6. Create the Test Suites CMakeLists.txt for the new Module, the pattern is very similar:
    ```cmake
    cmake_minimum_required(VERSION 3.22)

    enable_language(C ASM)

    set(LIBRARY_NAME limnmoco_foo_tests)
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
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/Inc/test/...
        ...
    )

    target_sources(${LIBRARY_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/test/...
        ...
    )

    set_property(GLOBAL APPEND PROPERTY LIMNMOCO_TEST_SUITES ${LIBRARY_NAME})
    ```
    - the major difference is that the test suite shall be added to the LIMNMOCO_TEST_SUITES global property.



