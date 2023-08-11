add_executable(test_example "test.c")
target_link_libraries(test_example PRIVATE cutest cutest_main)
target_link_libraries(test_example INTERFACE coverage_config)
cutest_discover_tests(test_example)
