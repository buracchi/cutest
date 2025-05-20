add_executable(test_example "example.c")
target_link_libraries(test_example
                      INTERFACE coverage_config
                      PRIVATE cutest_main)
cutest_discover_tests(test_example)

add_executable(test_asserts "asserts.c")
target_link_libraries(test_asserts
                      INTERFACE coverage_config
                      PRIVATE cutest_main)
cutest_discover_tests(test_asserts)
