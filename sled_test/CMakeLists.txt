cmake_minimum_required(VERSION 3.10)
project(sled_test)

add_subdirectory(../deps/sled_wrapper ${CMAKE_CURRENT_BINARY_DIR}/sled_wrapper_build)

add_executable(sled_test
    sled_test.cpp
)

target_link_libraries(
  sled_test
  sled_wrapper
)