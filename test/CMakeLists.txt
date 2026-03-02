include( CTest )
enable_testing()

set( TEST_TARGET_NAME ${PROJECT_NAME}_test )

add_executable( ${TEST_TARGET_NAME} )

target_sources( ${TEST_TARGET_NAME} PUBLIC 
    test_main.c
)

add_test( NAME ${TEST_TARGET_NAME}
          COMMAND ${TEST_TARGET_NAME} 
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_data )