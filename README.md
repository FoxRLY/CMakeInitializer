# CMake Initializer
Creates CMake project for C or CPP with modern structure and neat project manager bash script
## Requirements:
- Linux
- Boost   v1.83   >= 
- CMake   v3.27   >=
- Ninja   v1.11.1 >=
- GTest   v1.11   >=
- Doxygen v1.9.8  >=
Yes, reqs are a bit high, but I didn't test it on anything else
## Features:
- Fully featured command line program written with use of Boost::program_options
- Symlinking of ```compile_commands.json``` file in project root for easier clangd LSP header locating (Neovim users will appreciate)
- Common project structure with ```app```, ```src```, ```include``` and ```test``` directories. No need to specify new files in ```CMakeLists.txt``` as ```GLOB_RECURSE``` is used (Opinionated, but at least works)
- Project manager script (run without arguments to see help)
- Autodetection of project libraries (removes error-prone process of adding or removing library targets from `target_link_library` calls)
- Docs for your project with Doxygen (outdated css style my beloved)
- Tests and ease of adding new tests for your libs 
## Usage example:
1) ```cmake-init -v 3.27 -l cpp -n my-project```
2) ```cd my-project```
3) ```./cmake-pm run debug```
## FAQ:
### Can you generate projects for shared or header-only libraries?
No. But I don't see any obstacles in configuring generated project for your needs by yourself
### Is it IDE friendly?
Almost certainly not. Creating new libraries is made through bash script - good luck configuring CLion to accomodate this nuisance
### Why use bash script and not standalone project manager program?
Ease of extensibility - add your own script arguments and do what you need for your project
### Will there be Windows support?
Don't think so. Maybe very very later
## Some more info:
- Program is statically linked: no dynamic libraries are needed (don't know why, just flexin)
- This repository is created with older version, see examples for reference
- Some foolproofing is baked into program: you CANNOT create project without name, with incorrect language or run and build anything but release and debug versions
- Some foolproofing is intentionally omitted: you CAN generate cmake project with version ```BRUH``` and language standard ```19999```
- To install program systemwide just build it and copy executable from build/release into your /bin folder (or be a mage and use cmake_install)
## Example projects:
- https://github.com/FoxRLY/CMakeInitializerExampleProjects

