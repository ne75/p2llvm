add_library(drivers OBJECT
    SimpleSerial.c
    FdSerial.c
    terminal.c
    memory.c
)

target_include_directories(drivers PRIVATE ${CMAKE_SOURCE_DIR}/include)