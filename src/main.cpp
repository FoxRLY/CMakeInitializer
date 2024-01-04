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
    boost::format common_format = boost::format("%1%/%2%");
    int standard;

    static options::options_description get_description() {
        options::options_description desc("CMake project initializer");
        desc.add_options()
            ("help,h", "Help")
            ("version,v", options::value<std::string>()->default_value("3.0"), "Minimal CMake version")
            ("name,n", options::value<std::string>(), "Project name")
            ("language,l", options::value<std::string>()->default_value("cpp"), "Language (c or cpp)")
            ("standard,s", options::value<int>()->default_value(17), "Language standard");
        return desc;
    }
    
    void create_folder_structure(){
        auto folder_format = boost::format("%1%/%2%");
        fs::create_directories((folder_format % project_name % "src").str());
        fs::create_directories((folder_format % project_name % "include").str());
        fs::create_directories((folder_format % project_name % "build").str());
        fs::create_directories((folder_format % project_name % "build/release").str());
        fs::create_directories((folder_format % project_name % "build/debug").str());
    }

    void create_cmake_file(){
        const char* cmake_file_contents = 
            "cmake_minimum_required(VERSION %1%)\n"
            "project(%2%)\n"
            "set(CMAKE_%3%_STANDARD %4%)\n"
            "set(CMAKE_%3%_FLAGS \"-Wall -Wextra\")\n"
            "set(CMAKE_%3%_FLAGS_DEBUG \"-g\")\n"
            "set(CMAKE_%3%_FLAGS_RELEASE \"-O3\")\n"
            "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n"
            "file(GLOB SOURCE_FILES ./src/*%5%)\n"
            "file(GLOB HEADER_FILES ./include/*.h)\n"
            "file(CREATE_LINK\n"
            "  \"${CMAKE_BINARY_DIR}/compile_commands.json\"\n"
            "  \"${CMAKE_SOURCE_DIR}/compile_commands.json\"\n"
            "  SYMBOLIC\n"
            ")\n\n"
            "add_executable(%2% ${SOURCE_FILES} ${HEADER_FILES})\n"
            "target_include_directories(%2% PRIVATE ./include)\n";
        std::string file_name = (common_format % project_name % "CMakeLists.txt").str();
        std::ofstream cmake_file;
        cmake_file.open(file_name, std::ios::out | std::ios::trunc);
        cmake_file << boost::format(cmake_file_contents) % cmake_version % project_name % language % standard % file_extension;
        cmake_file.close();
    }

    void create_run_file(){
        const char* run_file_contents = 
            "#!/bin/bash\n"
            "if [[ \"${1,,}\" == \"release\" ]] || [[ \"${1,,}\" == \"debug\" ]]\n"
            "then\n"
            "    cmake -B ./build/${1,,} -DCMAKE_BUILD_TYPE=${1^^} -G Ninja && ninja -C ./build/${1,,} && ./build/${1,,}/%1% \"${@:2}\"\n"
            "else\n"
            "    echo \"only release or debug\"\n"
            "fi;";
        std::string file_name = (boost::format("%1%/run.sh") % project_name).str();
        std::ofstream run_file;
        run_file.open(file_name, std::ios::out | std::ios::trunc);
        run_file << boost::format(run_file_contents) % project_name;
        run_file.close();
        fs::permissions(file_name, fs::perms::owner_exec, fs::perm_options::add);
    }

    void create_build_file(){
        const char* build_file_contents = 
            "#!/bin/bash\n"
            "if [[ \"${1,,}\" == \"release\" ]] || [[ \"${1,,}\" == \"debug\" ]]\n"
            "then\n"
            "    cmake -B ./build/${1,,} -DCMAKE_BUILD_TYPE=${1^^} -G Ninja && ninja -C ./build/${1,,}\n"
            "else\n"
            "    echo \"only release or debug\"\n"
            "fi;";
        std::ofstream build_file;
        std::string file_name = (boost::format("%1%/build.sh") % project_name).str();
        build_file.open(file_name, std::ios::out | std::ios::trunc);
        build_file << build_file_contents;
        build_file.close();
        fs::permissions(file_name, fs::perms::owner_exec, fs::perm_options::add);
    }

    void create_main_file(){
        const char* main_file_contents = 
            "int main(int argc, char* argv[]) {\n"
            "    return 0;\n"
            "}\n";
        std::ofstream main_file;
        std::string file_name = (boost::format("%1%/src/%2%%3%") % project_name % "main" % file_extension).str();
        main_file.open(file_name);
        main_file << main_file_contents;
        main_file.close();
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
        if (map.count("language")){
            std::string language_input = map["language"].as<std::string>();
            boost::to_lower(language_input);
            if (language_input == "c"){
                language = "C";
                file_extension = ".c";
            } else if (language_input == "cpp" || language_input == "c++") {
                language = "CXX";
                file_extension = ".cpp";
            } else {
                throw LogicException("language must be specified as c/cpp/c++");
            }
        }
        if (map.count("standard")){
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
        create_cmake_file();
        create_run_file();
        create_build_file();
        create_main_file();
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
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
