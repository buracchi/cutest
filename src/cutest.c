#include <buracchi/cutest/cutest.h>

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test_suite *cutest_test_suite_list = nullptr;

static struct test_info *find_test_info(const char *test_function_name);
static struct test_suite *create_test_suite(const char *suite_name);
static struct test_suite *find_test_suite(const char *suite_name);
static void order_test_list(struct test_info **test_list);
static void order_test_suite_list();

extern void cutest_register_test(const char *test_suite_name, const char *test_name, void (*test)()) {
	struct test_suite *suite;
	struct test_info *info;
	suite = find_test_suite(test_suite_name);
	if (suite == nullptr) {
		suite = create_test_suite(test_suite_name);
	}
	info = malloc(sizeof *info);
	info->name = test_name;
	info->test = test;
	info->result = true;
	info->next = suite->test_list;
	suite->test_list = info;
}

extern void cutest_fail_test(const char *test_function_name, const char *file, int line, const char *fmessage, ...) {
	struct test_info *test_info = nullptr;
	va_list args;
	va_start(args, fmessage);
	test_info = find_test_info(test_function_name);
	assert((test_info != nullptr) && "CuTest assert macros must be called within a test.");
	test_info->result = false;
	fprintf(stderr, "%s:%d: Failure\n", file, line);
	vfprintf(stderr, fmessage, args);
	va_end(args);
}

static struct test_info *find_test_info(const char *test_function_name) {
	for(struct test_suite *suite = cutest_test_suite_list; suite != nullptr; suite = suite->next) {
		const char *tf_name_ptr = test_function_name + sizeof("test");
		if (strncmp(tf_name_ptr, suite->name, strlen(suite->name)) != 0) {
			continue;
		}
		tf_name_ptr += strlen(suite->name);
		for(struct test_info *info = suite->test_list; info != nullptr; info = info->next) {
			if (strcmp(tf_name_ptr, info->name) == 0) {
				return info;
			}
		}
	}
	return nullptr;
}

static struct test_suite *create_test_suite(const char *suite_name) {
    struct test_suite *new_suite;
    new_suite = malloc(sizeof *new_suite);
    new_suite->name = suite_name;
    new_suite->test_list = nullptr;
    new_suite->next = cutest_test_suite_list;
    cutest_test_suite_list = new_suite;
    return new_suite;
}

static struct test_suite *find_test_suite(const char *suite_name) {
	struct test_suite *current = cutest_test_suite_list;
	while (current != nullptr) {
		if (strcmp(current->name, suite_name) == 0) {
			return current;
		}
		current = current->next;
	}
	return nullptr;
}

[[maybe_unused]]
[[gnu::constructor(120)]]
static void order_test_suite_list() {
    struct test_suite *prev = nullptr;
    struct test_suite *current = cutest_test_suite_list;
    struct test_suite *next = nullptr;
    while (current != nullptr) {
        order_test_list(&current->test_list);
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    cutest_test_suite_list = prev;
}

static void order_test_list(struct test_info **test_list) {
    struct test_info *prev = nullptr;
    struct test_info *current = *test_list;
    struct test_info *next = nullptr;
    while (current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    *test_list = prev;
}
