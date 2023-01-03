cmake_minimum_required(VERSION 3.11)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_TOOLCHAIN_FILE p2.cmake)

# set the project name
project(libp2++)

add_subdirectory(lib)