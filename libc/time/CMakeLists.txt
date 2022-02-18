add_library(time OBJECT
    clock.c
    sleep.c
    usleep.c
    time.c
    difftime.c
    strftime.c
    localtim.c
    asctime.c
	settimeofday.c
    gettimeofday.c
)

target_include_directories(time PRIVATE ${CMAKE_SOURCE_DIR}/include)