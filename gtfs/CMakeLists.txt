cmake_minimum_required(VERSION 3.28)

project("gtfs"
        DESCRIPTION "C++ implementation of GTFS reader"
)

if (MSVC)
    add_compile_options(/W4 /WX)
elseif (GCC)
    add_compile_options(-Wall -Wextra -pedantic -Werror)
else ()
    # neither MSVC nor GCC is being used
    # TODO add compiler specific options
    message(using another compiler)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif ()


include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
)
message(STATUS "Fetching spdlog...")
message(STATUS "spdlog_SOURCE_DIR: ${spdlog_SOURCE_DIR}")
FetchContent_MakeAvailable(spdlog)

list(
        APPEND
        CMAKE_MODULE_PATH "${spdlog_SOURCE_DIR}/include"
)

if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        spdlog::spdlog
)
