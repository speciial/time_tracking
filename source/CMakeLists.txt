add_subdirectory( ttr )

add_executable( ${PROJECT_NAME} )

target_sources( ${PROJECT_NAME} PUBLIC main.c )

target_link_libraries( ${PROJECT_NAME} PUBLIC ttr_lib )