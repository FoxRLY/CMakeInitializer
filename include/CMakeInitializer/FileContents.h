#pragma once

namespace contents {

namespace root {

const char* const cmake_file = R""""(cmake_minimum_required(VERSION %1%)
project(
    %2%
    VERSION 0.1.0
    DESCRIPTION "project description"
    LANGUAGES C CXX)
set(CMAKE_%3%_STANDARD %4%)
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
)"""";

const char* const manager_file = R""""(#!/bin/bash
project_name=%1%
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path";

if [[ "${1,,}" == "build" ]]
then
    if [[ "${2,,}" == "release" ]] || [[ "${2,,}" == "debug" ]]
    then
        cmake -B ./build/${2,,} -DCMAKE_BUILD_TYPE=${2^^} -G Ninja && cmake --build build/${2,,}
    else
        echo 'Build avalable only in release and debug mode'
    fi;
    exit
fi;

if [[ "${1,,}" == "run" ]]
then
    if [[ "${2,,}" == "release" ]] || [[ "${2,,}" == "debug" ]]
    then
        cmake -B ./build/${2,,} -DCMAKE_BUILD_TYPE=${2^^} -G Ninja && cmake --build build/${2,,} && ./build/${2,,}/app/${project_name} "${@:3}"
    else
        echo 'Run avalable only in release and debug mode'
    fi;
    exit
fi;

if [[ "${1,,}" == "newlib" ]]
then
    if [ -d ./include/${2} ] || [ -d ./src/${2} ]
    then
        echo 'Include or src directories already have similarly named library, aborting'
        exit
    fi;
    mkdir ./include/${2}
    echo '#pragma once' >> ./include/${2}/${2}.h
    echo '' >> ./include/${2}/${2}.h
    echo 'int sum(int a, int b);' >> ./include/${2}/${2}.h
    mkdir ./src/${2}
    echo "#include <${2}/${2}.h>" >> ./src/${2}/${2}%2%
    echo '' >> ./src/${2}/${2}%2%
    echo 'int sum(int a, int b){ return a + b; }' >> ./src/${2}/${2}%2%
    echo "set(LIB_NAME ${2})" >> ./src/${2}/CMakeLists.txt
    echo 'file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.h")' >> ./src/${2}/CMakeLists.txt
    echo 'file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/${LIB_NAME}/*%2%")' >> ./src/${2}/CMakeLists.txt
    echo 'add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})' >> ./src/${2}/CMakeLists.txt
    echo 'target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")' >> ./src/${2}/CMakeLists.txt
    echo 'list(APPEND LIBRARY_LIST ${LIB_NAME})' >> ./src/${2}/CMakeLists.txt
    echo 'set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)' >> ./src/${2}/CMakeLists.txt
    echo "Generated new library ${2}"
    exit
fi;

if [[ "${1,,}" == "docs" ]]
then
    cmake -B ./build/debug -DCMAKE_BUILD_TYPE=DEBUG -G Ninja && cmake --build ./build/debug --target docs
    exit
fi;

if [[ "${1,,}" == "clear" ]]
then
    rm -rf ./build
    echo 'Cleared build directory'
    exit
fi;

echo 'Project manager program:'
echo '    newlib {lib_name}     - create new project library with name lib_name'
echo '    run {release/debug}   - run project in release/debug mode'
echo '    build {release/debug} - build project in release/debug mode'
echo '    docs                  - build docs (will be located in build/debug/docs directory)'
echo '    test                  - run tests (you can provide same arguments as if you were calling ctest)'
echo '    clear                 - clear build directory'
)"""";

const char* const gitignore_file = R""""(/build
/.cache
compile_commands.json
)"""";
}

namespace app {

const char* const c_file = R""""(#include <stdio.h>
#include <example_lib/example_lib.h>

int main(int argc, char* argv[]) {
    printf("%i", sum(10, 20));
}
)"""";

const char* const cpp_file = R""""(#include <iostream>
#include <example_lib/example_lib.h>

int main(int argc, char* argv[]) {
    std::cout << sum(10, 20) << std::endl;
}
)"""";

const char* const cmake_file = R""""(add_executable(${CMAKE_PROJECT_NAME} app%1%)
set(GENERAL_COMPILE_FLAGS "-Wall;-Wextra")
set(DEBUG_COMPILE_FLAGS "${GENERAL_COMPILE_FLAGS};-g;-O0")
set(RELEASE_COMPILE_FLAGS "${GENERAL_COMPILE_FLAGS};-O3")
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_COMPILE_FLAGS}>")
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_FLAGS}>")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBRARY_LIST})
)"""";
}

namespace include {

const char* const header_file = R""""(#pragma once

int sum(int a, int b);
)"""";
}

namespace src {

const char* const cmake_file = R""""(file(GLOB V_GLOB LIST_DIRECTORIES true "*")
foreach(item ${V_GLOB})
    if(IS_DIRECTORY ${item})
        add_subdirectory(${item})
    endif()
endforeach()
set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)
)"""";

const char* const example_lib_file = R""""(#include <example_lib/example_lib.h>

int sum(int a, int b) { return a + b; }
)"""";

const char* const example_lib_cmake_file = R""""(set(LIB_NAME example_lib)
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/${LIB_NAME}/*%1%")
add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
list(APPEND LIBRARY_LIST ${LIB_NAME})
set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)
)"""";
}

namespace docs {

const char* const cmake_file = R""""(set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
doxygen_add_docs(docs "${CMAKE_SOURCE_DIR}")
)"""";

const char* const mainpage_file = R""""(# Documentation for %1% project {#mainpage}
This is docs for your project!
)"""";
}

namespace test {

const char* const main_file = R""""(#include <gtest/gtest.h>
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)"""";

const char* const cmake_file = R""""(find_package(GTest REQUIRED)
file(GLOB_RECURSE TEST_FILES "./*.cpp")
add_executable(test_exec ${TEST_FILES})
target_link_libraries(test_exec PRIVATE GTest::gtest_main ${LIBRARY_LIST})
gtest_discover_tests(test_exec)
)"""";

const char* const example_lib_c_file = R""""(#include <gtest/gtest.h>
extern "C"{
    #include <example_lib/example_lib.h>
}

TEST(ExampleTests, TestFive_Five){ ASSERT_EQ(10, sum(5, 5)); }
)"""";

const char* const example_lib_cpp_file = R""""(#include <gtest/gtest.h>
#include <example_lib/example_lib.h>

TEST(ExampleTests, TestFive_Five){ ASSERT_EQ(10, sum(5, 5)); }
)"""";

}

}

