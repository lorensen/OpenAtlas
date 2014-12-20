cmake_minimum_required(VERSION 2.8)
 
project(OpenAtlas)

set (EXECUTABLE_OUTPUT_PATH ${OpenAtlas_BINARY_DIR})
set (LIBRARY_OUTPUT_PATH ${OpenAtlas_BINARY_DIR})
message(STATUS "OpenAtlas: Executables are stored in ${EXECUTABLE_OUTPUT_PATH}")

enable_testing()
include(CTest)

add_subdirectory(Tools)
