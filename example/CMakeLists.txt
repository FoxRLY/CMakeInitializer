cmake_minimum_required(VERSION 3.27)

include(./scripts/macros.cmake)

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

cmake_pm_add_CPM()

set(LIBRARY_LIST "")

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(app)
    cmake_pm_add_tests()
    # cmake_pm_add_docs()
    # cmake_pm_add_graphviz()
    cmake_pm_add_compile_commands()
endif()
