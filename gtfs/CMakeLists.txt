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

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/common/logging
        ${CMAKE_SOURCE_DIR}/common/logging/include
)

target_link_libraries(${PROJECT_NAME}
        logging
)

# target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_EXPORTS)
message(BINARY_DIR: ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test)