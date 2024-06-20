project(geometry)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        src/spatial/CoordinateComponent.h
        src/spatial/index/K2DTree.h
        src/spatial/Coordinate.h
        src/utils/utils.h
        src/utils/usings.h)

# specify the include directory for the library and the users of the library
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)