project(benchmark_schedule)

file(GLOB CHAPTER_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(${PROJECT_NAME} ${CHAPTER_SRC_FILES}
)

find_package(GTest CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include/model)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/geometry/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        GTest::gtest_main
        schedule
)

target_compile_definitions(${PROJECT_NAME} PRIVATE gtfs_EXPORTS)

add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/resources/gtfs_switzerland/")





