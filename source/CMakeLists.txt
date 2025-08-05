add_subdirectory( base )
add_subdirectory( ttr )

add_executable( ${PROJECT_NAME} )

target_sources( ${PROJECT_NAME} PUBLIC main.cpp )

target_link_libraries( ${PROJECT_NAME} PUBLIC base ttr_lib )