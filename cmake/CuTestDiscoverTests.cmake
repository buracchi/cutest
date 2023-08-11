#[=======================================================================[.rst:
CuTest
----------

This module defines functions to help use the CuTest infrastructure.
A mechanisms for adding tests, aka :command:`cutest_discover_tests` is provided
via ``find_package(buracchi-cutest)``.

The :command:`cutest_discover_tests` discovers tests by asking the compiled test
executable to enumerate its tests.
This does not require CMake to be re-run when tests change.
However, it may not work in a cross-compiling environment, and setting test
properties is less convenient.

This command is intended to replace use of :command:`add_test` to register
tests, and it will create a separate CTest test for each CuTest test case.
Note that this is in some cases less efficient, as common set-up and tear-down
logic cannot be shared by multiple test cases executing in the same instance.
However, it provides more fine-grained pass/fail information to CTest, which is
usually considered as more beneficial.  By default, the CTest test name is the
same as the CuTest name (i.e. ``suite.testcase``); see also
``TEST_PREFIX`` and ``TEST_SUFFIX``.

.. command:: cutest_discover_tests

  Automatically add tests with CTest by querying the compiled test executable
  for available tests::

    cutest_discover_tests(target
                          [EXTRA_ARGS arg1...]
                          [WORKING_DIRECTORY dir]
                          [TEST_PREFIX prefix]
                          [TEST_SUFFIX suffix]
                          [TEST_FILTER expr]
                          [NO_PRETTY_TYPES] [NO_PRETTY_VALUES]
                          [PROPERTIES name1 value1...]
                          [TEST_LIST var]
                          [DISCOVERY_TIMEOUT seconds]
                          [XML_OUTPUT_DIR dir]
                          [DISCOVERY_MODE <POST_BUILD|PRE_TEST>]
    )

  ``cutest_discover_tests()`` sets up a post-build command on the test executable
  that generates the list of tests by parsing the output from running the test
  with the ``--cutest_list_tests`` argument.
  Since test discovery occurs at build time, it is not necessary to re-run
  CMake when the list of tests changes.
  However, it requires that :prop_tgt:`CROSSCOMPILING_EMULATOR` is properly set
  in order to function in a cross-compiling environment.

  Additionally, setting properties on tests is somewhat less convenient, since
  the tests are not available at CMake time.  Additional test properties may be
  assigned to the set of tests as a whole using the ``PROPERTIES`` option.  If
  more fine-grained test control is needed, custom content may be provided
  through an external CTest script using the :prop_dir:`TEST_INCLUDE_FILES`
  directory property.  The set of discovered tests is made accessible to such a
  script via the ``<target>_TESTS`` variable.

  The options are:

  ``target``
    Specifies the CuTest executable, which must be a known CMake
    executable target.  CMake will substitute the location of the built
    executable when running the test.

  ``EXTRA_ARGS arg1...``
    Any extra arguments to pass on the command line to each test case.

  ``WORKING_DIRECTORY dir``
    Specifies the directory in which to run the discovered test cases.  If this
    option is not provided, the current binary directory is used.

  ``TEST_PREFIX prefix``
    Specifies a ``prefix`` to be prepended to the name of each discovered test
    case.  This can be useful when the same test executable is being used in
    multiple calls to ``cutest_discover_tests()`` but with different ``EXTRA_ARGS``.

  ``TEST_SUFFIX suffix``
    Similar to ``TEST_PREFIX`` except the ``suffix`` is appended to the name of
    every discovered test case.  Both ``TEST_PREFIX`` and ``TEST_SUFFIX`` may
    be specified.

  ``TEST_FILTER expr``
    Filter expression to pass as a ``--cutest_filter`` argument during test
    discovery.  Note that the expression is a wildcard-based format that
    matches against the original test names as used by the test.  For type or
    value-parameterized tests, these names may be different to the potentially
    pretty-printed test names that :program:`ctest` uses.

  ``NO_PRETTY_TYPES``
    By default, the type index of type-parameterized tests is replaced by the
    actual type name in the CTest test name.  If this behavior is undesirable
    (e.g. because the type names are unwieldy), this option will suppress this
    behavior.

  ``NO_PRETTY_VALUES``
    By default, the value index of value-parameterized tests is replaced by the
    actual value in the CTest test name.  If this behavior is undesirable
    (e.g. because the value strings are unwieldy), this option will suppress
    this behavior.

  ``PROPERTIES name1 value1...``
    Specifies additional properties to be set on all tests discovered by this
    invocation of ``cutest_discover_tests()``.

  ``TEST_LIST var``
    Make the list of tests available in the variable ``var``, rather than the
    default ``<target>_TESTS``.  This can be useful when the same test
    executable is being used in multiple calls to ``discover_tests()``.
    Note that this variable is only available in CTest.

  ``DISCOVERY_TIMEOUT num``

    Specifies how long (in seconds) CMake will wait for the test to enumerate
    available tests.  If the test takes longer than this, discovery (and your
    build) will fail.  Most test executables will enumerate their tests very
    quickly, but under some exceptional circumstances, a test may require a
    longer timeout.  The default is 5.  See also the ``TIMEOUT`` option of
    :command:`execute_process`.

  ``XML_OUTPUT_DIR dir``

    If specified, the parameter is passed along with ``--cutest_output=xml:``
    to test executable. The actual file name is the same as the test target,
    including prefix and suffix. This should be used instead of
    ``EXTRA_ARGS --cutest_output=xml`` to avoid race conditions writing the
    XML result output when using parallel test execution.

  ``DISCOVERY_MODE``

    Provides greater control over when ``cutest_discover_tests()`` performs test
    discovery. By default, ``POST_BUILD`` sets up a post-build command
    to perform test discovery at build time. In certain scenarios, like
    cross-compiling, this ``POST_BUILD`` behavior is not desirable.
    By contrast, ``PRE_TEST`` delays test discovery until just prior to test
    execution. This way test discovery occurs in the target environment
    where the test has a better chance at finding appropriate runtime
    dependencies.

    ``DISCOVERY_MODE`` defaults to the value of the
    ``CMAKE_CUTEST_DISCOVER_TESTS_DISCOVERY_MODE`` variable if it is not
    passed when calling ``cutest_discover_tests()``. This provides a mechanism
    for globally selecting a preferred test discovery behavior without having
    to modify each call site.

#]=======================================================================]

# Save project's policies
cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW) # if IN_LIST

#------------------------------------------------------------------------------

function(cutest_discover_tests TARGET)
    cmake_parse_arguments(
            ""
            "NO_PRETTY_TYPES;NO_PRETTY_VALUES"
            "TEST_PREFIX;TEST_SUFFIX;WORKING_DIRECTORY;TEST_LIST;DISCOVERY_TIMEOUT;XML_OUTPUT_DIR;DISCOVERY_MODE"
            "EXTRA_ARGS;PROPERTIES;TEST_FILTER"
            ${ARGN}
    )

    if (NOT _WORKING_DIRECTORY)
        set(_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif ()
    if (NOT _TEST_LIST)
        set(_TEST_LIST ${TARGET}_TESTS)
    endif ()
    if (NOT _DISCOVERY_TIMEOUT)
        set(_DISCOVERY_TIMEOUT 5)
    endif ()
    if (NOT _DISCOVERY_MODE)
        if (NOT CMAKE_CUTEST_DISCOVER_TESTS_DISCOVERY_MODE)
            set(CMAKE_CUTEST_DISCOVER_TESTS_DISCOVERY_MODE "POST_BUILD")
        endif ()
        set(_DISCOVERY_MODE ${CMAKE_CUTEST_DISCOVER_TESTS_DISCOVERY_MODE})
    endif ()

    get_property(
            has_counter
            TARGET ${TARGET}
            PROPERTY CTEST_DISCOVERED_TEST_COUNTER
            SET
    )
    if (has_counter)
        get_property(
                counter
                TARGET ${TARGET}
                PROPERTY CTEST_DISCOVERED_TEST_COUNTER
        )
        math(EXPR counter "${counter} + 1")
    else ()
        set(counter 1)
    endif ()
    set_property(
            TARGET ${TARGET}
            PROPERTY CTEST_DISCOVERED_TEST_COUNTER
            ${counter}
    )

    # Define rule to generate test list for aforementioned test executable
    set(ctest_file_base "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}[${counter}]")
    set(ctest_include_file "${ctest_file_base}_include.cmake")
    set(ctest_tests_file "${ctest_file_base}_tests.cmake")
    get_property(crosscompiling_emulator
                 TARGET ${TARGET}
                 PROPERTY CROSSCOMPILING_EMULATOR
    )

    if (_DISCOVERY_MODE STREQUAL "POST_BUILD")
        add_custom_command(
                TARGET ${TARGET} POST_BUILD
                BYPRODUCTS "${ctest_tests_file}"
                COMMAND "${CMAKE_COMMAND}"
                -D "TEST_TARGET=${TARGET}"
                -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
                -D "TEST_EXECUTOR=${crosscompiling_emulator}"
                -D "TEST_WORKING_DIR=${_WORKING_DIRECTORY}"
                -D "TEST_EXTRA_ARGS=${_EXTRA_ARGS}"
                -D "TEST_PROPERTIES=${_PROPERTIES}"
                -D "TEST_PREFIX=${_TEST_PREFIX}"
                -D "TEST_SUFFIX=${_TEST_SUFFIX}"
                -D "TEST_FILTER=${_TEST_FILTER}"
                -D "NO_PRETTY_TYPES=${_NO_PRETTY_TYPES}"
                -D "NO_PRETTY_VALUES=${_NO_PRETTY_VALUES}"
                -D "TEST_LIST=${_TEST_LIST}"
                -D "CTEST_FILE=${ctest_tests_file}"
                -D "TEST_DISCOVERY_TIMEOUT=${_DISCOVERY_TIMEOUT}"
                -D "TEST_XML_OUTPUT_DIR=${_XML_OUTPUT_DIR}"
                -P "${_CUTEST_DISCOVER_TESTS_SCRIPT}"
                VERBATIM
        )

        file(WRITE "${ctest_include_file}"
             "if(EXISTS \"${ctest_tests_file}\")\n"
             "  include(\"${ctest_tests_file}\")\n"
             "else()\n"
             "  add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)\n"
             "endif()\n"
        )
    elseif (_DISCOVERY_MODE STREQUAL "PRE_TEST")

        get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL
                     PROPERTY GENERATOR_IS_MULTI_CONFIG
        )

        if (GENERATOR_IS_MULTI_CONFIG)
            set(ctest_tests_file "${ctest_file_base}_tests-$<CONFIG>.cmake")
        endif ()

        string(CONCAT ctest_include_content
               "if(EXISTS \"$<TARGET_FILE:${TARGET}>\")" "\n"
               "  if(NOT EXISTS \"${ctest_tests_file}\" OR" "\n"
               "     NOT \"${ctest_tests_file}\" IS_NEWER_THAN \"$<TARGET_FILE:${TARGET}>\" OR\n"
               "     NOT \"${ctest_tests_file}\" IS_NEWER_THAN \"\${CMAKE_CURRENT_LIST_FILE}\")\n"
               "    include(\"${_CUTEST_DISCOVER_TESTS_SCRIPT}\")" "\n"
               "    cutest_discover_tests_impl(" "\n"
               "      TEST_EXECUTABLE" " [==[" "$<TARGET_FILE:${TARGET}>" "]==]" "\n"
               "      TEST_EXECUTOR" " [==[" "${crosscompiling_emulator}" "]==]" "\n"
               "      TEST_WORKING_DIR" " [==[" "${_WORKING_DIRECTORY}" "]==]" "\n"
               "      TEST_EXTRA_ARGS" " [==[" "${_EXTRA_ARGS}" "]==]" "\n"
               "      TEST_PROPERTIES" " [==[" "${_PROPERTIES}" "]==]" "\n"
               "      TEST_PREFIX" " [==[" "${_TEST_PREFIX}" "]==]" "\n"
               "      TEST_SUFFIX" " [==[" "${_TEST_SUFFIX}" "]==]" "\n"
               "      TEST_FILTER" " [==[" "${_TEST_FILTER}" "]==]" "\n"
               "      NO_PRETTY_TYPES" " [==[" "${_NO_PRETTY_TYPES}" "]==]" "\n"
               "      NO_PRETTY_VALUES" " [==[" "${_NO_PRETTY_VALUES}" "]==]" "\n"
               "      TEST_LIST" " [==[" "${_TEST_LIST}" "]==]" "\n"
               "      CTEST_FILE" " [==[" "${ctest_tests_file}" "]==]" "\n"
               "      TEST_DISCOVERY_TIMEOUT" " [==[" "${_DISCOVERY_TIMEOUT}" "]==]" "\n"
               "      TEST_XML_OUTPUT_DIR" " [==[" "${_XML_OUTPUT_DIR}" "]==]" "\n"
               "    )" "\n"
               "  endif()" "\n"
               "  include(\"${ctest_tests_file}\")" "\n"
               "else()" "\n"
               "  add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)" "\n"
               "endif()" "\n"
        )

        if (GENERATOR_IS_MULTI_CONFIG)
            foreach (_config ${CMAKE_CONFIGURATION_TYPES})
                file(GENERATE OUTPUT "${ctest_file_base}_include-${_config}.cmake" CONTENT "${ctest_include_content}" CONDITION $<CONFIG:${_config}>)
            endforeach ()
            file(WRITE "${ctest_include_file}" "include(\"${ctest_file_base}_include-\${CTEST_CONFIGURATION_TYPE}.cmake\")")
        else ()
            file(GENERATE OUTPUT "${ctest_file_base}_include.cmake" CONTENT "${ctest_include_content}")
            file(WRITE "${ctest_include_file}" "include(\"${ctest_file_base}_include.cmake\")")
        endif ()

    else ()
        message(FATAL_ERROR "Unknown DISCOVERY_MODE: ${_DISCOVERY_MODE}")
    endif ()

    # Add discovered tests to directory TEST_INCLUDE_FILES
    set_property(DIRECTORY
                 APPEND PROPERTY TEST_INCLUDE_FILES "${ctest_include_file}"
    )

endfunction()

###############################################################################

set(_CUTEST_DISCOVER_TESTS_SCRIPT
    ${CMAKE_CURRENT_LIST_DIR}/CuTestAddTests.cmake
    CACHE INTERNAL "Full path of the CMake script defining the discover test command implementation."
)

# Restore project's policies
cmake_policy(POP)
