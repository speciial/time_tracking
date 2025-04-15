set( BASE_LIB_TARGET base_lib )
add_library( ${BASE_LIB_TARGET} )

target_sources( ${BASE_LIB_TARGET} PUBLIC arena.c strings.c files.c time_and_date.c )

target_include_directories( ${BASE_LIB_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )