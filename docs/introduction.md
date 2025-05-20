# Introduction

## Why CuTest?

*CuTest* helps you write better C tests.

CuTest is a unit testing framework for the C programming language, based on the 
xUnit architecture and heavily influenced by GoogleTest. It is designed to 
provide a similar developer experience to GoogleTest, but for C, not C++. 
Whether you work on Linux, Windows, or Mac, if you write C code, CuTest can 
help you. It supports *any* kind of tests, not just unit tests.

So what makes a good test, and how does CuTest fit in? We believe:

1.  Tests should be *independent* and *repeatable*. CuTest isolates the tests 
    by running each of them independently. When a test fails, CuTest allows you 
    to run it in isolation for quick debugging.
2.  Tests should be well *organized* and reflect the structure of the tested 
    code. CuTest groups related tests into test suites that can share data and
    subroutines. This makes tests easy to maintain and understand.
3.  Tests should be *portable* and *reusable*. CuTest works on different OSes 
    and compilers, so your tests can work with a variety of configurations.
4.  When tests fail, they should provide as much *information* about the 
    problem as possible. CuTest doesn't stop at the first test failure. 
    Instead, it only stops the current test and continues with the next. 
    You can also set up tests that report non-fatal failures after which the 
    current test continues. Thus, you can detect and fix multiple bugs in a 
    single run-edit-compile cycle.
5.  The testing framework should liberate test writers from housekeeping chores 
    and let them focus on the test *content*. CuTest automatically keeps track 
    of all tests defined, and doesn't require the user to enumerate them in 
    order to run them.
6.  Tests should be *fast*. With CuTest, you can reuse shared resources across 
    tests and pay for the set-up/tear-down only once, without making tests 
    depend on each other.

Since CuTest is based on the popular xUnit architecture, you'll feel right at 
home if you've used GoogleTest, JUnit, or PyUnit before. If not, it will take 
you about 10 minutes to learn the basics and get started.

## Basic Concepts

When using CuTest, you start by writing *assertions*, which are statements 
that check whether a condition is true. An assertion's result can be *success*, 
*nonfatal failure*, or *fatal failure*. If a fatal failure occurs, it aborts 
the current function; otherwise the program continues normally.

*Tests* use assertions to verify the tested code's behavior. If a test crashes 
or has a failed assertion, then it *fails*; otherwise it *succeeds*.

A *test suite* contains one or many tests. You should group your tests into 
test suites that reflect the structure of the tested code.
As of now, there is no concept of *test fixture* to allow multiple tests in a 
test suite to share common resources.

A *test program* can contain multiple test suites.

## Assertions

CuTest assertions are macros that resemble function calls. You test a function 
by making assertions about its behavior. When an assertion fails, CuTest prints 
the assertion's source file and line number location, along with a failure 
message. You may also supply a custom failure message which will be appended 
to CuTest's message.

The assertions come in pairs that test the same thing but have different 
effects on the current function. `ASSERT_*` versions generate fatal failures 
when they fail, and abort the current function. `EXPECT_*` versions generate 
nonfatal failures, which don't abort the current function. Usually `EXPECT_*` 
are preferred, as they allow more than one failure to be reported in a test. 
However, you should use `ASSERT_*` if it doesn't make sense to continue when 
the assertion in question fails.

Since a failed `ASSERT_*` returns from the current function immediately, 
possibly skipping clean-up code that comes after it, it may cause a resource 
leak. Depending on the nature of the leak, it may or may not be worth fixing - 
so keep this in mind if you get a heap checker error in addition to assertion 
errors.

To provide a custom failure message, you can use optional variadic parameters 
of the macros. See the following example, using the `ASSERT_EQ` and `EXPECT_EQ` 
macros to verify value equality:

```c
ASSERT_EQ(strlen(s1), strlen(s2), "Strings s1 and s2 are of unequal length");

for (int i = 0; i < strlen(s1); ++i) {
  EXPECT_EQ(s1[i], s2[i], "Strings s1 and s2 differ at index %d", i);
}
```

CuTest provides a collection of assertions for verifying the behavior of 
your code in various ways. You can check Boolean conditions, compare values 
based on relational operators, verify string values, floating-point values, and 
much more. There are even assertions that enable you to verify more complex 
states by providing custom predicates. For the complete list of assertions 
provided by CuTest, see the [Assertions](assertions.md) documentation.

## Simple Tests

To create a test:

1.  Use the `TEST()` macro to define and name a test function. These are 
    ordinary C functions that don't return a value.
2.  In this function, along with any valid C statements you want to include, 
    use the various CuTest assertions to check values.
3.  The test's result is determined by the assertions; if any assertion in the 
    test fails or if the test crashes, the entire test fails. Otherwise, it 
    succeeds.

```c
TEST(test_suite_name, test_name) {
  // test body ...
}
```

`TEST()` arguments go from general to specific. The *first* argument is the 
name of the test suite, and the *second* argument is the test's name within the 
test suite. Both names must be valid C identifiers. A test's *full name* 
consists of its containing test suite and its individual name. Tests from 
different test suites can have the same individual name.

For example, let's take a simple integer function:

```c
int factorial(int n);  // Returns the factorial of n
```

A test suite for this function might look like:

```c
// Tests factorial of 0.
TEST(factorial, handles_zero_input) {
  EXPECT_EQ(factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(factorial, handles_positive_input) {
  EXPECT_EQ(factorial(1), 1);
  EXPECT_EQ(factorial(2), 2);
  EXPECT_EQ(factorial(3), 6);
  EXPECT_EQ(factorial(8), 40320);
}
```

CuTest groups the test results by test suites, so logically related tests 
should be in the same test suite; in other words, the first argument to their 
`TEST()` should be the same. In the above example, we have two tests, 
`handles_zero_input` and `handles_positive_input`, that belong to the same test 
suite `factorial`.

## Invoking the Tests

`TEST()` implicitly registers your tests with CuTest. So, unlike with many 
other C testing frameworks, you don't have to re-list all your defined tests in 
order to run them.

After defining your tests, you can run them by linking with the `cutest_main` 
library, which provides a suitable `main()` function. This function will 
automatically discover and run all registered tests.

If you want to write your own `main` function (for custom setup or teardown), 
you can do so by calling the CuTest API directly.
