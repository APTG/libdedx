cmake_minimum_required(VERSION 3.21)

project(dedx LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(DEDX_BUILD_EXAMPLES "Build libdedx example programs" ON)
option(DEDX_BUILD_TESTS "Build libdedx test suite" ON)

# ---- Version from git tag ----
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()
if(NOT GIT_VERSION)
    set(GIT_VERSION "0.0.0-unknown")
endif()
message(STATUS "libdedx version: ${GIT_VERSION}")

# Parse semver components from tag (e.g. v1.2.1 or v1.2.1-3-gabcdef)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ "${GIT_VERSION}")
if(CMAKE_MATCH_1)
    set(DEDX_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(DEDX_VERSION_MINOR ${CMAKE_MATCH_2})
    set(DEDX_VERSION_PATCH ${CMAKE_MATCH_3})
else()
    set(DEDX_VERSION_MAJOR 0)
    set(DEDX_VERSION_MINOR 0)
    set(DEDX_VERSION_PATCH 0)
endif()
set(DEDX_PROJECT_VERSION "${DEDX_VERSION_MAJOR}.${DEDX_VERSION_MINOR}.${DEDX_VERSION_PATCH}")

enable_testing()
add_subdirectory(src)
if(DEDX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(DEDX_BUILD_TESTS)
    add_subdirectory(tests)
endif()

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/dedxConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/dedxConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedx"
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/dedxConfigVersion.cmake"
    VERSION "${DEDX_PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${PROJECT_BINARY_DIR}/dedxConfig.cmake"
    "${PROJECT_BINARY_DIR}/dedxConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedx"
)

# add uninstall target according to CMAKE FAQ:
# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
