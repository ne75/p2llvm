add_library(stdlib OBJECT
    malloc.c
    calloc.c
    realloc.c
    assert.c
    atoi.c
    atoll.c
    atof.c
    atexit.c
    abort.c
    qsort.c
    bsearch.c
    signal.c
    strtol.c
    strtoul.c
    strtod.c
    strtof.c
    strtoll.c
    strtoull.c
    assert.c
    rand.c
    div.c
    lldiv.c
    abs.c
    llabs.c
    getenv.c
    multibyte.c
    fenv.c
    mblencur.c
    init_cstd.c
    c++.cpp
)

target_include_directories(stdlib PRIVATE ${CMAKE_SOURCE_DIR}/include)