add_library(p2
    crt0.c
    propeller2.c
)

target_include_directories(p2 PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(p2 builtins)