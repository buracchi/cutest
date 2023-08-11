#ifndef CUTEST_LIBRARY_H
#define CUTEST_LIBRARY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct test_suite {
    const char *name;
    struct test_info *test_list;
    struct test_suite *next;
};

struct test_info {
    const char *name;
    
    int (*test)(void);
    
    struct test_info *next;
};

extern struct test_suite *test_suite_list_;

extern struct test_suite *find_or_create_test_suite(const char *suite_name);
extern void register_test(const char *test_suite_name, const char *test_name, int (*test)(void));

#define TEST(test_suite_name, test_name) int test_##test_suite_name##test_name(void);           \
    __attribute__((constructor(110))) static void register_##test_suite_name##test_name(void) { \
        _Static_assert(sizeof(#test_suite_name) > 1, "test_suite_name must not be empty");      \
        _Static_assert(sizeof(#test_name) > 1, "test_name must not be empty");                  \
        register_test(#test_suite_name, #test_name, test_##test_suite_name##test_name);         \
    }                                                                                           \
    int test_##test_suite_name##test_name(void)

#define ASSERT_EQ(val1, val2)                                           \
    do {                                                                \
        if ((val1) != (val2)) {                                         \
            fprintf(stderr, "%s:%d: Failure\n", __FILE__, __LINE__);    \
            fprintf(stderr, "Expected equality of these values:\n");    \
            fprintf(stderr, "  %s\n", #val1);                           \
            fprintf(stderr, "  %s\n", #val2);                           \
            return EXIT_FAILURE;                                        \
        }                                                               \
        else {                                                          \
            return EXIT_SUCCESS;                                        \
        }                                                               \
    } while (0)

#endif //CUTEST_LIBRARY_H
