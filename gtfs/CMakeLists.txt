cmake_minimum_required(VERSION 3.28)

project(gtfs
        DESCRIPTION "C++ implementation of GTFS reader"
)

if (MSVC)
    add_compile_options(/W4 /WX)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
else ()
    # neither MSVC nor GCC is being used
    # TODO add compiler specific options
    message(STATUS "Using another compiler")
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif ()

add_library(${PROJECT_NAME} SHARED
        src/TestObject.cpp
        src/TestObject.h
        src/DataReader.h
        src/GtfsReader.cpp
        src/GtfsReader.h
        src/GtfsReaderStrategy.cpp
        src/GtfsReaderStrategy.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/common/logging
        ${CMAKE_SOURCE_DIR}/common/logging/include
)

target_link_libraries(${PROJECT_NAME}
        logging
)

# target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_EXPORTS)
message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)