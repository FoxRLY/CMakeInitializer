#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <CMakeInitializer/CMakeInitializer.h>
#include <CMakeInitializer/Errors.h>
#include <CMakeInitializer/FileContents.h>

namespace options = boost::program_options;
namespace fs = std::filesystem;

static options::options_description get_description();

CMakeInitializer::CMakeInitializer(int argc, char* argv[]) {
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
}

void CMakeInitializer::initialize() {
    create_folder_structure();
    populate_root_folder();
    populate_app_folder();
    populate_include_folder();
    populate_src_folder();
    populate_docs_folder();
    populate_test_folder();
    populate_scripts_folder();
    const char* result_output_format = R""""(Project creation successfull:
Project name: %1%
Language: %2%%3%
CMake version: %4%
)"""";
    std::cout << boost::format(result_output_format) % project_name % language % standard % cmake_version;
}

static options::options_description get_description() {
    options::options_description desc("CMake project initializer");
    desc.add_options()
        ("help,h", "Help")
        ("version,v", options::value<std::string>()->default_value("3.27"), "Minimal CMake version")
        ("name,n", options::value<std::string>(), "Project name")
        ("language,l", options::value<std::string>()->default_value("cpp"), "Language (c or cpp)")
        ("standard,s", options::value<int>()->default_value(20), "Language standard");
    return desc;
}

void CMakeInitializer::create_folder_structure() {
    fs::create_directories(project_name + "/src");
    fs::create_directories(project_name + "/scripts");
    fs::create_directories(project_name + "/include");
    fs::create_directories(project_name + "/docs");
    fs::create_directories(project_name + "/app");
    fs::create_directories(project_name + "/test");
}

static void create_file(const std::string& file_name, const std::string& file_content) {
    std::ofstream file = std::ofstream();
    file.open(file_name);
    file << file_content;
    file.close();
}

void CMakeInitializer::populate_root_folder() {
    create_file(project_name + "/CMakeLists.txt",
        (boost::format(contents::root::cmake_file)
            % cmake_version
            % project_name
            % language
            % standard).str());
    create_file(project_name + "/.gitignore", contents::root::gitignore_file);
    create_file(project_name + "/cmake-pm",
        (boost::format(contents::root::manager_file)
            % project_name
            % file_extension).str());
    fs::permissions(project_name + "/cmake-pm", fs::perms::owner_exec, fs::perm_options::add);
}

void CMakeInitializer::populate_scripts_folder() {
    create_file(project_name + "/scripts/CPM.cmake", contents::scripts::cpm);
    create_file(project_name + "/scripts/macros.cmake", contents::scripts::macros);
}

void CMakeInitializer::populate_app_folder() {
    if (file_extension == ".c") {
        create_file(project_name + "/app/app" + file_extension, contents::app::c_file);
    } else {
        create_file(project_name + "/app/app" + file_extension, contents::app::cpp_file);
    }
    create_file(project_name + "/app/CMakeLists.txt",
        (boost::format(contents::app::cmake_file) % file_extension).str());
}

void CMakeInitializer::populate_include_folder() {
    fs::create_directories(project_name + "/include/example_lib");
    create_file(project_name + "/include/example_lib/example_lib.h", contents::include::header_file);
}

void CMakeInitializer::populate_src_folder() {
    fs::create_directories(project_name + "/src/example_lib");
    create_file(project_name + "/src/CMakeLists.txt", contents::src::cmake_file);
    create_file(project_name + "/src/example_lib/example_lib" + file_extension, contents::src::example_lib_file);
    create_file(project_name + "/src/example_lib/CMakeLists.txt",
        (boost::format(contents::src::example_lib_cmake_file) % file_extension).str());
}

void CMakeInitializer::populate_docs_folder() {
    create_file(project_name + "/docs/CMakeLists.txt", contents::docs::cmake_file);
    create_file(project_name + "/docs/mainpage.md",
        (boost::format(contents::docs::mainpage_file) % project_name).str());
}

void CMakeInitializer::populate_test_folder() {
    create_file(project_name + "/test/CMakeLists.txt", contents::test::cmake_file);
    create_file(project_name + "/test/main.cpp", contents::test::main_file);
    if (file_extension == ".c") {
        create_file(project_name + "/test/exampletest.cpp", contents::test::example_lib_c_file);
    } else {
        create_file(project_name + "/test/exampletest.cpp", contents::test::example_lib_cpp_file);
    }
}

