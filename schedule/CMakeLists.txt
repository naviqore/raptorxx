project(schedule
        DESCRIPTION "C++ implementation of GTFS reader"
)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE
        src/DataReader.h
        src/gtfs/GtfsReader.h
        src/model/Agency.h
        src/gtfs/strategies/GtfsAgencyReader.cpp
        src/gtfs/strategies/GtfsAgencyReader.h
        src/utils/utils.h
        src/gtfs/GtfsData.h
        src/model/Stop.h
        src/model/Route.h
        src/model/Trip.h
        src/model/StopTime.h
        src/model/Calendar.h
        src/model/CalendarDate.h
        src/model/Transfer.h
        src/gtfs/strategies/GtfsCalendarReader.cpp
        src/gtfs/strategies/GtfsCalendarReader.h
        src/gtfs/strategies/GtfsCalendarDateReader.cpp
        src/gtfs/strategies/GtfsCalendarDateReader.h
        src/gtfs/strategies/GtfsRouteReader.cpp
        src/gtfs/strategies/GtfsRouteReader.h
        src/gtfs/strategies/GtfsStopReader.cpp
        src/gtfs/strategies/GtfsStopReader.h
        src/gtfs/strategies/GtfsStopTimeReader.cpp
        src/gtfs/strategies/GtfsStopTimeReader.h
        src/gtfs/strategies/GtfsTransferReader.cpp
        src/gtfs/strategies/GtfsTransferReader.h
        src/gtfs/strategies/GtfsTripReader.cpp
        src/gtfs/strategies/GtfsTripReader.h
        src/utils/scopedTimer.h
        src/gtfs/GtfsReader.cpp
        src/utils/DataContainer.h
        src/gtfs/RelationManager.cpp
        src/gtfs/RelationManager.h
        src/Types.h
        src/gtfs/GtfsSchedule.cpp
        src/gtfs/GtfsSchedule.h
        src/utils/ServiceDayTime.cpp
        src/utils/ServiceDayTime.h
)


message("CU_LIST_DIR UO: ${CMAKE_CURRENT_LIST_DIR}")


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/geometry/src)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)


find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        geometry
        spdlog::spdlog
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GTFS_DLL_EXPORTS)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/schedule/src # specify path to include files during build
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME GTFS_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

set(LIBRARY_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIBRARY_INSTALL_DIR})
install(FILES
        src/TestObject.h
        src/gtfs/GtfsReader.h
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
)

message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)