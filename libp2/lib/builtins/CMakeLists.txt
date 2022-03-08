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
    fixsfsi.c
    fixunsdfdi.c
    fixunsdfsi.c
    fixunssfdi.c
    fixunssfsi.c
    floatsidf.c
    floatundidf.c
    floatunsidf.c
    floatunsisf.c
    floatsisf.c
    fp_mode.c
    lshrdi3.c
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
)

# create a compiler_builtins library for rust
add_library(compiler_builtins STATIC)

target_link_libraries(compiler_builtins builtins)