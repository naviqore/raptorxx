project("schedule_tests"
        DESCRIPTION "C++ implementation of GTFS reader"
)

add_executable(${PROJECT_NAME})
add_subdirectory(src)

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        schedule
        logging
        spdlog::spdlog
)


# add_test(NAME GtfsCsvReaderStrategiesTest COMMAND ${PROJECT_NAME})
# add_test(NAME GTFS COMMAND ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include/model)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/geometry/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin/release
)


add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/test/test_schedule/test-data/")