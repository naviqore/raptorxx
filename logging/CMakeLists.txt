cmake_minimum_required(VERSION 3.28)

project("logging")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage("gh:gabime/spdlog#v1.x")


add_library(${PROJECT_NAME}
        LoggingPool.cpp
        LoggingPool.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
