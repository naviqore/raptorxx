project("raptor"
        DESCRIPTION "C++ implementation of raptor algorithm"
)

add_library(${PROJECT_NAME} SHARED)

add_subdirectory(include)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        include
        src
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src/logging/include
        ${CMAKE_SOURCE_DIR}/src/logging/src
        ${CMAKE_SOURCE_DIR}/src/schedule/src
        ${CMAKE_SOURCE_DIR}/src/schedule/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/schedule/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/schedule/include/config
        ${CMAKE_CURRENT_SOURCE_DIR}/src/schedule/src/data
)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        schedule
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME RAPTOR_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

set(INCLUDE_INSTALL_DIR include)

install(FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
        DESTINATION
        ${INCLUDE_INSTALL_DIR})