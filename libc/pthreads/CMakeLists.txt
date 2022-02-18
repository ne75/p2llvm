add_library(pthreads OBJECT
    pthread_create.c
    pthread_join.c
    pthread_detach.c
	pthread_mutex.c
    pthread_attr.c
    pthread_io.c
    pthread_cond.c
	pthread_key.c
    semaphore.c
)

target_include_directories(pthreads PRIVATE ${CMAKE_SOURCE_DIR}/include)