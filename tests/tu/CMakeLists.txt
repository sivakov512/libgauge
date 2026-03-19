add_library(tu STATIC
    3rdparty/cJSON.c
    3rdparty/stb_impl.c
    convert.c
    examples.c
    fixtures.c
    image.c
    json.c
    snapshot.c
    utils.c
)
target_include_directories(tu PUBLIC . 3rdparty)
target_link_libraries(tu PUBLIC gauge m unity)
