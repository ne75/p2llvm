add_library(string OBJECT
    memchr.c
    memcmp.c
	strcat.c
    strcmp.c
    strcpy.c
    strlen.c
	strncat.c
    strncmp.c
    strncpy.c
	strchr.c
    strrchr.c
    strspn.c
    strcspn.c
    strtok.c
	strcoll.c
    strxfrm.c
	strstr.c
    strpbrk.c
	strerror.c
    strcasecmp.c
    strncasecmp.c
	strdup.c
)

target_include_directories(string PRIVATE ${CMAKE_SOURCE_DIR}/include)