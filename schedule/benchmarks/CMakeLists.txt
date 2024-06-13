project(benchmarks)

file(GLOB CHAPTER_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${CHAPTER_SRC_FILES}
)

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtfs/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/src)

target_link_libraries(${PROJECT_NAME} PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        GTest::gtest_main
        schedule
)

target_compile_definitions(${PROJECT_NAME} PRIVATE gtfs_EXPORTS)

#target_include_directories(${PROJECT_NAME} PRIVATE ${benchmark_SOURCE_DIR}/include ${benchmark_SOURCE_DIR})
#target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_definitions(-DTEST_DATA_DIR="C:/Users/MichaelBrunner/Downloads/gtfs_fp2024_2024-05-27/") # "${CMAKE_SOURCE_DIR}/schedule/test/test-data/"


