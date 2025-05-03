set(LIB_NAME CMakeInitializer)
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.h")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/${LIB_NAME}/*.cpp")
add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${LIB_NAME} Boost::program_options Boost::format Boost::algorithm)
target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
list(APPEND LIBRARY_LIST ${LIB_NAME})
set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)

