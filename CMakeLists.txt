cmake_minimum_required(VERSION 3.14)

project(http)

set(CMAKE_C_STANDARD 99)

aux_source_directory(. SOURCE_DIR)

add_executable(http ${SOURCE_DIR} request.c)