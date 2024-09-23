project(benchmark_gtfsRaptor)

add_executable(${PROJECT_NAME}
        gtfsRaptor_benchmark.cpp
)

add_executable(SimpleBenchmark
        SimpleRaptorBenchmark.cpp
)

find_package(benchmark CONFIG REQUIRED)

set(EXECUTABLES
        ${PROJECT_NAME}
        SimpleBenchmark
)

foreach(EXECUTABLE IN LISTS EXECUTABLES)
    target_include_directories(${EXECUTABLE} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_link_libraries(${EXECUTABLE} PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
            raptor
            schedule
            logging
            gtfsRaptorConfig
    )

    target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
    target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include)
    target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include/config)
    target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)
    target_include_directories(${EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/include)
endforeach()

add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/resources/gtfs_switzerland/")





