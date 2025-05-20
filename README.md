# CuTest

CuTest is a lightweight unit testing framework for the C programming language,
following the xUnit architecture and drawing strong inspiration from GoogleTest.

C is *not* a subset of C++, and nontrivial C headers typically will not compile
cleanly in C++ environments.\
While C++ testing frameworks may offer a smoother developer experience, applying
them to C code often feels like forcing a square peg into a round hole.

CuTest is purpose-built for modern C, providing functionality and ease of use 
comparable to GoogleTest, but without sacrificing C compatibility.

CuTest requires a C23 transpiler or a 
[ISO/IEC 9899:2024 compliant compiler](https://en.cppreference.com/w/c/compiler_support/23).

## Features

- **xUnit test framework**: CuTest is based on the xUnit testing framework, a 
popular architecture for unit testing
- **Test discovery**: CuTest automatically discovers and runs your tests, 
eliminating the need to manually register your tests
- **Various options for running tests**: CuTest provides many options for 
running tests including running individual tests.

## Get started with CMake

CuTest includes a CMake build script that works across a wide range of platforms.

Start by creating a directory for your project:
```bash
mkdir my_project
cd my_project
```
Then, create a file named `hello_test.c` in the project directory with the
following content:
```c
#include <buracchi/cutest/cutest.h>

// Demonstrate some basic assertions.
TEST(hello_test, basic_assertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
```
Next, create a `CMakeLists.txt` file in the same directory to declare a
dependency on CuTest and compile the source file into an executable.
There are many ways to do so, but we will use CMake’s FetchContent module in
this example:
```cmake
cmake_minimum_required(VERSION 3.31)
project(my_project)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        buracchi-cutest 
        GIT_REPOSITORY https://github.com/buracchi/cutest.git 
        GIT_TAG        main # use the latest commit hash for production code
)
FetchContent_MakeAvailable(buracchi-cutest)

add_executable(hello_test "hello_test.c")
target_link_libraries(hello_test buracchi::cutest::cutest_main)

cutest_discover_tests(hello_test)
```
You can now build and run the test with:
```bash
cmake -S . -B build && cmake --build build && cd build && ctest
```

## Documentation

For a further overview of CuTest, refer to the 
[introduction](docs/introduction.md) in the documentation folder.
