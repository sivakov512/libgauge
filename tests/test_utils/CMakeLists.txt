set(TEST_GAUGE_UTILS_TESTS
    test_normalize_angle
)

foreach(TESTNAME IN LISTS TEST_GAUGE_UTILS_TESTS)
    add_executable(${TESTNAME} ${TESTNAME}.c)

    target_link_libraries(${TESTNAME} PRIVATE gauge unity)

    set_target_properties(${TESTNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endforeach()
