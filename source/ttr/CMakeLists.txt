add_library( ttr_lib
    ttr.c
    timer.c
)

target_link_libraries( ttr_lib PRIVATE base_lib )

target_include_directories( ttr_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )