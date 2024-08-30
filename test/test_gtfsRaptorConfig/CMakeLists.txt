project("gtfsRaptorConfig_tests"
        DESCRIPTION "gtfsRaptorConfig test"
)

add_executable(${PROJECT_NAME})
add_subdirectory(src)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        raptor
        schedule
        logging
        gtfsRaptorConfig
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include/config)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/include)

add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/test/test_schedule/test-data/")



