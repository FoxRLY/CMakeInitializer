cmake_minimum_required(VERSION 3.27)
project(
    example
    VERSION 0.1.0
    DESCRIPTION "project description"
    LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
enable_testing()
include(GoogleTest)
find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found - no docs will be generated")
endif()
set(LIBRARY_LIST "")
add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(test)
file(CREATE_LINK
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
    SYMBOLIC
)