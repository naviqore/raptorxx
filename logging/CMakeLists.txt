cmake_minimum_required(VERSION 3.28)

project("logging")

set(SPDLOG_BUILD_SHARED OFF)

find_package(spdlog CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE
        src/LoggingPool.cpp
        src/LoggingPool.h
        include/LoggerBridge.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME LOGGER_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set(LIBRARY_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIBRARY_INSTALL_DIR})
install(FILES
        src/LoggingPool.h
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
)