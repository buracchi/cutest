add_executable(test_example "example.c")
target_link_libraries(test_example PRIVATE cutest cutest_main)
target_link_libraries(test_example INTERFACE coverage_config)
cutest_discover_tests(test_example)

add_executable(test_asserts "asserts.c")
target_link_libraries(test_asserts PRIVATE cutest cutest_main)
target_link_libraries(test_asserts INTERFACE coverage_config)
cutest_discover_tests(test_asserts)
