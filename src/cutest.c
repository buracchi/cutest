#include <buracchi/cutest/cutest.h>

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cutest *cutest_ = nullptr;

static struct cutest_test *find_test(const char test_function_name[static 8]);

[[maybe_unused]]
[[gnu::constructor(110)]]
static void cutest_init_() {
	cutest_ = cutest_init(1, 1);
	if (cutest_ == nullptr) {
		perror(strerror(errno));
		exit(1);
	}
}

extern void cutest_test_add_(const char suite_name[static 1], const char test_name[static 1], void test()) {
	if (cutest_test_add(cutest_, suite_name, test_name, test) == nullptr) {
		perror(strerror(errno));
		exit(1);
	}
}

extern struct cutest *cutest_init(size_t initial_suite_capacity, size_t initial_test_capacity) {
	assert((initial_suite_capacity > 0) && "Initial suite capacity must be greater than zero.");
	assert((initial_test_capacity > 0) && "Initial test capacity must be greater than zero.");
	if (!initial_test_capacity || !initial_suite_capacity) {
		return nullptr;
	}
	struct cutest *cutest = malloc(sizeof *cutest);
	if (cutest == nullptr) {
		return nullptr;
	}
	*cutest = (struct cutest) {
		.initial_test_capacity = initial_test_capacity,
		.capacity = initial_suite_capacity,
		.suite = malloc(initial_suite_capacity * sizeof(struct cutest_test_suite)),
	};
	if (cutest->suite == nullptr) {
		free(cutest);
		return nullptr;
	}
	return cutest;
}

extern void cutest_destroy(struct cutest *cutest) {
	for (size_t i = 0; i < cutest->size; i++) {
		free(cutest->suite[i].test);
	}
	free(cutest->suite);
	free(cutest);
}

extern struct cutest_test_suite *cutest_test_suite_add(struct cutest cutest[1], const char suite_name[static 1]) {
	if (cutest->capacity == cutest->size) {
		cutest->capacity *= 2;
		size_t new_size = cutest->capacity * sizeof(struct cutest_test_suite);
		struct cutest_test_suite *ptr = realloc(cutest->suite, new_size);
		if (ptr == nullptr) {
			cutest->capacity /= 2;
			return nullptr;
		}
		cutest->suite = ptr;
	}
	cutest->suite[cutest->size] = (struct cutest_test_suite) {
		.name = suite_name,
		.test = malloc(cutest->initial_test_capacity * sizeof(struct cutest_test)),
		.size = 0,
		.capacity = cutest->initial_test_capacity,
		.enabled = true,
	};
	if (cutest->suite[cutest->size].test == nullptr) {
		return nullptr;
	}
	cutest->size++;
	cutest->enabled_suites++;
	return &cutest->suite[cutest->size - 1];
}

extern struct cutest_test_suite *cutest_test_suite_get(struct cutest cutest[1], const char suite_name[static 1]) {
	for (size_t i = 0; i < cutest->size; i++) {
		if (strcmp(cutest->suite[i].name, suite_name) == 0) {
			return &cutest->suite[i];
		}
	}
	return nullptr;
}

extern struct cutest_test *cutest_test_add(struct cutest cutest[1],
                                           const char suite_name[static 1],
                                           const char test_name[static 1],
                                           void test()) {
	struct cutest_test_suite *suite = cutest_test_suite_get(cutest, suite_name);
	if (suite == nullptr) {
		suite = cutest_test_suite_add(cutest, suite_name);
	}
	if (suite == nullptr) {
		return nullptr;
	}
	if (suite->capacity == suite->size) {
		suite->capacity *= 2;
		size_t new_size = suite->capacity * sizeof(struct cutest_test);
		struct cutest_test *ptr = realloc(suite->test, new_size);
		if (ptr == nullptr) {
			suite->capacity /= 2;
			return nullptr;
		}
		suite->test = ptr;
	}
	suite->test[suite->size] = (struct cutest_test) {
		.name = test_name,
		.execute = test,
		.result = true,
		.enabled = true,
	};
	suite->size++;
	suite->enabled_tests++;
	cutest->total_tests++;
	cutest->enabled_tests++;
	return &suite->test[suite->size - 1];
}

extern void cutest_test_fail_(const char test_function_name[static 8],
                              const char file[static 1],
                              int line,
                              const char *fmessage, ...) {
	struct cutest_test *test;
	va_list args;
	va_start(args, fmessage);
	test = find_test(test_function_name);
	assert((test != nullptr) && "CuTest assert macros must be called within a test.");
	if (test != nullptr) {
		test->result = false;
		fprintf(stderr, "%s:%d: Failure\n", file, line);
		vfprintf(stderr, fmessage, args);
	}
	va_end(args);
}

static struct cutest_test *find_test(const char test_function_name[static 8]) {
	for (size_t i = 0; i < cutest_->size; i++) {
		struct cutest_test_suite *suite = &cutest_->suite[i];
		const char *tfn_ptr = test_function_name + sizeof("test");
		if (strncmp(tfn_ptr, suite->name, strlen(suite->name)) != 0) {
			continue;
		}
		tfn_ptr += strlen(suite->name);
		for (size_t j = 0; j < suite->size; j++) {
			if (strcmp(tfn_ptr, suite->test[j].name) == 0) {
				return &suite->test[j];
			}
		}
	}
	return nullptr;
}
