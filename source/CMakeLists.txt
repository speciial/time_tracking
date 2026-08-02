add_subdirectory( base )
add_subdirectory( ttr )
add_subdirectory( scratch )

add_executable( ${PROJECT_NAME} 
    main.c
)

target_link_libraries( ${PROJECT_NAME} PRIVATE base_lib ttr_lib )
