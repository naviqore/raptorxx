cmake_minimum_required(VERSION 3.28)
project(raptorxx VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

if (CMAKE_CXX_STANDARD LESS 23)
    message(FATAL_ERROR "At least C++23 required but have ${CMAKE_CXX_STANDARD}")
endif ()

if (MSVC)
    add_compile_options(/W4 /WX /std:c++latest)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c++23)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c++2b)
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with MSVC, GCC and Clang.")
else ()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with MSVC, GCC and Clang.")
endif ()

configure_file(
        "${PROJECT_SOURCE_DIR}/ProjectConfig.h.in"
        "${PROJECT_BINARY_DIR}/ProjectConfig.h"
)

set(BUILD_SHARED_LIBS ON)

set(CPM_DOWNLOAD_LOCATION "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")

#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
#        CACHE STRING "Vcpkg toolchain file")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/output/bin/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/output/lib/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/output/lib/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/output/bin/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/output/lib/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/output/lib/release)

add_subdirectory(interfaces)
add_subdirectory(logging)
add_subdirectory(gtfs)
add_subdirectory(gtfs/test)
add_subdirectory(gtfs/benchmarks)
add_subdirectory(raptor)