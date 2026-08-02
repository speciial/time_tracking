set ( SCRATCH_EXEC_NAME scratch )

add_executable( ${SCRATCH_EXEC_NAME} 
    scratch.c
)

target_link_libraries( ${SCRATCH_EXEC_NAME} PRIVATE base_lib ttr_lib )
