project(CommonInterfaces)

add_library(CommonInterfaces INTERFACE
        include/ITestInterface.h)

target_include_directories(CommonInterfaces INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS CommonInterfaces EXPORT CommonInterfacesConfig)
install(DIRECTORY include/ DESTINATION include)
