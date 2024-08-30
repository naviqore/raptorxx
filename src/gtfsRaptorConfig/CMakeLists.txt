project(gtfsRaptorConfig)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE
        src/GtfsToRaptorConverter.cpp
        src/RoutePartitioner.cpp
        src/TimetableManager.cpp
        src/SubRoute.cpp
        PUBLIC
        include/GtfsToRaptorConverter.h
        include/RoutePartitioner.h
        include/TimetableManager.h
        include/SubRoute.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/include) # check if public is needed
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        geometry
        schedule
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME GTFS_RAPTOR_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

set(INCLUDE_INSTALL_DIR include)

install(FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
        DESTINATION
        ${INCLUDE_INSTALL_DIR}
)

add_subdirectory(src/agencySubsetWriter)