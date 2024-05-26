cmake_minimum_required(VERSION 3.28)

project("test"
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

add_executable(${PROJECT_NAME}
        test_addition.cpp
)

