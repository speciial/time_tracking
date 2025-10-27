set( BASE_LIB_TARGET base )
add_library( ${BASE_LIB_TARGET} )

target_sources( ${BASE_LIB_TARGET} PUBLIC 
    base_arena.cpp
    base_string.cpp
    base_datetime.cpp
    # base_testing.cpp
)

target_include_directories( ${BASE_LIB_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
