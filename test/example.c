#include <buracchi/cutest/cutest.h>

TEST(test_suite_name, test_name) {
	ASSERT_EQ(1 + 1, 2);
}

TEST(test_suite_name, another_test_name) {
	ASSERT_EQ(3 - 1, 2);
}

TEST(another_test_suite_name, test_name) {
	ASSERT_EQ(2, 2);
}
