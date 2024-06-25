project("raptor"
        DESCRIPTION "C++ implementation of raptor algorithm"
)

add_library(${PROJECT_NAME}
        src/main.cpp
        src/Raptor.cpp
        src/Raptor.h
        src/Leg.h
        src/LegImpl.cpp
        src/LegImpl.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/logging/include
        ${CMAKE_SOURCE_DIR}/logging
        ${CMAKE_SOURCE_DIR}/schedule/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        logging
        schedule
)
