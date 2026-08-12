cmake_minimum_required(VERSION 3.16)

project(limnmoco LANGUAGES CXX)

option(LIMNMOCO_BUILD_HOST_TESTS "Build host-side test executables" ON)
option(LIMNMOCO_ENABLE_FIRMWARE_TARGETS
       "Expose Arduino CLI firmware build targets when arduino-cli is installed" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(LIMNMOCO_BUILD_HOST_TESTS)
  include(CTest)
  include(FetchContent)

  # Keep the test framework outside firmware builds.  The release tag makes
  # host-test configuration reproducible while still allowing FetchContent to
  # use an ordinary Git cache/build directory.
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(googletest)

  add_library(limnmoco_ik STATIC
    common/LimnmocoIK/src/AimAwareTarget.cpp
    common/LimnmocoIK/src/AimGeometry.cpp
    common/LimnmocoIK/src/AimPointProtocol.cpp
    common/LimnmocoIK/src/BoomCompensation.cpp
    common/LimnmocoIK/src/CameraLine.cpp
    common/LimnmocoIK/src/CameraLineTarget.cpp
    common/LimnmocoIK/src/CoordinatedTrajectory.cpp
    common/LimnmocoIK/src/KuperTrackConvention.cpp
    common/LimnmocoIK/src/LimnmocoIK.cpp
    common/LimnmocoIK/src/Mat3.cpp
    common/LimnmocoIK/src/Vec3.cpp
  )
  target_include_directories(limnmoco_ik PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/common/LimnmocoIK/src"
  )
  target_compile_features(limnmoco_ik PUBLIC cxx_std_17)

  add_executable(limnmoco_ik_test
    ik_test/main.cpp
    ik_test/limnmoco_ik.cpp
  )
  target_link_libraries(limnmoco_ik_test PRIVATE limnmoco_ik)

  add_executable(coordinated_trajectory_test
    ik_test/coordinated_trajectory_test.cpp
  )
  target_link_libraries(coordinated_trajectory_test PRIVATE limnmoco_ik)

  # This is intentionally a small integration check. Existing tests are
  # migrated incrementally, rather than mixed into the dependency commit.
  add_executable(googletest_smoke_test
    ik_test/googletest_smoke_test.cpp
  )
  target_link_libraries(googletest_smoke_test PRIVATE GTest::gtest_main)

  if(BUILD_TESTING)
    add_test(NAME limnmoco_ik_test COMMAND limnmoco_ik_test)
    set_tests_properties(limnmoco_ik_test PROPERTIES
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ik_test"
    )
    add_test(NAME coordinated_trajectory_test COMMAND coordinated_trajectory_test)
    include(GoogleTest)
    gtest_discover_tests(googletest_smoke_test)
  endif()

  add_custom_target(host-tests
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    DEPENDS limnmoco_ik_test coordinated_trajectory_test googletest_smoke_test
    USES_TERMINAL
    COMMENT "Running Limnmoco host tests"
  )
endif()

if(LIMNMOCO_ENABLE_FIRMWARE_TARGETS)
  find_program(ARDUINO_CLI_EXECUTABLE NAMES arduino-cli)
  if(ARDUINO_CLI_EXECUTABLE)
    set(LIMNMOCO_GIGA_FQBN "arduino:mbed_giga:giga:split=75_25")

    add_custom_target(firmware-m7
      COMMAND "${ARDUINO_CLI_EXECUTABLE}" compile
        --libraries "${CMAKE_CURRENT_SOURCE_DIR}/common"
        --fqbn "${LIMNMOCO_GIGA_FQBN},target_core=cm7"
        "${CMAKE_CURRENT_SOURCE_DIR}/m7"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      USES_TERMINAL
      COMMENT "Compiling Limnmoco M7 firmware through Arduino CLI"
    )

    add_custom_target(firmware-m4
      COMMAND "${ARDUINO_CLI_EXECUTABLE}" compile
        --fqbn "${LIMNMOCO_GIGA_FQBN},target_core=cm4"
        "${CMAKE_CURRENT_SOURCE_DIR}/m4"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      USES_TERMINAL
      COMMENT "Compiling Limnmoco M4 firmware through Arduino CLI"
    )

    add_custom_target(verify)
    if(TARGET host-tests)
      add_dependencies(verify host-tests)
    endif()
    add_dependencies(verify firmware-m7 firmware-m4)
  else()
    message(STATUS
      "arduino-cli was not found; firmware-m7, firmware-m4, and verify targets are unavailable")
  endif()
endif()
