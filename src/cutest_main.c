#include <buracchi/cutest/cutest.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct result {
	size_t suites_run;
	size_t tests_ran;
	size_t tests_passed;
	size_t tests_failed;
};

static void list_tests(struct cutest cutest[static 1]);
static void run_tests(struct cutest cutest[static 1], struct result result[static 1]);
static void run_test_suite(struct cutest_test_suite test_suite[static 1], struct result result[static 1]);
static void run_test(struct cutest_test test[static 1], const char test_suite_name[static 1]);
static void filter_tests(struct cutest cutest[static 1], size_t n, const char filter[static n]);

extern int main(int argc, char *argv[argc + 1]) {
	struct cutest *cutest = cutest_;
	if (argc > 1 && strcmp(argv[1], "--cutest_list_tests") == 0) {
		list_tests(cutest);
		return EXIT_SUCCESS;
	}
	if (argc > 1 && strncmp(argv[1], "--cutest_filter=", strlen("--cutest_filter=")) == 0) {
		char const *filter = argv[1] + strlen("--cutest_filter=");
		printf("Note: Test filtered = %s\n", filter);
		filter_tests(cutest, strlen(filter) + 1, filter);
		if (cutest == nullptr) {
			perror(strerror(errno));
			exit(1);
		}
	}
	struct result result = {};
	run_tests(cutest, &result);
	cutest_destroy(cutest);
	return !result.tests_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void list_tests(struct cutest cutest[static 1]) {
	printf("Place holder message: Running main() from PATH\\test_main.c\n");
	for (size_t i = 0; i < cutest->size; i++) {
		printf("%s.\n", cutest->suite[i].name);
		for (size_t j = 0; j < cutest->suite[i].size; j++) {
			printf("  %s\n", cutest->suite[i].test[j].name);
		}
	}
}

static void run_tests(struct cutest cutest[static 1], struct result result[static 1]) {
	printf("[==========] Running %zu tests from %zu test suites.\n", cutest->enabled_suites, cutest->enabled_tests);
	printf("[----------] Global test environment set-up.\n");
	clock_t total_start_time = clock();
	for (size_t i = 0; i < cutest->size; i++) {
		if (!cutest->suite[i].enabled) {
			continue;
		}
		run_test_suite(&cutest->suite[i], result);
	}
	clock_t total_end_time = clock();
	double elapsed_time = ((double) (total_end_time - total_start_time)) / CLOCKS_PER_SEC * 1000;
	printf("\n[----------] Global test environment tear-down.\n");
	printf("[==========] %zu test from %zu test suite ran. (%.2f ms total)\n",
	       result->tests_ran,
	       result->suites_run,
	       elapsed_time);
	printf("[  PASSED  ] %zu tests.\n", result->tests_passed);
	if (result->tests_failed > 0) {
		printf("[  FAILED  ] %zu test, listed below:\n", result->tests_failed);
	}
}

static void run_test_suite(struct cutest_test_suite test_suite[static 1], struct result result[static 1]) {
	size_t suite_tests_ran = 0;
	printf("[----------] %zu tests from %s\n", test_suite->enabled_tests, test_suite->name);
	clock_t suite_start_time = clock();
	for (size_t i = 0; i < test_suite->size; i++) {
		if (!test_suite->test[i].enabled) {
			continue;
		}
		run_test(&test_suite->test[i], test_suite->name);
		test_suite->test[i].result ? result->tests_passed++ : result->tests_failed++;
		suite_tests_ran++;
	}
	clock_t suite_end_time = clock();
	double elapsed_time = ((double) (suite_end_time - suite_start_time)) / CLOCKS_PER_SEC * 1000;
	printf("[----------] %zu tests from %s (%.2f ms total)\n", suite_tests_ran, test_suite->name, elapsed_time);
	result->tests_ran += suite_tests_ran;
	result->suites_run++;
}

static void run_test(struct cutest_test test[static 1], const char test_suite_name[static 1]) {
	double elapsed_time;
	printf("[ RUN      ] %s.%s\n", test_suite_name, test->name);
	clock_t start_time = clock();
	test->execute();
	clock_t end_time = clock();
	elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
	printf(test->result ? "[       OK ]" : "[  FAILED  ]");
	printf(" %s.%s (%.2f ms)\n", test_suite_name, test->name, elapsed_time);
}

static void filter_tests(struct cutest cutest[static 1], size_t n, const char filter[static n]) {
	size_t suite_name_length;
	char *dot_ptr = memchr(filter, '.', n);
	suite_name_length = (dot_ptr == nullptr) ? n - 1 : (size_t) (dot_ptr - filter);
	suite_name_length = (suite_name_length > n) ? n - 1 : suite_name_length;
	for (size_t i = 0; i < cutest->size; i++) {
		struct cutest_test_suite *suite = &cutest->suite[i];
		if (strncmp(suite->name, filter, suite_name_length) != 0 || strlen(suite->name) != suite_name_length) {
			suite->enabled = false;
			cutest->enabled_tests -= suite->size;
			cutest->enabled_suites--;
			continue;
		}
		const char *required_test = (dot_ptr == nullptr) ? &filter[n - 1] : dot_ptr + 1;
		for (size_t j = 0; j < suite->size; j++) {
			struct cutest_test *test = &suite->test[j];
			if (strcmp(test->name, required_test) != 0) {
				test->enabled = false;
				suite->enabled_tests--;
				cutest->enabled_tests--;
			}
		}
	}
}
