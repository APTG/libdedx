cmake_minimum_required(VERSION 3.21)

project(dedx LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# ---- Data paths (baked into dedx_config.h at configure time) ----
# DEDX_DATA_PATH_LOCAL is used at runtime to prefer the source-tree data dir over the install path.
# Pass -DDEDX_DATA_PATH_LOCAL= (empty) to disable it, e.g. in install smoke tests.
set(DEDX_DATA_PATH_LOCAL "${PROJECT_SOURCE_DIR}/libdedx/data/" CACHE STRING "Build-tree data path (empty to force use of install path)")
if(WIN32)
    set(DEDX_DATA_PATH "data/")
else()
    set(DEDX_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/libdedx/")
endif()

# Generate dedx_config.h into the build directory (not source tree)
configure_file(
    "${PROJECT_SOURCE_DIR}/libdedx/dedx_config.h.in"
    "${PROJECT_BINARY_DIR}/dedx_config.h"
)

include_directories("${PROJECT_SOURCE_DIR}/libdedx")
include_directories("${PROJECT_BINARY_DIR}")
link_directories("${PROJECT_SOURCE_DIR}/libdedx")
option(DEDX_BUILD_BINARY_TABLE "Build the binary data tables (requires a working install of libdedx data)" ON)
if(DEDX_BUILD_BINARY_TABLE)
    add_subdirectory(buildbins)
endif()
add_subdirectory(examples)
add_subdirectory(libdedx)

enable_testing()
add_subdirectory(tests)

# add uninstall target according to CMAKE FAQ:
# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

