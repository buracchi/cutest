cmake_minimum_required(VERSION 3.27)

option(BURACCHI_CUTEST_BUILD_EXAMPLES "Build cutest's example programs." ON)
option(BURACCHI_CUTEST_BUILD_TESTS "Build cutest's own tests." ON)
option(BURACCHI_CUTEST_TESTS_COVERAGE "Enable cutest's own tests coverage reporting" OFF)
option(BURACCHI_CUTEST_INSTALL "Enable installation of cutest. (Projects embedding cutest may want to turn this OFF.)" ON)

project(cutest
        DESCRIPTION "A unit testing library for the C programming language inspired by GoogleTest."
        LANGUAGES C
        VERSION 0.1)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)


add_library(coverage_config INTERFACE)
if(BURACCHI_CUTEST_TESTS_COVERAGE)
    target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
    target_link_options(coverage_config INTERFACE --coverage)
endif()

if(BURACCHI_CUTEST_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(BURACCHI_CUTEST_PACKAGE_NAME "buracchi-${PROJECT_NAME}" CACHE INTERNAL "")
    set(BURACCHI_CUTEST_LIBRARY_PREFIX "libburacchi-${PROJECT_NAME}-" CACHE INTERNAL "")
    set(BURACCHI_CUTEST_TARGETS_EXPORT_NAME "${BURACCHI_CUTEST_PACKAGE_NAME}-targets" CACHE INTERNAL "")
    set(BURACCHI_CUTEST_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated" CACHE INTERNAL "")
    set(BURACCHI_CUTEST_CMAKE_FILES_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${BURACCHI_CUTEST_PACKAGE_NAME}")
    set(BURACCHI_CUTEST_CONFIG_FILE "${BURACCHI_CUTEST_GENERATED_DIR}/${BURACCHI_CUTEST_PACKAGE_NAME}-config.cmake")

    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/config.cmake.in"
            "${BURACCHI_CUTEST_CONFIG_FILE}"
            INSTALL_DESTINATION "${BURACCHI_CUTEST_CMAKE_FILES_INSTALL_DIR}")

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
            DESTINATION "${BURACCHI_CUTEST_CMAKE_FILES_INSTALL_DIR}/cmake")

    install(FILES "${BURACCHI_CUTEST_CONFIG_FILE}"
            DESTINATION "${BURACCHI_CUTEST_CMAKE_FILES_INSTALL_DIR}")
endif()

add_library(cutest "src/cutest.c")
target_include_directories(cutest SYSTEM PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(cutest INTERFACE $<BUILD_INTERFACE:coverage_config>)
set_target_properties(cutest PROPERTIES PREFIX ${BURACCHI_CUTEST_LIBRARY_PREFIX})
add_library(buracchi::cutest::cutest ALIAS cutest)

add_library(cutest_main "src/cutest_main.c")
target_include_directories(cutest_main SYSTEM PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(cutest_main
                      INTERFACE $<BUILD_INTERFACE:coverage_config>
                      PUBLIC cutest)
set_target_properties(cutest_main PROPERTIES PREFIX ${BURACCHI_CUTEST_LIBRARY_PREFIX})
add_library(buracchi::cutest::cutest_main ALIAS cutest_main)

if(BURACCHI_CUTEST_INSTALL)
    install(TARGETS cutest cutest_main
            EXPORT ${BURACCHI_CUTEST_TARGETS_EXPORT_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.h*")

    install(EXPORT ${BURACCHI_CUTEST_TARGETS_EXPORT_NAME}
            NAMESPACE buracchi::${PROJECT_NAME}::
            DESTINATION ${BURACCHI_CUTEST_CMAKE_FILES_INSTALL_DIR})
endif()

if(BURACCHI_CUTEST_BUILD_TESTS)
    include(CTest)
    include(cmake/CuTestDiscoverTests.cmake)

    enable_testing()

    add_subdirectory("test")
endif()
