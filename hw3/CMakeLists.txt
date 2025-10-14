cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")
project(hw3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  nth_power_test
  nth_power.cpp
)
target_link_libraries(
  nth_power_test
  GTest::gtest_main
)
target_compile_options(
  nth_power_test
  PRIVATE
  -Wall
  -Wextra
  -Wpedantic
)

include(GoogleTest)
gtest_discover_tests(nth_power_test)

