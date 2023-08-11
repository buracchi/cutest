#include <buracchi/cutest/cutest.h>

struct test_suite *test_suite_list_ = NULL;

static void order_test_list(struct test_info **test_list);
static void order_test_suite_list(void);

extern struct test_suite *find_or_create_test_suite(const char *suite_name) {
    struct test_suite *current = test_suite_list_;
    while (current != NULL) {
        if (strcmp(current->name, suite_name) == 0) {
            return current;
        }
        current = current->next;
    }
    struct test_suite *new_suite;
    new_suite = malloc(sizeof *new_suite);
    new_suite->name = suite_name;
    new_suite->test_list = NULL;
    new_suite->next = test_suite_list_;
    test_suite_list_ = new_suite;
    return new_suite;
}

extern void register_test(const char *test_suite_name, const char *test_name, int (*test)(void)) {
    struct test_suite *suite = find_or_create_test_suite(test_suite_name);
    struct test_info *info = malloc(sizeof *info);
    info->name = test_name;
    info->test = test;
    info->next = suite->test_list;
    suite->test_list = info;
}

__attribute__((constructor(120)))
static void order_test_suite_list(void) {
    struct test_suite *prev = NULL;
    struct test_suite *current = test_suite_list_;
    struct test_suite *next = NULL;
    while (current != NULL) {
        order_test_list(&current->test_list);
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    test_suite_list_ = prev;
}

static void order_test_list(struct test_info **test_list) {
    struct test_info *prev = NULL;
    struct test_info *current = *test_list;
    struct test_info *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    *test_list = prev;
}
