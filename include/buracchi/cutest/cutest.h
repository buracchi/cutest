#ifndef CUTEST_LIBRARY_H
#define CUTEST_LIBRARY_H

#include <string.h>
#include <strings.h>

struct test_suite {
	const char *name;
	struct test_info *test_list;
	struct test_suite *next;
};

struct test_info {
	const char *name;
	void (*test)();
	bool result;
	struct test_info *next;
};

extern struct test_suite *cutest_test_suite_list;

extern void cutest_register_test(const char *test_suite_name, const char *test_name, void (*test)());
extern void cutest_fail_test(const char *test_function_name, const char *file, int line, const char *fmessage, ...);

#define TEST(test_suite_name, test_name) void test_##test_suite_name##test_name();			\
	[[maybe_unused]]                                                                            	\
	[[gnu::constructor(110)]]                                                                   	\
	static void register_##test_suite_name##test_name() {						\
		static_assert(sizeof(#test_suite_name) > 1, "test_suite_name must not be empty");	\
		static_assert(sizeof(#test_name) > 1, "test_name must not be empty");                   \
		cutest_register_test(#test_suite_name, #test_name, test_##test_suite_name##test_name);	\
	}                                                                                           	\
	void test_##test_suite_name##test_name()

#define ASSERT_TRUE(condition)									\
	do {											\
		if (!(condition)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected truthfulness of the expression:\n  %s\n",	\
			                 #condition);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_FALSE(condition)									\
	do {											\
		if ((condition)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected falsehood of the expression:\n  %s\n",	\
			                 #condition);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_EQ(val1, val2)									\
	do {											\
		if ((val1) != (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected equality of these values:\n  %s\n  %s\n",	\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_NE(val1, val2)									\
	do {											\
		if ((val1) == (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected inequality of these values:\n  %s\n  %s\n",	\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_LE(val1, val2)									\
	do {											\
		if ((val1) > (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected (%s) <= (%s)\n",				\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_LT(val1, val2)									\
	do {											\
		if ((val1) >= (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected (%s) < (%s)\n",				\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_GE(val1, val2)									\
	do {											\
		if ((val1) < (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected (%s) >= (%s)\n",				\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_GT(val1, val2)									\
	do {											\
		if ((val1) <= (val2)) {								\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected (%s) > (%s)\n",				\
			                 #val1,							\
			                 #val2);						\
			return;									\
		}										\
	} while (0)

#define ASSERT_STREQ(s1, s2)									\
	do {											\
		if (strcmp((s1), (s2)) != 0) {							\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                 #s1,							\
			                 #s2);							\
			return;									\
		}										\
	} while (0)

#define ASSERT_STRNE(s1, s2)									\
	do {											\
		if (strcmp((s1), (s2)) == 0) {							\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                 #s1,							\
			                 #s2);							\
			return;									\
		}										\
	} while (0)

#define ASSERT_STRCASE_EQ(s1, s2)								\
	do {											\
		if (strcasecmp((s1), (s2)) != 0) {						\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                 #s1,							\
			                 #s2);							\
			return;									\
		}										\
	} while (0)

#define ASSERT_STRCASE_NE(s1, s2)								\
	do {											\
		if (strcasecmp((s1), (s2)) == 0) {						\
			cutest_fail_test(__func__,						\
			                 __FILE__,						\
			                 __LINE__,						\
			                 "Expected inequality of these strings:\n  %s\n  %s\n",	\
			                 #s1,							\
			                 #s2);							\
			return;									\
		}										\
	} while (0)

#define ASSERT_FLOAT_EQ(val1, val2) // TODO
#define ASSERT_DOUBLE_EQ(val1, val2) // TODO
#define ASSERT_NEAR(val1, val2, abs_error) // TODO

#endif //CUTEST_LIBRARY_H
