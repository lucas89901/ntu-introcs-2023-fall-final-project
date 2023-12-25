cmake_minimum_required(VERSION 3.20)
project(vmtranslator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif (POLICY CMP0135)
include(FetchContent)
FetchContent_Declare(
  absl
  URL https://github.com/abseil/abseil-cpp/archive/refs/heads/master.zip
)
set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_MakeAvailable(absl)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/master.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

# Targets

add_library(
  addressing
  src/addressing.cpp
)
target_link_libraries(
  addressing
  absl::log
  absl::str_format
)

add_library(
  commands
  src/commands.cpp
)
target_link_libraries(
  commands
  absl::strings
  absl::str_format
  addressing
)

add_library(
  parser
  src/parser.cpp
)
target_link_libraries(
  parser
  absl::check
  absl::log
  absl::strings
  absl::str_format
  addressing
  commands
)

add_executable(
  vmtranslator
  src/main.cpp
)

target_link_libraries(
  vmtranslator
  absl::check
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::log
  absl::strings
  absl::str_format
  commands
  parser
)

install(
  TARGETS vmtranslator
  DESTINATION ${CMAKE_SOURCE_DIR}
)

# Unit tests

add_executable(
  addressing_test
  src/addressing_test.cpp
)
target_link_libraries(
  addressing_test
  addressing
  absl::str_format
  GTest::gtest_main
)
gtest_discover_tests(addressing_test)

add_executable(
  commands_test
  src/commands_test.cpp
)
target_link_libraries(
  commands_test
  commands
  GTest::gtest_main
)
gtest_discover_tests(commands_test)

# Test programs

set(
  test_programs
  test_programs/StackArithmetic/SimpleAdd/
  test_programs/StackArithmetic/StackTest/
  test_programs/MemoryAccess/BasicTest/
  test_programs/MemoryAccess/PointerTest/
  test_programs/MemoryAccess/StaticTest/
  test_programs/ProgramFlow/BasicLoop/
  test_programs/ProgramFlow/FibonacciSeries/
  test_programs/FunctionCalls/SimpleFunction/
)

set(
  full_test_programs
  test_programs/FunctionCalls/NestedCall/
  test_programs/FunctionCalls/FibonacciElement/
  test_programs/FunctionCalls/StaticsTest/
)

if(WIN32)
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.bat)
else()
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.sh)
endif()

foreach(program ${test_programs})
  cmake_path(REMOVE_FILENAME program)
  message(STATUS "Adding test VM program: " ${program})

  cmake_path(GET program PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME basename)
  message(DEBUG "basename: " ${basename})

  file(
    COPY ${program}
    DESTINATION test_programs/${basename}/
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  add_test(
    NAME
      "Translation: ${program}"
    COMMAND
      vmtranslator test_programs/${basename}/${basename}.vm
  )
  add_test(
    NAME
      "Comparison: ${program}"
    COMMAND
      ${CPUEmulator} test_programs/${basename}/${basename}.tst
  )
  set_tests_properties(
    "Comparison: ${program}"
    PROPERTIES
      DEPENDS "Translation: ${program}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()


foreach(program ${full_test_programs})
  cmake_path(REMOVE_FILENAME program)
  message(STATUS "Adding test VM program: " ${program})

  cmake_path(GET program PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME basename)
  message(DEBUG "basename: " ${basename})

  file(
    COPY ${program}
    DESTINATION test_programs/${basename}/
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  add_test(
    NAME
      "Translation: ${program}"
    COMMAND
      vmtranslator test_programs/${basename}/
  )
  add_test(
    NAME
      "Comparison: ${program}"
    COMMAND
      ${CPUEmulator} test_programs/${basename}/${basename}.tst
  )
  set_tests_properties(
    "Comparison: ${program}"
    PROPERTIES
      DEPENDS "Translation: ${program}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()
