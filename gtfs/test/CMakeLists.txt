cmake_minimum_required(VERSION 3.28)

project("test"
        DESCRIPTION "C++ implementation of GTFS reader"
)

enable_testing()

add_executable(${PROJECT_NAME}
        test_gtfs.cpp)

target_sources(${PROJECT_NAME} PRIVATE
        test_testGoogleFramework.cpp
        test_gtfsReader.cpp
        test_exploreGTest.cpp
        test_main.cpp
        )

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        gtfs
        logging
        spdlog::spdlog
)

set(BINARY ${PROJECT_NAME}_tests_test)
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtfs/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/src)

add_definitions(-DTEST_DATA_DIR="C:/Users/MichaelBrunner/Downloads/gtfs_fp2024_2024-05-27/")# "${CMAKE_SOURCE_DIR}/gtfs/test/test-data/"




