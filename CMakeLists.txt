cmake_minimum_required(VERSION 3.24)

option(CUTEST_BUILD_TESTS "Build tests" ON)
option(CUTEST_ENABLE_TEST_COVERAGE "Enable coverage reporting" OFF)
option(CUTEST_INSTALL "Generate installation target" ON)

project(cutest
        DESCRIPTION "A unit testing library for the C programming language inspired by GoogleTest."
        LANGUAGES C
        VERSION 0.1)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)

if (CUTEST_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
endif ()

if (CUTEST_BUILD_TESTS)
    include(CTest)
    enable_testing()
endif ()

add_library(coverage_config INTERFACE)

if (CUTEST_ENABLE_TEST_COVERAGE)
    target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
    target_link_options(coverage_config INTERFACE --coverage)
endif ()

if (CUTEST_INSTALL)
    set(CUTEST_EXPORT_NAME "buracchi-${PROJECT_NAME}")
    set(CUTEST_NAMESPACE "buracchi::${PROJECT_NAME}")
    set(CUTEST_LIBRARY_PREFIX "libburacchi-${PROJECT_NAME}-")

    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/config.cmake.in"
            "${PROJECT_BINARY_DIR}/${CUTEST_EXPORT_NAME}-config.cmake"
            INSTALL_DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/${CUTEST_EXPORT_NAME}")

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${CUTEST_EXPORT_NAME}/cmake")

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${CUTEST_EXPORT_NAME}-config.cmake"
            DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/${CUTEST_EXPORT_NAME}")
endif ()

add_library(cutest "src/cutest.c")
target_include_directories(cutest PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(cutest INTERFACE $<BUILD_INTERFACE:coverage_config>)
set_target_properties(cutest PROPERTIES PREFIX ${CUTEST_LIBRARY_PREFIX})

add_library(cutest_main "src/cutest_main.c")
target_link_libraries(cutest_main PRIVATE cutest)
target_link_libraries(cutest_main INTERFACE $<BUILD_INTERFACE:coverage_config>)
set_target_properties(cutest_main PROPERTIES PREFIX ${CUTEST_LIBRARY_PREFIX})

if (CUTEST_BUILD_TESTS)
    include(cmake/CuTestDiscoverTests.cmake)
    add_subdirectory("test")
endif ()

if (CUTEST_INSTALL)
    install(TARGETS cutest cutest_main
            EXPORT cutest-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.h*")

    install(EXPORT cutest-targets
            FILE ${CUTEST_EXPORT_NAME}-targets.cmake
            NAMESPACE ${CUTEST_NAMESPACE}::
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${CUTEST_EXPORT_NAME}")
endif ()
