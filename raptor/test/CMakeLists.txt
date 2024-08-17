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

set(BINARY ${PROJECT_NAME})
add_test(NAME RaptorAlgoTest COMMAND ${BINARY})
add_test(NAME RouteBuilderTest COMMAND ${BINARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/raptor/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/raptor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/raptor/include/config)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/schedule/include)



