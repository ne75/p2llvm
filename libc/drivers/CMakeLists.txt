add_library(drivers OBJECT
    SimpleSerial.c
    terminal.c
)

target_include_directories(drivers PRIVATE ${CMAKE_SOURCE_DIR}/include)