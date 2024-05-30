cmake_minimum_required(VERSION 3.28)

project("logging")

if (MSVC)
    add_compile_options(/W3)
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

add_library(${PROJECT_NAME}
        LoggingPool.cpp
        LoggingPool.h
)

include(../cmake/CPM.cmake)
CPMAddPackage("gh:gabime/spdlog#v1.x")

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
