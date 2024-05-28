cmake_minimum_required(VERSION 3.28)

project("raptor"
        DESCRIPTION "C++ implementation of raptor algorithm"
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

include_directories(${CMAKE_SOURCE_DIR}/common/logging/include)

add_library(${PROJECT_NAME}
        src/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/common/logging
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
)
