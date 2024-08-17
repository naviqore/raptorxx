project("schedule_tests"
        DESCRIPTION "C++ implementation of GTFS reader"
)

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
message(STATUS "Tests should be built in Debug mode")
endif ()

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        test_gtfsReaderTxt.cpp
        test_main.cpp
        test_gtfsReaderCsv.cpp
)

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        schedule
        logging
        spdlog::spdlog
)

set(BINARY ${PROJECT_NAME}_tests_test)
add_test(NAME GtfsCsvReaderStrategiesTest COMMAND ${PROJECT_NAME})
add_test(NAME GTFS COMMAND ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin/release
)


message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}/schedule/test/test-data/")

add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/schedule/test/test-data/")






