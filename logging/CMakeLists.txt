cmake_minimum_required(VERSION 3.28)

project("logging")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage("gh:gabime/spdlog#v1.x")

add_library(${PROJECT_NAME} SHARED
        LoggingPool.cpp
        LoggingPool.h
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
                "LOGGER_API="
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
                "LOGGER_API="
        )
    endif()
endif()
