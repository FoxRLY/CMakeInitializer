# CMake Initializer
Creates CMake project with common structure, clangd LSP support and bash scripts for easy build and run sequences in both release and debug modes.
## Requirements:
- Linux
- Boost v1.83 >=
- CMake v3.0 >=
- Ninja v1.11.1 >= (didn't test older versions, use 1.11 if possible)
## Features:
- Fully featured command line program written with use of Boost::program_options
- Symlinking of ```compile_commands.json``` file in project root for easier clangd LSP header locating (Neovim users will appreciate)
- Common project structure with ```src``` and ```include``` directories. No need to specify new files in ```CMakeLists.txt``` as ```GLOB_RECURSE``` is used (Opinionated, but at least works)
- Bash scripts for running and building project in release and debug modes
## Example:
1) ```cmake-init -v 3.27 -l cpp -n my-project```
2) ```cd my-project```
3) ```./run.sh debug```
## Some more info:
- Program is statically linked: no dynamic libraries are needed (don't know why, just flexin)
- This repository is created with this program: exactly this project structure will be generated
- Some foolproofing is baked into program: you CANNOT create project without name, with incorrect language or run and build anything but release and debug versions
- Some foolproofing is intentionally omitted: you CAN generate cmake project with version ```BRUH``` and language standard ```OHIO```
- To install program systemwide just build it and copy executable from build/release into your /bin folder
