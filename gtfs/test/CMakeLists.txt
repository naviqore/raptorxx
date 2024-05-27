cmake_minimum_required(VERSION 3.28)

project("test"
        DESCRIPTION "C++ implementation of GTFS reader"
)

if (MSVC)
    add_compile_options(/W4 /WX)
elseif (GCC)
    add_compile_options(-Wall -Wextra -pedantic -Werror)
else ()
    # neither MSVC nor GCC is being used
    # TODO add compiler specific options
    message(using another compiler)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif ()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include(CMakePrintHelpers)
include(CheckCXXSourceCompiles)

check_cxx_source_compiles(
        "#include <iostream>
  int main()
  {
    std::cout << \"Hello World!\" << std::endl;
    return 0;
  }"
        CodeTest1
)

check_cxx_source_compiles(
        "int main()
  {
    int position[2] = {1,2};
    auto [x,y] = position;
    return 0;
  }"
        CodeTest3
        FAIL_REGEX "[Ww]arn"
)

cmake_print_variables(CodeTest1 CodeTest3)

add_executable(${PROJECT_NAME}
        test_addition.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/gtfs/src
)

target_link_libraries(${PROJECT_NAME}
        gtest gtest_main
        gtfs
)
