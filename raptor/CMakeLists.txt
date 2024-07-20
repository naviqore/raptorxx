project("raptor"
        DESCRIPTION "C++ implementation of raptor algorithm"
)

add_library(${PROJECT_NAME} SHARED
        src/utils/RaptorData.cpp
        src/utils/RaptorData.h
        src/utils/RaptorDataBuilder.cpp
        src/utils/RaptorDataBuilder.h
        src/utils/RouteBuilder.cpp
        src/utils/RouteBuilder.h
        src/utils/OrderedHashMap.h
        include/RaptorAlgorithm.h
        include/QueryConfig.h
        src/RaptorRouter.cpp
        src/RaptorRouter.h
        src/Query.cpp
        src/Query.h
        src/StopLabelsAndTimes.cpp
        src/StopLabelsAndTimes.h
        src/FootpathRelaxer.cpp
        src/FootpathRelaxer.h
        src/RouteScanner.cpp
        src/RouteScanner.h)

target_sources(${PROJECT_NAME} PRIVATE
        src/Raptor.cpp
        src/Raptor.h
        src/Leg.h
        src/LegImpl.cpp
        src/LegImpl.h
        include/IRaptorAlgorithmStrategy.h
        include/IRaptorAlgorithmFactory.h
        src/RaptorAlgorithmFactory.cpp
        src/RaptorAlgorithmFactory.h
        src/strategies/RaptorStrategy.cpp
        src/strategies/RaptorStrategy.h
        include/raptorTypes.h
        include/IConnection.h
        include/IRaptor.h
        src/ConnectionImpl.cpp
        src/ConnectionImpl.h
        src/utils/Label.h
        src/utils/IndexedVisitedHashMap.cpp
        src/utils/IndexedVisitedHashMap.h
        src/utils/IndexedVisitedSet.cpp
        src/utils/IndexedVisitedSet.h
        src/strategies/StandardRaptorStrategy.cpp
        src/strategies/StandardRaptorStrategy.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/logging/include
        ${CMAKE_SOURCE_DIR}/logging/src
        ${CMAKE_SOURCE_DIR}/schedule/src
        ${CMAKE_SOURCE_DIR}/schedule/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

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
        src/Raptor.h
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h DESTINATION ${INCLUDE_INSTALL_DIR}
)