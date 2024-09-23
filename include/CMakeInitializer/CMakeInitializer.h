#pragma once
#include <string>

class CMakeInitializer {
private:
    void create_folder_structure();
    void populate_root_folder();

    void create_app_file();
    void create_app_cmake_file();
    void populate_app_folder();

    void create_include_file();
    void populate_include_folder();

    void create_src_cmake_file();
    void create_src_example_lib();
    void create_src_example_lib_cmake_file();
    void populate_src_folder();

    void populate_docs_folder();

    void populate_test_folder();
public:
    std::string cmake_version;
    std::string project_name;
    std::string language;
    std::string file_extension;
    int standard;

    CMakeInitializer(int arcg, char* argv[]);
    void initialize();
};

