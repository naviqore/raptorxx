project("geometry_tests"
        DESCRIPTION "Tests for the geometry library"
)


include(CTest)
enable_testing()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        test_kdTree.cpp
)


find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        spdlog::spdlog
        geometry
        logging
)

set(BINARY ${PROJECT_NAME})

add_test(NAME geomety COMMAND ${BINARY}_test)
add_test(NAME TestK2dTree COMMAND ${BINARY}_test)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/include)





