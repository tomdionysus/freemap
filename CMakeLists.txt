cmake_minimum_required(VERSION 3.14)
project(freemap)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(
  freemap
  src/freemap.c
)

enable_testing()

add_executable(
  freemap_test
  src/freemap_test.cc
)
target_link_libraries(
  freemap_test
  GTest::gtest_main
  freemap
)

include(GoogleTest)
gtest_discover_tests(freemap_test)

include_directories(
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_BINARY_DIR}/lib
)
