project("test"
        DESCRIPTION "C++ implementation of GTFS reader"
)

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
message(STATUS "Tests should be built in Debug mode")
endif ()

enable_testing()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        test_gtfsReader.cpp
        test_gtfs.cpp
        test_main.cpp
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
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/schedule/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/include)


message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}/schedule/test/test-data/")
add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/schedule/test/test-data/")
add_definitions(-DTEST_DATA_DIR_FULL_GTFS="C:/Users/MichaelBrunner/Downloads/gtfs_fp2024_2024-05-27/")




