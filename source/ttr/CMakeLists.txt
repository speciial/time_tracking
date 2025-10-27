set( TIME_TRACKING_LIB_TARGET ttr_lib )
add_library( ${TIME_TRACKING_LIB_TARGET} )

target_sources( ${TIME_TRACKING_LIB_TARGET} PUBLIC 
    # time_tracking.cpp 
    # tokenizer.cpp 

    ttr.cpp
    ttr_reader.cpp
)

target_include_directories( ${TIME_TRACKING_LIB_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries( ${TIME_TRACKING_LIB_TARGET} PUBLIC base )