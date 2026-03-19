set(TEST_CV_TESTS
    test_calculate_background
    test_subtract_background
    test_binarize
    test_extract_largest_blob
    test_blob_to_line
)

foreach(TESTNAME IN LISTS TEST_CV_TESTS)
    add_executable(${TESTNAME} ${TESTNAME}.c)

    target_link_libraries(${TESTNAME} PRIVATE gauge unity tu)

    target_compile_definitions(${TESTNAME} PRIVATE
        TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../fixtures"
        TEST_SNAPSHOTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../snapshots/test_cv/${TESTNAME}"
        TEST_EXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../examples/test_cv/${TESTNAME}")

    set_target_properties(${TESTNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endforeach()
