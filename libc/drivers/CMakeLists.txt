add_library(drivers OBJECT
    SimpleSerial.c
    FdSerial.c
    terminal.c
    memory.c
    # SDDriver.c
    # diskio.c
    # ff.c
    # sd_mmc.c
)

target_include_directories(drivers PRIVATE ${CMAKE_SOURCE_DIR}/include)