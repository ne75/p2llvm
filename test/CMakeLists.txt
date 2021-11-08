cmake_minimum_required(VERSION 3.11)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_TOOLCHAIN_FILE p2.cmake)

# set the project name
project(P2LLVMTests)

# get git version info
execute_process(COMMAND 
    bash -c "git describe --always --dirty=\"-modified\""
    OUTPUT_VARIABLE version
)
string(STRIP ${version} version)
add_compile_definitions(__P2_GIT_VERSION__="${version}")

# add optimizer enabled or disabled option
option(OPTIMIZE "Enable optimization in compiling" OFF)

if(OPTIMIZE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os" CACHE STRING "" FORCE)
endif()

function (add_executable _name)
    _add_executable(${ARGV})
    if (TARGET ${_name})
        add_custom_command(TARGET ${_name} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${_name} ${_name}.bin
            COMMENT "Creating bin..."
        )
    endif()
endfunction()

add_subdirectory(src)