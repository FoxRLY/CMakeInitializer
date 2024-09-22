cmake_minimum_required(VERSION 3.27)
project(cmake-init)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_FILES ./src/*.cpp)
file(GLOB HEADER_FILES ./include/*.h)
file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)

include(./cpm/CPM.cmake)

CPMAddPackage(
  NAME Boost
  VERSION 1.83.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.xz
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES program_options"
)

set(Boost_USE_STATIC_LIBS ON)

add_executable(cmake-init ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(cmake-init PRIVATE ./include)

set(CMAKE_EXE_LINKER_FLAGS " -static")
target_link_libraries(cmake-init Boost::program_options -static-libgcc -static-libstdc++)
