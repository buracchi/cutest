#include <buracchi/cutest/cutest.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct test_suite_run_result {
    int tests_ran;
    int tests_passed;
    int tests_failed;
};

static int list_tests(struct test_suite *test_suite_list);
static int test_suite_tests_num(struct test_suite *suite);
static int test_suites_num(struct test_suite *suite_list);
static int tests_num(struct test_suite *suite_list);
static int run_test(struct test_info *test, const char *test_suite_name);
static struct test_suite_run_result run_test_suite(struct test_suite *test_suite);
static int run_all_tests(struct test_suite *test_suite_list);
static int run_filtered_tests(struct test_suite *test_suite_list, const char *filter);

extern int main(int argc, char *argv[argc + 1]) {
    if (argc > 1 && strcmp(argv[1], "--cutest_list_tests") == 0) {
        return list_tests(cutest_test_suite_list);
    }
    if (argc > 1 && strncmp(argv[1], "--cutest_filter=", strlen("--cutest_filter=")) == 0) {
        const char *filter = argv[1] + strlen("--cutest_filter=");
        return run_filtered_tests(cutest_test_suite_list, filter);
    }
    return run_all_tests(cutest_test_suite_list);
}

static int list_tests(struct test_suite *test_suite_list) {
    struct test_suite *current = test_suite_list;
    printf("Place holder message: Running main() from PATH\\test_main.c\n");
    while (current != nullptr) {
        struct test_info *current_test = current->test_list;
        printf("%s.\n", current->name);
        while (current_test != nullptr) {
            printf("  %s\n", current_test->name);
            current_test = current_test->next;
        }
        current = current->next;
    }
    return EXIT_SUCCESS;
}

static inline int test_suite_tests_num(struct test_suite *suite) {
    int num_tests = 0;
    struct test_info *current_test = suite->test_list;
    while (current_test != nullptr) {
        num_tests++;
        current_test = current_test->next;
    }
    return num_tests;
}

static inline int test_suites_num(struct test_suite *suite_list) {
    int num_suites = 0;
    struct test_suite *current_suite = suite_list;
    while (current_suite != nullptr) {
        num_suites++;
        current_suite = current_suite->next;
    }
    return num_suites;
}

static inline int tests_num(struct test_suite *suite_list) {
    int num_tests = 0;
    struct test_suite *current_suite = suite_list;
    while (current_suite != nullptr) {
        num_tests += test_suite_tests_num(current_suite);
        current_suite = current_suite->next;
    }
    return num_tests;
}

static int run_test(struct test_info *test, const char *test_suite_name) {
    clock_t start_time;
    clock_t end_time;
    double elapsed_time;
    printf("[ RUN      ] %s.%s\n", test_suite_name, test->name);
    start_time = clock();
    test->test();
    end_time = clock();
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    if (test->result) {
        printf("[       OK ]");
    }
    else {
        printf("[  FAILED  ]");
    }
    printf(" %s.%s (%.2f ms)\n", test_suite_name, test->name, elapsed_time);
    return test->result ? EXIT_SUCCESS : EXIT_FAILURE;
}

static struct test_suite_run_result run_test_suite(struct test_suite *test_suite) {
    int tests_run = 0;
    int tests_passed = 0;
    int tests_failed = 0;
    printf("[----------] %d tests from %s\n", test_suite_tests_num(test_suite), test_suite->name);
    struct test_info *current_test = test_suite->test_list;
    clock_t suite_start_time = clock();
    while (current_test != nullptr) {
        run_test(current_test, test_suite->name) == EXIT_SUCCESS ? tests_passed++ : tests_failed++;
        current_test = current_test->next;
        tests_run++;
    }
    clock_t suite_end_time = clock();
    double elapsed_time = ((double) (suite_end_time - suite_start_time)) / CLOCKS_PER_SEC * 1000;
    printf("[----------] %d tests from %s (%.2f ms total)\n", tests_run, test_suite->name, elapsed_time);
    return (struct test_suite_run_result) {
        .tests_ran = tests_run,
        .tests_passed = tests_passed,
        .tests_failed = tests_failed
    };
}

static int run_all_tests(struct test_suite *test_suite_list) {
    int total_tests_passed = 0;
    int total_tests_failed = 0;
    int total_tests_ran = 0;
    int total_suites_run = 0;
    int num_tests = tests_num(test_suite_list);
    int num_suites = test_suites_num(test_suite_list);
    printf("[==========] Running %d tests from %d test suites.\n", num_tests, num_suites);
    printf("[----------] Global test environment set-up.\n");
    clock_t total_start_time = clock();
    struct test_suite *current_suite = test_suite_list;
    while (current_suite != nullptr) {
        struct test_suite_run_result result = run_test_suite(current_suite);
        total_suites_run++;
        total_tests_ran += result.tests_ran;
        total_tests_passed += result.tests_passed;
        total_tests_failed += result.tests_failed;
        current_suite = current_suite->next;
    }
    clock_t total_end_time = clock();
    double elapsed_time = ((double) (total_end_time - total_start_time)) / CLOCKS_PER_SEC * 1000;
    printf("\n[----------] Global test environment tear-down.\n");
    printf("[==========] %d test from %d test suite ran. (%.2f ms total)\n",
           total_tests_ran,
           total_suites_run,
           elapsed_time);
    printf("[  PASSED  ] %d tests.\n", total_tests_passed);
    if (total_tests_failed > 0) {
        printf("[  FAILED  ] %d test, listed below:\n", total_tests_failed);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int run_filtered_tests(struct test_suite *test_suite_list, const char *filter) {
    struct test_suite *current_suite = test_suite_list;
    struct test_info *test_to_run = nullptr;
    size_t suite_name_length;
    char *dot_ptr = strchr(filter, '.');
    if (dot_ptr != nullptr) {
        suite_name_length = dot_ptr - filter;
    }
    printf("Note: Test filter = %s\n", filter);
    while (current_suite != nullptr) {
        if (strncmp(current_suite->name, filter, suite_name_length) == 0) {
            struct test_info *current_test = current_suite->test_list;
            filter = filter + suite_name_length + 1;
            while (current_test != nullptr) {
                if (strcmp(current_test->name, filter) == 0) {
                    test_to_run = current_test;
                    goto run_test;
                }
                current_test = current_test->next;
            }
        }
        current_suite = current_suite->next;
    }
run_test:
    int total_tests_passed = 0;
    int total_tests_failed = 0;
    int total_tests_ran = 0;
    int total_suites_run = 0;
    int num_tests = test_to_run == nullptr ? 0 : 1;
    int num_suites = test_to_run == nullptr ? 0 : 1;
    printf("[==========] Running %d tests from %d test suites.\n", num_tests, num_suites);
    clock_t total_start_time = clock();
    if (num_tests > 0) {
        printf("[----------] Global test environment set-up.\n");
        printf("[----------] %d tests from %s\n", num_tests, current_suite->name);
        clock_t suite_start_time = clock();
        run_test(test_to_run, current_suite->name) == EXIT_SUCCESS ? total_tests_passed++ : total_tests_failed++;
        total_tests_ran++;
        total_suites_run++;
        clock_t suite_end_time = clock();
        double elapsed_time = ((double) (suite_end_time - suite_start_time)) / CLOCKS_PER_SEC * 1000;
        printf("[----------] %d tests from %s (%.2f ms total)\n", total_tests_ran, current_suite->name, elapsed_time);
        printf("\n[----------] Global test environment tear-down.\n");
    }
    clock_t total_end_time = clock();
    double elapsed_time = ((double) (total_end_time - total_start_time)) / CLOCKS_PER_SEC * 1000;
    printf("[==========] %d test from %d test suite ran. (%.2f ms total)\n",
           total_tests_ran,
           total_suites_run,
           elapsed_time);
    printf("[  PASSED  ] %d tests.", total_tests_passed);
    if (total_tests_failed > 0) {
        printf("[  FAILED  ] %d test, listed below:\n", total_tests_failed);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
