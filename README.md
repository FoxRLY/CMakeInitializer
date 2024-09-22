# CMake Initializer
Creates CMake project for C or CPP with modern structure and neat project manager bash script
## Requirements:
- Linux
- Boost   v1.83   >= 
- CMake   v3.27   >=
- Ninja   v1.11.1 >=
- GTest   v1.11   >=
- Doxygen v1.9.8  >= (optional)<br>
Yes, reqs are a bit high, but I didn't test it on anything else
## Initializer command overview
```
CMake project initializer:
  -h [ --help ]                Help
  -v [ --version ] arg (=3.27) Minimal CMake version
  -n [ --name ] arg            Project name
  -l [ --language ] arg (=cpp) Language (c or cpp)
  -s [ --standard ] arg (=17)  Language standard
```
## Project manager command overview
```
Project manager program:
    newlib {lib_name}     - create new project library with name 'lib_name'
    run {release/debug}   - run project in release/debug mode
    build {release/debug} - build project in release/debug mode
    docs                  - build docs (will be located in build/debug/docs directory)
    test                  - run tests (you can provide same arguments as if you were calling ctest)
    clear                 - clear build directory
```
## Usage example:
1) ```cmake-init -v 3.27 -l cpp -n my-project```
2) ```cd my-project```
3) ```./cmake-pm run debug```
## Features:
- Fully featured CLI tool written in C++ with Boost (blazingly fast)
- Symlinking of ```compile_commands.json``` file in project root for easier clangd LSP header locating (Neovim users will appreciate)
- Common project structure with ```app```, ```src```, ```include``` and ```test``` directories. No need to specify new files in ```CMakeLists.txt``` as ```GLOB_RECURSE``` is used (Opinionated, but at least works)
- Project manager script (run without arguments to see help)
- Autodetection of project libraries (removes error-prone process of adding or removing library targets from `target_link_library` calls)
- Docs for your project with Doxygen (outdated css style my beloved)
- Tests and ease of adding new tests for your libs 
## Planned features:
- CPM integration
## Distant future:
- Option to switch testing frameworks
- Python script extensions for main cmake-pm script
- ???
## FAQ:
### Can you generate projects for shared or header-only libraries?
No. But I don't see any obstacles in configuring generated project for your needs by yourself
### Is it IDE friendly?
Almost certainly not. Creating new libraries is made through bash script - good luck configuring CLion to use it.<br>
Neovim on the other hand works like a charm, just install clangd lsp through Mason and build project once
### Why use bash script and not standalone project manager program?
Ease of extensibility - add your own script arguments and do what you need for your project
### Will there be Windows support?
Don't think so. Maybe very very later and with the help of Powershell wizard
## Some more info:
- Program is statically linked: no dynamic libraries are needed (don't know why, just flexin)
- This repo is not created with the same initializer, go into ```example_project``` directory for the up-to-date example 
- Some foolproofing is baked into program: you CANNOT create project without name, with incorrect language or run and build anything but release and debug versions
- Some foolproofing is intentionally omitted: you CAN generate cmake project with version ```BRUH``` and language standard ```19999```
- To install program systemwide just build it (run ./build.sh release) and copy executable from build/release into your /bin folder (or be a mage and use cmake_install)
- Git repo initialization is intentionally not implemented (why don't you `git init` it yourself by that point?) but gitignore is there for you

