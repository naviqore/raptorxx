project(geometry)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        include/spatial/CoordinateComponent.h
        include/spatial/index/K2DTree.h
        include/spatial/Coordinate.h
        include/utils/utils.h
        include/utils/usings.h)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/spatial/index)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/spatial)
target_include_directories(${PROJECT_NAME} INTERFACE include/utils)