cmake_minimum_required(VERSION 3.5.0)

project(set49mode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

# Use customized cmake macros
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
find_package(LibUSB REQUIRED)

add_subdirectory(src)