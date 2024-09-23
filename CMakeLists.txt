cmake_minimum_required(VERSION 3.25.1)
project(raptorxx VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_STANDARD LESS 23)
    message(FATAL_ERROR "At least C++23 required but have ${CMAKE_CXX_STANDARD}")
endif ()

if (MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/W3 /WX /wd4251 /experimental:module /std:c++latest /RTC1)
    else ()
        add_compile_options(/W3 /WX /wd4251 /experimental:module /std:c++latest /O2)
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -pedantic -std=c++23) # -Werror => this is too strict for now - causes break when building CSV library
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(-Wall -Wextra -pedantic -std=c++2b) #  -Werror => this is too strict for now - causes break when building CSV library
else ()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with MSVC, GCC and Clang.")
endif ()

configure_file(
        "src/ProjectConfig.h.in"
        "${PROJECT_BINARY_DIR}/ProjectConfig.h"
)

set(BUILD_SHARED_LIBS ON)

set(CPM_DOWNLOAD_LOCATION "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")

# Download GTFS Switzerland
include(${CMAKE_SOURCE_DIR}/CMake/DownloadAndUnzip.cmake)
set(ZIP_URL "https://opentransportdata.swiss/en/dataset/timetable-2024-gtfs2020/permalink/gtfs_fp2024_2024-09-23.zip")
set(UNZIP_DIR "${CMAKE_SOURCE_DIR}/resources/gtfs_switzerland")

download_and_unzip(${ZIP_URL} ${UNZIP_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)

# SOURCE
add_subdirectory(src/logging)
add_subdirectory(src/geometry)
add_subdirectory(src/schedule)
add_subdirectory(src/gtfsRaptorConfig)
add_subdirectory(src/raptor)
# TESTS
add_subdirectory(test/test_raptor)
add_subdirectory(test/test_geometry)
add_subdirectory(test/test_schedule)
add_subdirectory(test/test_gtfsRaptorConfig)
# BECHMARKS
add_subdirectory(benchmark/benchmark_schedule)
add_subdirectory(benchmark/benchmark_gtfsRaptor)

include(CTest)
enable_testing()
