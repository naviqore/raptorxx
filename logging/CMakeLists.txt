cmake_minimum_required(VERSION 3.28)

project("logging")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage("gh:gabime/spdlog#v1.x")


add_library(${PROJECT_NAME} SHARED
        LoggingPool.cpp
        LoggingPool.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

if(WIN32)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME}
                PRIVATE "LOGGER_API=__declspec(dllexport)"
                INTERFACE "LOGGER_API=__declspec(dllimport)"
        )
    else()
        target_compile_definitions(${PROJECT_NAME}
                PUBLIC "LOGGER_API"
        )
    endif()
else()
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${PROJECT_NAME}
                PRIVATE "LOGGER_API=__attribute__((visibility(\"default\")))"
                INTERFACE "LOGGER_API"
        )
    else()
        target_compile_definitions(${PROJECT_NAME}
                PUBLIC "LOGGER_API"
        )
    endif()
endif()
