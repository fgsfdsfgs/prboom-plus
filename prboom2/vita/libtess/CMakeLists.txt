# Vita port of libtess from https://github.com/aljen/haiku-opengl
# Provides gluTess* functions

set(LIBTESS_SOURCES
    dict.c
    geom.c
    memalloc.c
    mesh.c
    normal.c
    priorityq-heap.c
    priorityq.c
    render.c
    sweep.c
    tess.c
    tessmono.c
)

add_library(tess ${LIBTESS_SOURCES})
target_include_directories(tess PRIVATE ${CMAKE_SOURCE_DIR}/vita)
