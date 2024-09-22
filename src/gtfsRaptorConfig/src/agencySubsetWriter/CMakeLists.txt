add_executable(gtfsSubsetWriter
        gtfsSubsetWriter.cpp
)

target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/src/logging/include)
target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/src)
target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/src/gtfsRaptorConfig/include)
target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/src/raptor/include)
target_include_directories(gtfsSubsetWriter PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_include_directories(gtfsSubsetWriter PRIVATE ${CMAKE_SOURCE_DIR}/src/schedule/3rdParty)

target_link_libraries(gtfsSubsetWriter PRIVATE
        logging
        geometry
        schedule
        ${PROJECT_NAME}
)
