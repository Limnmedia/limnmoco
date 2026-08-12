cmake_minimum_required(VERSION 3.16)

project(limnmoco LANGUAGES CXX)

option(LIMNMOCO_BUILD_HOST_TESTS "Build host-side test executables" ON)
option(LIMNMOCO_ENABLE_FIRMWARE_TARGETS
       "Expose Arduino CLI firmware build targets when arduino-cli is installed" ON)
set(LIMNMOCO_UPLOAD_PORT "" CACHE STRING
    "Serial port for the optional upload-m7 and upload-m4 targets")

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

  add_library(limnmoco_test_support INTERFACE)
  target_include_directories(limnmoco_test_support INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/support"
  )
  target_compile_definitions(limnmoco_test_support INTERFACE
    LIMNMOCO_TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
  )

  add_library(limnmoco_named_move_fixture STATIC
    tests/support/named_move_fixture.cpp
  )
  target_include_directories(limnmoco_named_move_fixture PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/support"
  )
  target_compile_features(limnmoco_named_move_fixture PUBLIC cxx_std_17)

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

  add_library(limnmoco_protocol STATIC
    common/LimnmocoProtocol/src/DmcFrame.cpp
    common/LimnmocoProtocol/src/VirtualPackets.cpp
  )
  target_include_directories(limnmoco_protocol PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/common/LimnmocoProtocol/src"
  )
  target_compile_features(limnmoco_protocol PUBLIC cxx_std_17)

  add_executable(limnmoco_ik_test
    tests/ik/limnmoco_ik.cpp
    tests/ik/aim_test.cpp
    tests/ik/boom_compensation_test.cpp
    tests/ik/camera_line_test.cpp
    tests/ik/compensation_fixture_test.cpp
    tests/ik/fk_ik_test.cpp
    tests/ik/kuper_coordinates_test.cpp
    tests/ik/named_virtual_move_test.cpp
  )
  target_link_libraries(limnmoco_ik_test PRIVATE
    limnmoco_ik limnmoco_named_move_fixture GTest::gtest_main
    limnmoco_test_support
  )

  add_executable(coordinated_trajectory_test
    tests/trajectory/coordinated_trajectory_test.cpp
    tests/trajectory/coordinated_handoff_test.cpp
  )
  target_link_libraries(coordinated_trajectory_test PRIVATE
    limnmoco_ik GTest::gtest_main limnmoco_test_support
  )

  # Retain a small direct integration check alongside the production suites.
  add_executable(googletest_smoke_test
    tests/support/googletest_smoke_test.cpp
  )
  target_link_libraries(googletest_smoke_test PRIVATE
    GTest::gtest_main limnmoco_test_support
  )

  add_executable(named_move_fixture_test
    tests/support/named_move_fixture_test.cpp
  )

  add_executable(dmc_frame_test tests/m7_protocol/dmc_frame_test.cpp)
  target_link_libraries(dmc_frame_test PRIVATE limnmoco_protocol GTest::gtest_main)
  add_executable(virtual_packets_test tests/m7_protocol/virtual_packets_test.cpp)
  target_link_libraries(virtual_packets_test PRIVATE limnmoco_protocol GTest::gtest_main)
  target_link_libraries(named_move_fixture_test PRIVATE
    limnmoco_named_move_fixture GTest::gtest_main limnmoco_test_support
  )

  if(BUILD_TESTING)
    include(GoogleTest)
    gtest_discover_tests(googletest_smoke_test)
    gtest_discover_tests(named_move_fixture_test)
    gtest_discover_tests(dmc_frame_test)
    gtest_discover_tests(virtual_packets_test)
    gtest_discover_tests(coordinated_trajectory_test)
    gtest_discover_tests(limnmoco_ik_test)
  endif()

  add_custom_target(host-tests
    COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure
    DEPENDS limnmoco_ik_test coordinated_trajectory_test googletest_smoke_test
            named_move_fixture_test
            dmc_frame_test
            virtual_packets_test
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

    if(LIMNMOCO_UPLOAD_PORT)
      add_custom_target(upload-m7
        COMMAND "${ARDUINO_CLI_EXECUTABLE}" compile --upload
          --port "${LIMNMOCO_UPLOAD_PORT}"
          --libraries "${CMAKE_CURRENT_SOURCE_DIR}/common"
          --fqbn "${LIMNMOCO_GIGA_FQBN},target_core=cm7"
          "${CMAKE_CURRENT_SOURCE_DIR}/m7"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Compiling and uploading Limnmoco M7 firmware through Arduino CLI"
      )

      add_custom_target(upload-m4
        COMMAND "${ARDUINO_CLI_EXECUTABLE}" compile --upload
          --port "${LIMNMOCO_UPLOAD_PORT}"
          --fqbn "${LIMNMOCO_GIGA_FQBN},target_core=cm4"
          "${CMAKE_CURRENT_SOURCE_DIR}/m4"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Compiling and uploading Limnmoco M4 firmware through Arduino CLI"
      )

      add_custom_target(upload-all
        COMMAND "${ARDUINO_CLI_EXECUTABLE}" compile --upload
          --port "${LIMNMOCO_UPLOAD_PORT}"
          --fqbn "${LIMNMOCO_GIGA_FQBN},target_core=cm4"
          "${CMAKE_CURRENT_SOURCE_DIR}/m4"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DEPENDS upload-m7
        USES_TERMINAL
        COMMENT "Uploading Limnmoco M7, then M4 firmware through Arduino CLI"
      )
    else()
      message(STATUS
        "Set LIMNMOCO_UPLOAD_PORT (for example -DLIMNMOCO_UPLOAD_PORT=/dev/ttyACM0) to enable upload-m7, upload-m4, and upload-all")
    endif()

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
