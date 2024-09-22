#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem>

namespace options = boost::program_options;
namespace fs = std::filesystem;
// Что я хочу: Консольная программа для инициализации CMake проекта
// Что должна уметь программа:
// 1) Создавать папку со структурой проекта и скриптом запуска debug и release
// 2) Позволять задавать версию cmake
// 3) Повзолять задавать название папки и проекта
// 4) Позволять задавать язык программирования (C/C++)
// 5) Позволять задавать стандарт языка

class LogicException : public std::exception {
private:
    const char *message;

public:
    LogicException(const char *message) : message(message) {}
    virtual const char *what() const throw() { return message; }
};

class ExitSignal: public std::exception {
private:
    virtual const char *what() const throw() { return "exit"; }
};

class CMakeInitializer {
private:
    std::string cmake_version;
    std::string project_name;
    std::string language;
    std::string file_extension;
    int standard;

    static options::options_description get_description() {
        options::options_description desc("CMake project initializer");
        desc.add_options()
            ("help,h", "Help")
            ("version,v", options::value<std::string>()->default_value("3.27"), "Minimal CMake version")
            ("name,n", options::value<std::string>(), "Project name")
            ("language,l", options::value<std::string>()->default_value("cpp"), "Language (c or cpp)")
            ("standard,s", options::value<int>()->default_value(17), "Language standard");
        return desc;
    }

    void create_folder_structure() {
        fs::create_directories(project_name + "/src");
        fs::create_directories(project_name + "/include");
        fs::create_directories(project_name + "/docs");
        fs::create_directories(project_name + "/app");
        fs::create_directories(project_name + "/test");
    }

    void create_root_cmake_file() {
        const char* cmake_file_contents = 
            "cmake_minimum_required(VERSION %1%)\n"
            "project(\n"
            "    %2%\n"
            "    VERSION 0.1.0\n"
            "    DESCRIPTION \"project description\"\n"
            "    LANGUAGES C CXX)\n\n"
            "set(CMAKE_%3%_STANDARD %4%)\n"
            "set(CMAKE_CXX_EXTENSIONS OFF)\n"
            "set(CMAKE_C_EXTENSIONS OFF)\n"
            "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n"
            "set_property(GLOBAL PROPERTY USE_FOLDERS ON)\n\n"
            "enable_testing()\n"
            "include(GoogleTest)\n"
            "find_package(Doxygen)\n"
            "if(Doxygen_FOUND)\n"
            "    add_subdirectory(docs)\n"
            "else()\n"
            "    message(STATUS \"Doxygen not found - no docs will be generated\")\n"
            "endif()\n\n"
            "set(LIBRARY_LIST \"\")\n\n"
            "add_subdirectory(src)\n"
            "add_subdirectory(app)\n"
            "add_subdirectory(test)\n"
            "file(CREATE_LINK\n"
            "    \"${CMAKE_BINARY_DIR}/compile_commands.json\"\n"
            "    \"${CMAKE_SOURCE_DIR}/compile_commands.json\"\n"
            "    SYMBOLIC\n"
            ")\n";
        std::string file_name = project_name + "/CMakeLists.txt";
        std::ofstream cmake_file;
        cmake_file.open(file_name, std::ios::out | std::ios::trunc);
        cmake_file << boost::format(cmake_file_contents) % cmake_version % project_name % language % standard;
        cmake_file.close();
    }

    void create_manager_file() {
        std::string file_name = project_name + "/cmake-pm";
        std::ofstream file;
        file.open(file_name);

        const char* header = 
            "#!/bin/bash\n"
            "project_name=%1%\n"
            "parent_path=$( cd \"$(dirname \"${BASH_SOURCE[0]}\")\" ; pwd -P )\n"
            "cd \"$parent_path\"\n\n";
        file << boost::format(header) % project_name;

        const char* build =
            "if [[ \"${1,,}\" == \"build\" ]]\n"
            "then\n"
            "    if [[ \"${2,,}\" == \"release\" ]] || [[ \"${2,,}\" == \"debug\" ]]\n"
            "    then\n"
            "        cmake -B ./build/${2,,} -DCMAKE_BUILD_TYPE=${2^^} -G Ninja && cmake --build build/${2,,}\n"
            "    else\n"
            "        echo 'Build avalable only in release and debug mode'\n"
            "    fi;\n"
            "    exit\n"
            "fi;\n\n";
        file << build;

        const char* run =
            "if [[ \"${1,,}\" == \"run\" ]]\n"
            "then\n"
            "    if [[ \"${2,,}\" == \"release\" ]] || [[ \"${2,,}\" == \"debug\" ]]\n"
            "    then\n"
            "        cmake -B ./build/${2,,} -DCMAKE_BUILD_TYPE=${2^^} -G Ninja && cmake --build build/${2,,} && ./build/${2,,}/app/${project_name} \"${@:3}\"\n"
            "    else\n"
            "        echo 'Run avalable only in release and debug mode'\n"
            "    fi;\n"
            "    exit\n"
            "fi;\n\n";
        file << run;

        const char* newlib =
            "if [[ \"${1,,}\" == \"newlib\" ]]\n"
            "then\n"
            "    if [ -d ./include/${2} ] || [ -d ./src/${2} ]\n"
            "    then\n"
            "        echo 'Include or src directories already have similarly named library, aborting'\n"
            "        exit\n"
            "    fi;"
            "    mkdir ./include/${2}\n"
            "    echo '#pragma once' >> ./include/${2}/${2}.h\n"
            "    echo '' >> ./include/${2}/${2}.h\n"
            "    echo 'int sum(int a, int b);' >> ./include/${2}/${2}.h\n"
            "    mkdir ./src/${2}\n"
            "    echo \"#include <${2}/${2}.h>\" >> ./src/${2}/${2}%1%\n"
            "    echo '' >> ./src/${2}/${2}%1%\n"
            "    echo 'int sum(int a, int b){ return a + b; }' >> ./src/${2}/${2}%1%\n"
            "    echo \"set(LIB_NAME ${2})\" >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'file(GLOB_RECURSE HEADER_FILES \"${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.h\")' >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'file(GLOB_RECURSE SOURCE_FILES \"${CMAKE_SOURCE_DIR}/src/${LIB_NAME}/*%1%\")' >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})' >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'target_include_directories(${LIB_NAME} PUBLIC \"${CMAKE_SOURCE_DIR}/include\")' >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'list(APPEND LIBRARY_LIST ${LIB_NAME})' >> ./src/${2}/CMakeLists.txt\n"
            "    echo 'set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)' >> ./src/${2}/CMakeLists.txt\n"
            "    echo \"Generated new library ${2}\"\n"
            "    exit\n"
            "fi;\n\n";
        file << boost::format(newlib) % file_extension;

        const char* docs =
            "if [[ \"${1,,}\" == \"docs\" ]]\n"
            "then\n"
            "    cmake -B ./build/debug -DCMAKE_BUILD_TYPE=DEBUG -G Ninja && cmake --build ./build/debug --target docs\n"
            "    exit\n"
            "fi;\n\n";
        file << docs;

        const char* test =
            "if [[ \"${1,,}\" == \"test\" ]]\n"
            "then\n"
            "    cmake -B ./build/debug -DCMAKE_BUILD_TYPE=DEBUG -G Ninja && cmake --build ./build/debug --target test_exec && GTEST_COLOR=1 ctest --test-dir ./build/debug \"${@:2}\"\n"
            "    exit\n"
            "fi;\n\n";
        file << test;

        const char* clear = 
            "if [[ \"#{1,,}\" == \"clear\" ]]\n"
            "then\n"
            "    rm -rf ./build\n"
            "    echo \"Cleared build directory\"\n"
            "    exit\n"
            "fi;\n\n";
        file << clear;

        const char* help = 
            "echo \"Project manager program:\"\n"
            "echo \"    newlib {lib_name}     - create new project library with name 'lib_name'\"\n"
            "echo \"    run {release/debug}   - run project in release/debug mode\"\n"
            "echo \"    build {release/debug} - build project in release/debug mode\"\n"
            "echo \"    docs                  - build docs (will be located in build/debug/docs directory)\"\n"
            "echo \"    test                  - run tests (you can provide same arguments as if you were calling ctest)\"\n"
            "echo \"    clear                 - clear build directory\"\n\n";
        file << help;

        file.close();
        fs::permissions(file_name, fs::perms::owner_exec, fs::perm_options::add);

    }

    void create_app_file() {
        if(file_extension == ".c") {
            const char* main_file_contents = 
                "#include <stdio.h>\n"
                "#include <example_lib/example_lib.h>\n\n"
                "int main(int argc, char* argv[]) {\n"
                "    printf(\"%i\\n\", sum(10, 20));\n"
                "}\n";
            std::ofstream main_file;
            std::string file_name = project_name + "/app/app" + file_extension;
            main_file.open(file_name);
            main_file << main_file_contents;
            main_file.close();
            return;
        }
        const char* main_file_contents = 
            "#include <iostream>\n"
            "#include <example_lib/example_lib.h>\n\n"
            "int main(int argc, char* argv[]) {\n"
            "    std::cout << sum(10, 20) << std::endl;\n"
            "}\n";
        std::ofstream main_file;
        std::string file_name = project_name + "/app/app" + file_extension;
        main_file.open(file_name);
        main_file << main_file_contents;
        main_file.close();
    }

    void create_app_cmake_file() {
        const char* contents =
            "add_executable(${CMAKE_PROJECT_NAME} app%2%)\n"
            "set(GENERAL_COMPILE_FLAGS \"-Wall;-Wextra\")\n"
            "set(DEBUG_COMPILE_FLAGS \"${GENERAL_COMPILE_FLAGS};-g;-O0\")\n"
            "set(RELEASE_COMPILE_FLAGS \"${GENERAL_COMPILE_FLAGS};-O3\")\n"
            "target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE \"$<$<CONFIG:DEBUG>:${DEBUG_COMPILE_FLAGS}>\")\n"
            "target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE \"$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_FLAGS}>\")\n"
            "target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBRARY_LIST})\n";
        std::ofstream file;
        std::string file_name = project_name + "/app/CMakeLists.txt";
        file.open(file_name);
        file << boost::format(contents) % project_name % file_extension;
        file.close();
    }

    void populate_app_folder() {
        create_app_file();
        create_app_cmake_file();
    }

    void populate_include_folder() {
        fs::create_directories(project_name + "/include/example_lib");
        const char* contents =
            "#pragma once\n\n"
            "int sum(int a, int b);\n";
        std::string file_name = project_name + "/include/example_lib/example_lib.h";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_src_cmake_file() {
        const char* contents = 
            "file(GLOB V_GLOB LIST_DIRECTORIES true \"*\")\n"
            "foreach(item ${V_GLOB})\n"
            "    if(IS_DIRECTORY ${item})\n"
            "        add_subdirectory(${item})\n"
            "    endif()\n"
            "endforeach()\n"
            "set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)\n";
        std::string file_name = project_name + "/src/CMakeLists.txt";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_src_example_lib() {
        fs::create_directories(project_name + "/src/example_lib");
        const char* contents = 
            "#include <example_lib/example_lib.h>\n\n"
            "int sum(int a, int b){ return a + b; }\n";
        std::string file_name = project_name + "/src/example_lib/example_lib" + file_extension;
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_src_example_lib_cmake_file() {
        const char* contents = 
            "set(LIB_NAME example_lib)\n"
            "file(GLOB_RECURSE HEADER_FILES \"${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.h\")\n"
            "file(GLOB_RECURSE SOURCE_FILES \"${CMAKE_SOURCE_DIR}/src/${LIB_NAME}/*%1%\")\n"
            "add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})\n"
            "target_include_directories(${LIB_NAME} PUBLIC \"${CMAKE_SOURCE_DIR}/include\")\n"
            "list(APPEND LIBRARY_LIST ${LIB_NAME})\n"
            "set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)\n";
        std::string file_name = project_name + "/src/example_lib/CMakeLists.txt";
        std::ofstream file;
        file.open(file_name);
        file << boost::format(contents) % file_extension;
        file.close();
    }

    void populate_src_folder() {
        create_src_cmake_file();
        create_src_example_lib();
        create_src_example_lib_cmake_file();
    }

    void create_docs_cmake_file() {
        const char* contents =
            "set(DOXYGEN_EXTRACT_ALL YES)\n"
            "set(DOXYGEN_BUILTIN_STL_SUPPORT YES)\n\n"
            "doxygen_add_docs(docs \"${CMAKE_SOURCE_DIR}\")\n";
        std::string file_name = project_name + "/docs/CMakeLists.txt";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_docs_mainpage_file() {
        const char* contents = 
            "# Documentation for %1% project {#mainpage}\n\n"
            "This is the docs for your project\n";
        std::string file_name = project_name + "/docs/mainpage.md";
        std::ofstream file;
        file.open(file_name);
        file << boost::format(contents) % project_name;
        file.close();
    }

    void populate_docs_folder() {
        create_docs_cmake_file();
        create_docs_mainpage_file();
    }

    void create_test_cmake_file() {
        const char* contents = 
            "find_package(GTest REQUIRED)\n"
            "file(GLOB_RECURSE TEST_FILES \"./*.cpp\")\n"
            "add_executable(test_exec ${TEST_FILES})\n"
            "target_link_libraries(test_exec PRIVATE GTest::gtest_main ${LIBRARY_LIST})\n"
            "gtest_discover_tests(test_exec)\n";
        std::string file_name = project_name + "/test/CMakeLists.txt";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_test_main_file() {
        const char* contents =
            "#include <gtest/gtest.h>\n\n"
            "int main(int argc, char** argv){\n"
            "    ::testing::InitGoogleTest(&argc, argv);\n"
            "    return RUN_ALL_TESTS();\n"
            "}\n";
        std::string file_name = project_name + "/test/main.cpp";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void create_test_example_file() {
        if(file_extension == ".c") {
            const char* contents =
                "#include <gtest/gtest.h>\n"
                "extern \"C\"{\n"
                "    #include <example_lib/example_lib.h>\n"
                "}\n\n"
                "TEST(ExampleTests, TestFive_Five){ ASSERT_EQ(10, sum(5, 5)); }\n";
            std::string file_name = project_name + "/test/exampletest.cpp";
            std::ofstream file;
            file.open(file_name);
            file << contents;
            file.close();
            return;
        }
        const char* contents =
            "#include <gtest/gtest.h>\n"
            "#include <example_lib/example_lib.h>\n\n"
            "TEST(ExampleTests, TestFive_Five){ ASSERT_EQ(10, sum(5, 5)); }\n";
        std::string file_name = project_name + "/test/exampletest.cpp";
        std::ofstream file;
        file.open(file_name);
        file << contents;
        file.close();
    }

    void populate_test_folder() {
        create_test_cmake_file();
        create_test_main_file();
        create_test_example_file();
    }

public:
    CMakeInitializer(int argc, char *argv[]) {
        options::options_description desc = get_description();
        options::variables_map map;
        options::store(options::parse_command_line(argc, argv, desc), map);
        options::notify(map);
        if (map.count("help")) {
            std::cout << desc << "\n";
            throw ExitSignal();
        }
        if (map.count("version")) {
            cmake_version = map["version"].as<std::string>();
        }
        if (map.count("language")) {
            std::string language_input = map["language"].as<std::string>();
            boost::to_lower(language_input);
            if (language_input == "c") {
                language = "C";
                file_extension = ".c";
            } else if (language_input == "cpp" || language_input == "c++") {
                language = "CXX";
                file_extension = ".cpp";
            } else {
                throw LogicException("language must be specified as c/cpp/c++");
            }
        }
        if (map.count("standard")) {
            standard = map["standard"].as<int>();
        }
        if (map.count("name")) {
            project_name = map["name"].as<std::string>();
        } else {
            throw LogicException("project name must be specified");
        }
    };
    void initialize() {
        create_folder_structure();
        create_root_cmake_file();
        create_manager_file();
        populate_app_folder();
        populate_include_folder();
        populate_src_folder();
        populate_docs_folder();
        populate_test_folder();
        const char* result_output_format = 
            "Project creation successfull:\n"
            "Project name: %1%\n"
            "Language: %2%%3%\n"
            "CMake version: %4%\n";
        std::cout << boost::format(result_output_format) % project_name % language % standard % cmake_version;
    }
};

int main(int argc, char *argv[]) {
    try {
        CMakeInitializer init = CMakeInitializer(argc, argv);
        init.initialize();
    } catch (options::error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (ExitSignal &e) {
        return 0;
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
