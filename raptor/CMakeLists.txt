cmake_minimum_required(VERSION 3.28)

project("raptor"
        DESCRIPTION "C++ implementation of raptor algorithm"
)

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
