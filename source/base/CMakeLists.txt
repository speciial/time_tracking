add_library( base_lib
    base_arena.c
    base_string.c
    base_file.c
    base_datetime.c
)

target_include_directories( base_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( base_lib PRIVATE ttr_warnings )