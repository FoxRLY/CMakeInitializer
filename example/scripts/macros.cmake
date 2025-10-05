cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

macro(cmake_pm_add_docs)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found - no docs will be generated")
    endif()
endmacro()

macro(cmake_pm_add_tests)
    find_package(GTest)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(test)
    else()
        message(STATUS "GoogleTest not found - tests are not available")
    endif()
endmacro()

macro(cmake_pm_add_graphviz)
    find_program(GRAPHVIZ dot)
    if(GRAPHVIZ)
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/_graphviz
            COMMAND ${CMAKE_COMMAND} "--graphviz=graphviz/deps.txt" .
            COMMAND dot -Tsvg graphviz/deps.txt -o deps.svg
            COMMAND dot -Tpng graphviz/deps.txt -o deps.png
            COMMENT "Plotting dependencies graph to deps.svg"
            DEPENDS "${CMAKE_PROJECT_NAME}-app"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
        add_custom_target(graphviz ALL
            DEPENDS ${CMAKE_BINARY_DIR}/_graphviz)
    else()
        message(STATUS "GraphViz not found - dependency graph will not be generated")
    endif()
endmacro()

macro(cmake_pm_add_compile_commands)
    file(CREATE_LINK
        "${PROJECT_BINARY_DIR}/compile_commands.json"
        "${PROJECT_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endmacro()

macro(cmake_pm_add_CPM)
    include(./scripts/CPM.cmake)
    set(CPM_SOURCE_CACHE ~/.cache/CPM)
endmacro()
