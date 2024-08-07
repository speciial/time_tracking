set( TEST_DEPENDENCIES
    ../source/time_tracking.c
)

add_executable( test_${PROJECT_NAME}
    ${TEST_DEPENDENCIES}    
    test_main.c
    test_time_tracking.c
)