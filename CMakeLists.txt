cmake_minimum_required( VERSION 3.22 )

set( CMAKE_C_STANDARD 11 )
set( CMAKE_C_STANDARD_REQUIRED ON )

# Put executable into bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin )

project( ttr VERSION 2.0 )

add_library( ttr_warnings INTERFACE )

if( MSVC )
    target_compile_options( ttr_warnings INTERFACE /W4 /permissive- )
elseif ( CMAKE_C_COMPILER_ID STREQUAL "GNU" )
    target_compile_options( ttr_warnings INTERFACE -Wall -Wextra )
endif()

add_subdirectory( source )
add_subdirectory( test )

# This line is added to make sure vscode can find the test config
enable_testing()