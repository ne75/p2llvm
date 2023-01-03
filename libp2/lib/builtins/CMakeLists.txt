add_library(builtins OBJECT
    adddf3.c
    addsf3.c
    ashldi3.c
    ashrdi3.c
    comparedf2.c
    comparesf2.c
    divdi3.c
    divdf3.c
    divsi3.c
    divsf3.c
    extendsfdf2.c
    fixdfsi.c
    fixsfdi.c
    fixsfsi.c
    fixunsdfdi.c
    fixunsdfsi.c
    fixunssfdi.c
    fixunssfsi.c
    floatdisf.c
    floatsidf.c
    floatundidf.c
    floatundisf.c
    floatunsidf.c
    floatunsisf.c
    floatsisf.c
    fp_mode.c
    lshrdi3.c
    memcpy.c
    memmove.c
    memset.c
    moddi3.c
    modsi3.c
    muldf3.c
    mulsf3.c
    muldi3.c
    negdi2.c
    subdf3.c
    subsf3.c
    udivdi3.c
    udivmoddi4.c
    umoddi3.c

    sqrtf.c
    powf.c
)

target_include_directories(builtins PRIVATE ${CMAKE_SOURCE_DIR}/include)

# create a compiler_builtins library for rust
add_library(compiler_builtins STATIC)

target_link_libraries(compiler_builtins builtins)