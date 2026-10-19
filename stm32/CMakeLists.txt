cmake_minimum_required(VERSION 3.22)
include(ExternalProject)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# In order to use CMake for cross-compiling
set(CMAKE_CXX_COMPILER_FORCED "true")
set(CMAKE_C_COMPILER_FORCED "true")

# Set the project name
set(CMAKE_PROJECT_NAME limnmoco)

# Core project settings
project(${CMAKE_PROJECT_NAME})

include(CTest)

# add test project
ExternalProject_Add(limnmoco_test
    BINARY_DIR                 ${CMAKE_SOURCE_DIR}/Test/build
    SOURCE_DIR                 ${PROJECT_SOURCE_DIR}/Test
    PREFIX                     Test 
    CONFIGURE_HANDLED_BY_BUILD true
    INSTALL_COMMAND            ""
    CMAKE_ARGS                 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_ALWAYS               true
)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_SOURCE_DIR}/Test/build")

# Include external project file
include("mx-generated.cmake")

if (BUILD_TESTING)
    add_test(
        NAME execute_unit_tests
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/Test/build --target test
    )

    set_tests_properties(execute_unit_tests PROPERTIES 
        DEPENDS limnmoco_test
    )
endif ()

