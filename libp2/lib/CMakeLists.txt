add_library(p2
    crt0.c
    propeller2.c
)

target_link_libraries(p2 builtins)