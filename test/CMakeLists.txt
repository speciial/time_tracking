enable_testing()

set( TEST_TARGET_NAME ttr_test )
add_executable( ${TEST_TARGET_NAME} )

target_sources( ${TEST_TARGET_NAME} PUBLIC test_main.c )

target_link_libraries( ${TEST_TARGET_NAME} PUBLIC ttr_lib )

add_test( NAME ${TEST_TARGET_NAME}
          COMMAND ${TEST_TARGET_NAME} 
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_data )