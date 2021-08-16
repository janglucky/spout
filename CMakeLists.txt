cmake_minimum_required(VERSION 3.5)

project(muduo LANGUAGES C CXX)

message("CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


add_subdirectory(Log)