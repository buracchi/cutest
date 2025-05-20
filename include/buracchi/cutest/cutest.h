#ifndef CUTEST_H
#define CUTEST_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

struct cutest_test {
	const char *name;
	void (*execute)();
	bool result;
	bool enabled;
};

struct cutest_test_suite {
	const char *name;
	size_t capacity;
	size_t size;
	struct cutest_test *test;
	bool enabled;
	size_t enabled_tests;
};

extern struct cutest {
	size_t total_tests;
	size_t initial_test_capacity;
	size_t capacity;
	size_t size;
	struct cutest_test_suite *suite;
	size_t enabled_suites;
	size_t enabled_tests;
} *cutest_;

extern struct cutest *cutest_init(size_t initial_suite_capacity, size_t initial_test_capacity);
[[gnu::nonnull(1)]]
extern void cutest_destroy(struct cutest *cutest);
extern struct cutest_test_suite *cutest_test_suite_add(struct cutest cutest[static 1], const char suite_name[static 1]);
extern struct cutest_test_suite *cutest_test_suite_get(struct cutest cutest[static 1], const char suite_name[static 1]);
[[gnu::nonnull(4)]]
extern struct cutest_test *cutest_test_add(struct cutest cutest[static 1],
                                           const char suite_name[static 1],
                                           const char test_name[static 1],
                                           void test());

[[gnu::nonnull(3)]]
extern void cutest_test_add_(const char suite_name[static 1], const char test_name[static 1], void test());
[[gnu::format(printf, 4, 5)]]
extern void cutest_test_fail_(const char test_function_name[static 8],
                              const char file[static 1],
                              int line,
                              const char *fmessage,
                              ...);

#define TEST(test_suite_name, test_name) void test_##test_suite_name##test_name();         \
    [[maybe_unused]]                                                                       \
    [[gnu::constructor(120)]]                                                              \
    static void test_add_##test_suite_name##test_name() {                                  \
        static_assert(sizeof(#test_suite_name) > 1, "test_suite_name must not be empty");  \
        static_assert(sizeof(#test_name) > 1, "test_name must not be empty");              \
        cutest_test_add_(#test_suite_name, #test_name, test_##test_suite_name##test_name); \
    }                                                                                      \
    void test_##test_suite_name##test_name()

#define CUTEST_PREDICATE(is_fatal, predicate, REPORT_FAILURE) \
    do {                                                      \
        if (!(predicate)) {                                   \
            REPORT_FAILURE;                                   \
            if (is_fatal) {                                   \
                return;                                       \
            }                                                 \
        }                                                     \
    } while (0)

#define CUTEST_COND(is_fatal, condition, fmt, expr, ...)                \
    CUTEST_PREDICATE(is_fatal, condition,                               \
        do {                                                            \
            cutest_test_fail_(__func__, __FILE__, __LINE__, fmt, expr); \
            __VA_OPT__(fprintf(stderr, __VA_ARGS__);)                   \
            __VA_OPT__(fprintf(stderr, "\n");)                          \
        } while(0)                                                      \
    )

#define CUTEST_COMP(is_fatal, comparison, fmt, expr1, expr2, ...)               \
    CUTEST_PREDICATE(is_fatal, comparison,                                      \
        do {                                                                    \
            cutest_test_fail_(__func__, __FILE__, __LINE__, fmt, expr1, expr2); \
            __VA_OPT__(fprintf(stderr, __VA_ARGS__);)                           \
            __VA_OPT__(fprintf(stderr, "\n");)                                  \
        } while(0)                                                              \
    )

#define CUTEST_COND_TRUE(is_fatal, condition, ...) \
    CUTEST_COND(is_fatal, (condition), "Expected truthfulness of the expression:\n  %s", #condition, __VA_ARGS__)
#define CUTEST_COND_FALSE(is_fatal, condition, ...) \
    CUTEST_COND(is_fatal, !(condition), "Expected falsehood of the expression:\n  %s", #condition, __VA_ARGS__)
#define CUTEST_COMP_EQ(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) == (val2)), "Expected equality of these values:\n  %s\n  %s", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_NE(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) != (val2)), "Expected inequality of these values:\n  %s\n  %s", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_LE(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) <= (val2)), "Expected (%s) <= (%s).", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_LT(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) < (val2)), "Expected (%s) < (%s).", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_GE(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) >= (val2)), "Expected (%s) >= (%s).", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_GT(is_fatal, val1, val2, ...) \
    CUTEST_COMP(is_fatal, ((val1) > (val2)), "Expected (%s) > (%s).", #val1, #val2, __VA_ARGS__)
#define CUTEST_COMP_STREQ(is_fatal, s1, s2, ...) \
    CUTEST_COMP(is_fatal, (strcmp(s1, s2) == 0), "Expected equality of these strings:\n  %s\n  %s", #s1, #s2, __VA_ARGS__)
#define CUTEST_COMP_STRNE(is_fatal, s1, s2, ...) \
    CUTEST_COMP(is_fatal, (strcmp(s1, s2) != 0), "Expected inequality of these strings:\n  %s\n  %s", #s1, #s2, __VA_ARGS__)
#define CUTEST_COMP_STRCASE_EQ(is_fatal, s1, s2, ...) \
    CUTEST_COMP(is_fatal, (strcasecmp(s1, s2) == 0), "Expected case-insensitive equality of these strings:\n  %s\n  %s", #s1, #s2, __VA_ARGS__)
#define CUTEST_COMP_STRCASE_NE(is_fatal, s1, s2, ...) \
    CUTEST_COMP(is_fatal, (strcasecmp(s1, s2) != 0), "Expected case-insensitive inequality of these strings:\n  %s\n  %s", #s1, #s2, __VA_ARGS__)

#define EXPECT_TRUE(condition, ...) CUTEST_COND_TRUE(false, condition, __VA_ARGS__)
#define EXPECT_FALSE(condition, ...) CUTEST_COND_FALSE(false, condition, __VA_ARGS__)
#define EXPECT_EQ(val1, val2, ...) CUTEST_COMP_EQ(false, val1, val2, __VA_ARGS__)
#define EXPECT_NE(val1, val2, ...) CUTEST_COMP_NE(false, val1, val2, __VA_ARGS__)
#define EXPECT_LE(val1, val2, ...) CUTEST_COMP_LE(false, val1, val2, __VA_ARGS__)
#define EXPECT_LT(val1, val2, ...) CUTEST_COMP_LT(false, val1, val2, __VA_ARGS__)
#define EXPECT_GE(val1, val2, ...) CUTEST_COMP_GE(false, val1, val2, __VA_ARGS__)
#define EXPECT_GT(val1, val2, ...) CUTEST_COMP_GT(false, val1, val2, __VA_ARGS__)
#define EXPECT_STREQ(s1, s2, ...) CUTEST_COMP_STREQ(false, s1, s2, __VA_ARGS__)
#define EXPECT_STRNE(s1, s2, ...) CUTEST_COMP_STRNE(false, s1, s2, __VA_ARGS__)
#define EXPECT_STRCASE_EQ(s1, s2, ...) CUTEST_COMP_STRCASE_EQ(false, s1, s2, __VA_ARGS__)
#define EXPECT_STRCASE_NE(s1, s2, ...) CUTEST_COMP_STRCASE_NE(false, s1, s2, __VA_ARGS__)

#define EXPECT_FLOAT_EQ(val1, val2, ...) // TODO
#define EXPECT_DOUBLE_EQ(val1, val2, ...) // TODO
#define EXPECT_NEAR(val1, val2, abs_error, ...) // TODO

#define ASSERT_TRUE(condition, ...) CUTEST_COND_TRUE(true, condition, __VA_ARGS__)
#define ASSERT_FALSE(condition, ...) CUTEST_COND_FALSE(true, condition, __VA_ARGS__)
#define ASSERT_EQ(val1, val2, ...) CUTEST_COMP_EQ(true, val1, val2, __VA_ARGS__)
#define ASSERT_NE(val1, val2, ...) CUTEST_COMP_NE(true, val1, val2, __VA_ARGS__)
#define ASSERT_LE(val1, val2, ...) CUTEST_COMP_LE(true, val1, val2, __VA_ARGS__)
#define ASSERT_LT(val1, val2, ...) CUTEST_COMP_LT(true, val1, val2, __VA_ARGS__)
#define ASSERT_GE(val1, val2, ...) CUTEST_COMP_GE(true, val1, val2, __VA_ARGS__)
#define ASSERT_GT(val1, val2, ...) CUTEST_COMP_GT(true, val1, val2, __VA_ARGS__)
#define ASSERT_STREQ(s1, s2, ...) CUTEST_COMP_STREQ(true, s1, s2, __VA_ARGS__)
#define ASSERT_STRNE(s1, s2, ...) CUTEST_COMP_STRNE(true, s1, s2, __VA_ARGS__)
#define ASSERT_STRCASE_EQ(s1, s2, ...) CUTEST_COMP_STRCASE_EQ(true, s1, s2, __VA_ARGS__)
#define ASSERT_STRCASE_NE(s1, s2, ...) CUTEST_COMP_STRCASE_NE(true, s1, s2, __VA_ARGS__)

#define ASSERT_FLOAT_EQ(val1, val2, ...) // TODO
#define ASSERT_DOUBLE_EQ(val1, val2, ...) // TODO
#define ASSERT_NEAR(val1, val2, abs_error, ...) // TODO

#endif //CUTEST_H
