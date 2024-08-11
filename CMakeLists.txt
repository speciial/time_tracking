cmake_minimum_required( VERSION 3.22 )

set( CMAKE_C_STANDARD 11 )
set( CMAKE_C_STANDARD_REQUIRED ON )

# Put executable into bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

project( ttr VERSION 1.0 )

add_executable( ${PROJECT_NAME} 
    source/main.c
    source/files.c
    source/time_tracking.c
    source/cmd_line_opt.c
)

# add_subdirectory(test)