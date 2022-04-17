cmake_minimum_required(VERSION 3.11)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_TOOLCHAIN_FILE p2.cmake)

# set the project name
project(libc)

add_subdirectory(drivers)
add_subdirectory(math)
add_subdirectory(misc)
# add_subdirectory(pthreads)
add_subdirectory(stdio)
add_subdirectory(stdlib)
add_subdirectory(string)
# add_subdirectory(time)
add_subdirectory(wchar)

add_library(c STATIC)
target_link_libraries(c PUBLIC drivers math misc stdio stdlib string time wchar)