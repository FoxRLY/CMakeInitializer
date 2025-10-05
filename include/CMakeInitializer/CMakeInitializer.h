#pragma once
#include <string>

class CMakeInitializer {
private:
    std::string cmake_version;
    std::string project_name;
    std::string language;
    std::string file_extension;
    int standard;

    void create_folder_structure();
    void populate_root_folder();
    void populate_app_folder();
    void populate_include_folder();
    void populate_src_folder();
    void populate_docs_folder();
    void populate_test_folder();
    void populate_scripts_folder();
public:
    CMakeInitializer(int arcg, char* argv[]);
    void initialize();
};

