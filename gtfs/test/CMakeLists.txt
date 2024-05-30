cmake_minimum_required(VERSION 3.28)

project("test"
        DESCRIPTION "C++ implementation of GTFS reader"
)

include(${CPM_DOWNLOAD_LOCATION})
CPMAddPackage("gh:google/googletest#main")


add_executable(${PROJECT_NAME}
        test_addition.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtfs/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} gtfs)


