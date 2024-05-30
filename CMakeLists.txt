cmake_minimum_required(VERSION 3.28)
project(raptorxx VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_STANDARD LESS 23)
    message(FATAL_ERROR "At least C++23 required but have ${CMAKE_CXX_STANDARD}")
endif ()

if(MSVC)
    add_compile_options(/W4 /WX /std:c++latest)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c++23)
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with MSVC, GCC and Clang.")
endif()

configure_file (
        "${PROJECT_SOURCE_DIR}/ProjectConfig.h.in"
        "${PROJECT_BINARY_DIR}/ProjectConfig.h"
)



add_subdirectory(logging)
add_subdirectory(gtfs)
add_subdirectory(gtfs/test)
add_subdirectory(gtfs/benchmarks)
add_subdirectory(raptor)