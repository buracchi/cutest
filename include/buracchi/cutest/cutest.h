#ifndef CUTEST_LIBRARY_H
#define CUTEST_LIBRARY_H

#include <stddef.h>
#include <string.h>
#include <strings.h>

struct cutest_test {
	const char *name;
	void (*execute)();
	bool result;
};

struct cutest_test_suite {
	const char *name;
	size_t capacity;
	size_t size;
	struct cutest_test *test;
};

extern struct cutest {
	size_t total_tests;
	size_t initial_test_capacity;
	size_t capacity;
	size_t size;
	struct cutest_test_suite *suite;
} *cutest_;

extern struct cutest *cutest_init(size_t initial_suite_capacity, size_t initial_test_capacity);
extern void cutest_destroy(struct cutest *cutest);
extern struct cutest_test_suite *cutest_test_suite_add(struct cutest cutest[1], const char suite_name[static 1]);
extern struct cutest_test_suite *cutest_test_suite_get(struct cutest cutest[1], const char suite_name[static 1]);
extern struct cutest_test *cutest_test_add(struct cutest cutest[1], const char suite_name[static 1], const char test_name[static 1], void test());
extern void cutest_test_add_(const char suite_name[static 1], const char test_name[static 1], void test());
extern void cutest_test_fail_(const char test_function_name[static 8], const char file[static 1], int line, const char *fmessage, ...);

#define TEST(test_suite_name, test_name) void test_##test_suite_name##test_name();			\
	[[maybe_unused]]                                                                            	\
	[[gnu::constructor(120)]]                                                                   	\
	static void test_add_##test_suite_name##test_name() {						\
		static_assert(sizeof(#test_suite_name) > 1, "test_suite_name must not be empty");	\
		static_assert(sizeof(#test_name) > 1, "test_name must not be empty");                   \
		cutest_test_add_(#test_suite_name, #test_name, test_##test_suite_name##test_name);	\
	}                                                                                           	\
	void test_##test_suite_name##test_name()

#define ASSERT_TRUE(condition)										\
	do {												\
		if (!(condition)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected truthfulness of the expression:\n  %s\n",		\
			                  #condition);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_FALSE(condition)										\
	do {												\
		if ((condition)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected falsehood of the expression:\n  %s\n",		\
			                  #condition);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_EQ(val1, val2)										\
	do {												\
		if ((val1) != (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected equality of these values:\n  %s\n  %s\n",		\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_NE(val1, val2)										\
	do {												\
		if ((val1) == (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected inequality of these values:\n  %s\n  %s\n",		\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_LE(val1, val2)										\
	do {												\
		if ((val1) > (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected (%s) <= (%s)\n",					\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_LT(val1, val2)										\
	do {												\
		if ((val1) >= (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected (%s) < (%s)\n",					\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_GE(val1, val2)										\
	do {												\
		if ((val1) < (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected (%s) >= (%s)\n",					\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_GT(val1, val2)										\
	do {												\
		if ((val1) <= (val2)) {									\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected (%s) > (%s)\n",					\
			                  #val1,							\
			                  #val2);							\
			return;										\
		}											\
	} while (0)

#define ASSERT_STREQ(s1, s2)										\
	do {												\
		if (strcmp((s1), (s2)) != 0) {								\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                  #s1,								\
			                  #s2);								\
			return;										\
		}											\
	} while (0)

#define ASSERT_STRNE(s1, s2)										\
	do {												\
		if (strcmp((s1), (s2)) == 0) {								\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                  #s1,								\
			                  #s2);								\
			return;										\
		}											\
	} while (0)

#define ASSERT_STRCASE_EQ(s1, s2)									\
	do {												\
		if (strcasecmp((s1), (s2)) != 0) {							\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                  #s1,								\
			                  #s2);								\
			return;										\
		}											\
	} while (0)

#define ASSERT_STRCASE_NE(s1, s2)									\
	do {												\
		if (strcasecmp((s1), (s2)) == 0) {							\
			cutest_test_fail_(__func__,							\
			                  __FILE__,							\
			                  __LINE__,							\
			                  "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                  #s1,								\
			                  #s2);								\
			return;										\
		}											\
	} while (0)

#define ASSERT_FLOAT_EQ(val1, val2) // TODO
#define ASSERT_DOUBLE_EQ(val1, val2) // TODO
#define ASSERT_NEAR(val1, val2, abs_error) // TODO

#endif //CUTEST_LIBRARY_H
