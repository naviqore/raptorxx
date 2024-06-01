cmake_minimum_required(VERSION 3.28)
project(benchmarks)

#include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
#CPMAddPackage("gh:google/googletest#main")
#CPMAddPackage("gh:google/benchmark#main")


file(GLOB CHAPTER_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${CHAPTER_SRC_FILES}
        lambda_vs_function.cpp)

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        GTest::gtest_main
        gtfs)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtfs/src)
#target_include_directories(${PROJECT_NAME} PRIVATE ${benchmark_SOURCE_DIR}/include ${benchmark_SOURCE_DIR})
#target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

