set(TEST_GAUGE_TESTS
    test_update_background
    test_calibrate_by_axis_intersection
    test_calibrate_spin
    test_scan_radial
)

foreach(TESTNAME IN LISTS TEST_GAUGE_TESTS)
    add_executable(${TESTNAME} ${TESTNAME}.c)

    target_link_libraries(${TESTNAME} PRIVATE gauge unity tu)

    target_compile_definitions(${TESTNAME} PRIVATE
        TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../fixtures"
        TEST_SNAPSHOTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../snapshots/test_gauge/${TESTNAME}"
        TEST_EXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../examples/test_gauge/${TESTNAME}")

    set_target_properties(${TESTNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endforeach()
