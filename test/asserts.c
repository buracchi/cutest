#include <buracchi/cutest/cutest.h>

TEST(asserts, true) {
	ASSERT_TRUE(true);
}

TEST(asserts, false) {
	ASSERT_FALSE(false);
}

TEST(asserts, equality) {
	ASSERT_EQ(1, 1);
}

TEST(asserts, inequality) {
	ASSERT_NE(1, 0);
}

TEST(asserts, less_than_or_equal_to) {
	ASSERT_LE(0, 1);
}

TEST(asserts, less_than) {
	ASSERT_LT(0, 1);
}

TEST(asserts, greater_than_or_equal_to) {
	ASSERT_GE(1, 0);
}

TEST(asserts, greater_than) {
	ASSERT_GT(1, 0);
}

TEST(asserts, string_equality) {
	ASSERT_STREQ("test_string", "test_string");
}

TEST(asserts, string_inequality) {
	ASSERT_STRNE("test_string", "another_string");
}

TEST(asserts, string_case_insensitive_equality) {
	ASSERT_STRCASE_EQ("test_string", "TEST_STRING");
}

TEST(asserts, string_case_insensitive_inequality) {
	ASSERT_STRCASE_NE("test_string", "ANOTHER_STRING");
}
