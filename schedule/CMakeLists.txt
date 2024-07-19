project(schedule
        DESCRIPTION "C++ implementation of GTFS reader"
)

#set(CSV_PARSER_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/gtfs/strategies/csv_reader_fast/csv.h")
#
#if(MSVC)
#    set_source_files_properties(${GTFS_CALENDAR_READER_CSV_PARSER_CPP} PROPERTIES COMPILE_FLAGS "/wd2220 /wd4996 /wd4267 /wd4244")
#else()
#    set_source_files_properties(${CSV_PARSER_HEADER} PROPERTIES COMPILE_FLAGS "-Wno-error")
#endif()

add_executable(gtfsSubsetWriter
        src/gtfs/agencySubsetWriter/gtfsSubsetWriter.cpp
)

add_library(${PROJECT_NAME} SHARED)

find_path(P_RANAV_CSV2_INCLUDE_DIRS "csv2/mio.hpp")

target_sources(${PROJECT_NAME} PRIVATE
        src/DataReader.h
        src/gtfs/strategies/txt_reader/GtfsAgencyReader.cpp
        src/gtfs/strategies/txt_reader/GtfsAgencyReader.h
        src/utils/utils.h
        src/gtfs/strategies/txt_reader/GtfsCalendarReader.cpp
        src/gtfs/strategies/txt_reader/GtfsCalendarReader.h
        src/gtfs/strategies/txt_reader/GtfsCalendarDateReader.cpp
        src/gtfs/strategies/txt_reader/GtfsCalendarDateReader.h
        src/gtfs/strategies/txt_reader/GtfsRouteReader.cpp
        src/gtfs/strategies/txt_reader/GtfsRouteReader.h
        src/gtfs/strategies/txt_reader/GtfsStopReader.cpp
        src/gtfs/strategies/txt_reader/GtfsStopReader.h
        src/gtfs/strategies/txt_reader/GtfsStopTimeReader.cpp
        src/gtfs/strategies/txt_reader/GtfsStopTimeReader.h
        src/gtfs/strategies/txt_reader/GtfsTransferReader.cpp
        src/gtfs/strategies/txt_reader/GtfsTransferReader.h
        src/gtfs/strategies/txt_reader/GtfsTripReader.cpp
        src/gtfs/strategies/txt_reader/GtfsTripReader.h
        src/utils/scopedTimer.h
        src/gtfs/GtfsReader.cpp
        src/utils/DataContainer.h
        src/gtfs/TimetableManager.cpp
        src/Types.h
        src/utils/ServiceDayTime.cpp
        src/gtfs/GtfsTxtReaderStrategyFactory.cpp
        src/gtfs/GtfsTxtReaderStrategyFactory.h
        src/gtfs/strategies/csv_reader/GtfsAgencyReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsAgencyReaderCsv.h
        src/utils/DataTypeConverter.h
        src/gtfs/strategies/csv_reader/GtfsCalendarDateReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsCalendarDateReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsCsvHelpers.h
        src/gtfs/strategies/csv_reader/GtfsCalendarReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsCalendarReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsRouteReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsRouteReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsStopReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsStopReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsStopTimeReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsStopTimeReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsTripReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsTripReaderCsv.h
        src/gtfs/strategies/csv_reader/GtfsTransferReaderCsv.cpp
        src/gtfs/strategies/csv_reader/GtfsTransferReaderCsv.h
        src/gtfs/GtfsCsvReaderStrategyFactory.cpp
        src/gtfs/GtfsCsvReaderStrategyFactory.h
        src/gtfs/GtfsReaderStrategyFactory.cpp
        ${CSV_PARSER_HEADER}
        src/gtfs/strategies/csv_reader_fast/GtfsAgencyReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsAgencyReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsCalendarDateReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsCalendarDateReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsCalendarReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsCalendarReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsRouteReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsRouteReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsStopReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsStopReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsStopTimeReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsStopTimeReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsTransferReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsTransferReaderCsvParser.cpp
        src/gtfs/strategies/csv_reader_fast/GtfsTripReaderCsvParser.h
        src/gtfs/strategies/csv_reader_fast/GtfsTripReaderCsvParser.cpp
        src/gtfs/GtfsCsvParserReaderStrategyFactory.cpp
        src/gtfs/GtfsCsvParserReaderStrategyFactory.h
        src/gtfs/strategies/csv_reader_fast/csv_wrapper.h
        PUBLIC
        src/gtfs/GtfsData.h
        include/model/Agency.h
        include/model/Stop.h
        include/model/Route.h
        include/model/Trip.h
        include/model/StopTime.h
        include/model/Calendar.h
        include/model/CalendarDate.h
        include/model/Transfer.h
        include/GtfsReaderStrategyFactory.h
        include/IGtfsReaderStrategyFactory.h
        include/TimetableManager.h
        src/utils/ServiceDayTime.h
        include/GtfsReader.h
)


message("CU_LIST_DIR UO: ${CMAKE_CURRENT_LIST_DIR}")


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/geometry/include)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_include_directories(${PROJECT_NAME} PRIVATE ${P_RANAV_CSV2_INCLUDE_DIRS})

target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/logging/include)
target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/schedule/src)
target_include_directories(gtfsSubsetWriter PUBLIC ${CMAKE_SOURCE_DIR}/geometry/include)
target_include_directories(gtfsSubsetWriter PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        geometry
        spdlog::spdlog
)

target_link_libraries(gtfsSubsetWriter PRIVATE
        logging
        geometry
        spdlog::spdlog
        ${PROJECT_NAME}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GTFS_DLL_EXPORTS)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/schedule/src # specify path to include files during build
        ${CMAKE_SOURCE_DIR}/schedule/include
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME SCHEDULE_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

set(INCLUDE_INSTALL_DIR include)

install(FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
)

message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)

if (USE_ZURICH_GTFS)
    add_compile_definitions(-DOPEN_DATA_ZURICH)
endif ()