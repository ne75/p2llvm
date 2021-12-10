cmake_minimum_required(VERSION 3.11)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_TOOLCHAIN_FILE p2.cmake)

# set the project name
project(libp2)

# get git version info
execute_process(COMMAND 
    bash -c "git describe --always --dirty=\"-modified\""
    OUTPUT_VARIABLE version
)
string(STRIP ${version} version)
add_compile_definitions(__P2_GIT_VERSION__="${version}")

add_subdirectory(lib)
add_subdirectory(lib/p2db)
add_subdirectory(lib/builtins)