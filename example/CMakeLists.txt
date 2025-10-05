cmake_minimum_required(VERSION 3.27)
project(
    example
    VERSION 0.1.0
    DESCRIPTION "project description"
    LANGUAGES C CXX)
set(PROJECT_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(./cpm/CPM.cmake)
set(CPM_SOURCE_CACHE ~/.cache/CPM)

set(LIBRARY_LIST "")

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(app)

    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found - no docs will be generated")
    endif()

    find_package(GTest)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(test)
    else()
        message(STATUS "GoogleTest not found - tests are not available")
    endif()

    file(CREATE_LINK
        "${PROJECT_BINARY_DIR}/compile_commands.json"
        "${PROJECT_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endif()
