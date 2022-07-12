set(target ${PROJECT_NAME})
set(SOURCES
    filesystem_unix.c
    filesystem_win.c
)

if(static)
    add_library(${target} STATIC ${SOURCES})
else()
    add_library(${target} SHARED ${SOURCES})
endif()