add_library(misc OBJECT
    ctype.c
    toupper.c
    tolower.c
    isalpha.c
    isalnum.c
    iscntrl.c
    isprint.c
    isspace.c
    isdigit.c
    isblank.c
    isxdigit.c
    isupper.c
    islower.c
    isgraph.c
    ispunct.c
    sbrk.c
    locale.c
    thread.c
    # mkdir.c
    # rmdir.c
)

target_include_directories(misc PRIVATE ${CMAKE_SOURCE_DIR}/include)