project("raptor_test"
        DESCRIPTION "Raptor test"
)


enable_testing()

add_executable(${PROJECT_NAME}
        test_routeBuilder.cpp
        test_raptorDataBuilder.cpp
        test_raptorAlgorithm.cpp
        RaptorRouterTestBuilder.h)

target_sources(${PROJECT_NAME} PRIVATE
        test_raptor.cpp
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
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/raptor/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/raptor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/schedule/include)

 add_definitions(-DTEST_DATA_DIRRECTORY="C:/Users/MichaelBrunner/Downloads/gtfs_fp2024_2024-05-27/Verkehrsbetriebe_der_Stadt_St_Gallen/")

# if(USE_ZURICH_GTFS)
#    
#     # add_definitions(-DTEST_DATA_DIR="C:/Users/MichaelBrunner/Downloads/2024_google_transit/")
#     # add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/raptor/test/sample-feed/")
# else ()
#     # add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/schedule/test/test-data/")
#     add_definitions(-DTEST_DATA_DIR="C:/Users/MichaelBrunner/Downloads/gtfs_fp2024_2024-05-27/Verkehrsbetriebe_der_Stadt_St_Gallen/")
# endif()





