project(gtfs
        DESCRIPTION "C++ implementation of GTFS reader"
)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE
        src/TestObject.cpp
        src/TestObject.h
        src/DataReader.h
        src/GtfsReader.cpp
        src/GtfsReader.h
        src/strategies/GtfsReaderStrategy.cpp
        src/strategies/GtfsReaderStrategy.h
        src/model/Agency.h
        src/strategies/GtfsAgencyReader.cpp
        src/strategies/GtfsAgencyReader.h
        src/utils/utils.h
        src/GtfsData.h
        src/model/Stop.h
        src/model/Route.h
        src/model/Trip.h
        src/model/stopTime.h
        src/model/Calendar.h
        src/model/CalendarDate.h
        src/model/Transfer.h
        src/strategies/GtfsCalendarReader.cpp
        src/strategies/GtfsCalendarReader.h
        src/strategies/GtfsCalendarDateReader.cpp
        src/strategies/GtfsCalendarDateReader.h
)

# target_sources(${PROJECT_NAME}
#         PUBLIC
#         FILE_SET all_my_modules TYPE CXX_MODULES FILES
#         src/mod.test.cppm
# )

message("CU_LIST_DIR UO: ${CMAKE_CURRENT_LIST_DIR}")

#[[target_sources(${PROJECT_NAME}
        PRIVATE

)]]

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/logging/src)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        spdlog::spdlog
)

#target_link_libraries(${PROJECT_NAME} PRIVATE CommonInterfaces)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/CommonInterfaces/include)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_MACRO_NAME GTFS_API
        EXPORT_FILE_NAME ${PROJECT_NAME}_export.h
        STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set(LIBRARY_INSTALL_DIR lib)
set(INCLUDE_INSTALL_DIR include)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIBRARY_INSTALL_DIR})
install(FILES
        src/TestObject.h
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
)

message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)