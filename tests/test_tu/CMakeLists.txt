set(TEST_UTILS_TESTS test_json test_image test_convert test_draw test_snapshot)

foreach(TESTNAME IN LISTS TEST_UTILS_TESTS)
    add_executable(${TESTNAME} ${TESTNAME}.c)

    target_link_libraries(${TESTNAME} PRIVATE unity tu)

    target_compile_definitions(${TESTNAME} PRIVATE
        TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../fixtures"
        TEST_SNAPSHOTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../snapshots/test_tu/${TESTNAME}"
        TEST_EXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../examples/test_tu/${TESTNAME}")

    set_target_properties(${TESTNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endforeach()
