file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

message(STATUS "configure cmake in directory ./test")

foreach( SOURCE ${TEST_SOURCES})
    get_filename_component( SOURCE_FILENAME ${SOURCE} NAME)
    string(REPLACE ".cpp" "" OUTPUT_NAME  ${SOURCE_FILENAME})
    add_executable(${OUTPUT_NAME} EXCLUDE_FROM_ALL ${SOURCE})
    target_link_libraries(${OUTPUT_NAME} server_shared)

endforeach(SOURCE ${TEST_SOURCES})

