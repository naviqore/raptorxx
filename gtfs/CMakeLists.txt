project(gtfs
        DESCRIPTION "C++ implementation of GTFS reader"
)

add_library(${PROJECT_NAME} SHARED
        src/TestObject.cpp
        src/TestObject.h
        src/DataReader.h
        src/GtfsReader.cpp
        src/GtfsReader.h
        src/GtfsReaderStrategy.cpp
        src/GtfsReaderStrategy.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging)

target_link_libraries(${PROJECT_NAME}
        logging
        spdlog::spdlog
)

if(WIN32)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME}
                PRIVATE "GTFS_API=__declspec(dllexport)"
                INTERFACE "GTFS_API=__declspec(dllimport)"
                "LOGGER_API=__declspec(dllimport)"
        )
    else()
        target_compile_definitions(${PROJECT_NAME}
                PUBLIC "GTFS_API"
                "LOGGER_API"
        )
    endif()
else()
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME}
                PRIVATE "GTFS_API=__attribute__((visibility(\"default\")))"
                INTERFACE "GTFS_API"
                "LOGGER_API=__attribute__((visibility(\"default\")))"
        )
    else()
        target_compile_definitions(${PROJECT_NAME}
                PUBLIC "GTFS_API"
                "LOGGER_API"
        )
    endif()
endif()


# target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_EXPORTS)
message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)