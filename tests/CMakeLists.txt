enable_testing()

include_directories(.)

set(TESTS
    TransposeTest
)

set(TEST_DATA_PATH "../../TestData/")
set(DTEST_OUTPUT_PATH "../../TestOutput/")

add_definitions(-DTEST_DATA_PATH="${TEST_DATA_PATH}")
add_definitions(-DTEST_OUTPUT_PATH="${DTEST_OUTPUT_PATH}")

foreach(test ${TESTS})
        add_executable(${test} ${test}.cpp ${lib_headers})

        target_link_libraries(${test} computecl)
        target_link_libraries(${test} boost_unit_test_framework)

        add_test(${test} tests/${test})
endforeach(test)

