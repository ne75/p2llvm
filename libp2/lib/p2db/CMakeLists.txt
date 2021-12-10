add_compile_options(-falign-functions=128)

add_library(p2db STATIC
    p2db_isr.c
    p2db.c
)
