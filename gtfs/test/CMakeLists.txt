cmake_minimum_required(VERSION 3.28)

project("test"
        DESCRIPTION "C++ implementation of GTFS reader"
)

enable_testing()

add_executable(${PROJECT_NAME}
        test_addition.cpp
)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        gtfs
)


add_test(AllTestsInMain main)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtfs/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})



