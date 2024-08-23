project("raptor_tests"
        DESCRIPTION "Raptor test"
)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        test_routeBuilder.cpp
        test_raptorAlgorithm.cpp
        RaptorRouterTestBuilder.h
        RaptorRouterTestBuilder.cpp
)


find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main
        GTest::gtest
        spdlog::spdlog
        raptor
        schedule
        logging
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include/config)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)



