include( CTest )
enable_testing()

set( TEST_FILE_LIST
    test_reader.c
    test_timer.c
    test_ttr.c
)
create_test_sourcelist( TEST_SOURCE_LIST ttr_test_driver.c ${TEST_FILE_LIST} )

add_executable( ${PROJECT_NAME}_test ${TEST_SOURCE_LIST} )
target_link_libraries( ${PROJECT_NAME}_test PUBLIC base_lib ttr_lib )

foreach(test ${TEST_FILE_LIST})
    get_filename_component( TEST_NAME ${test} NAME_WE )
    add_test( NAME ${TEST_NAME}
              COMMAND ${PROJECT_NAME}_test ${TEST_NAME} 
              WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_data )
endforeach()
