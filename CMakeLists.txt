#[[ As minimum CMake we choose version which supports Ubuntu 18.04 LTS
I followed advice from https://hsf-training.github.io/hsf-training-cmake-webpage/aio/index.html
CMake 3.10 was released 20.11.2017 (https://www.kitware.com/cmake-3-10-0-available-for-download/)

consider using newer version of CMake, i.e. CMake 3.15 (released 17.07.2019) supports `-j N threads` and `-t target`
(following https://indico.jlab.org/event/420/contributions/7961/attachments/6507/8734/CMakeSandCroundtable.slides.pdf)]]

cmake_minimum_required(VERSION 3.10)

project(dedx LANGUAGES C)

# List of variables to be inserted into libdedx/dedx_config.h by `configure_file` command
# The DEDX version number.
set (DEDX_VERSION_MAJOR 1)
set (DEDX_VERSION_MINOR 2)
set (DEDX_VERSION_PATCH 1)

set (DEDX_DATA_PATH_LOCAL "data/")
if (WIN32)
  set (DEDX_DATA_PATH "data/")
else()
  set (DEDX_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/libdedx/")
endif()

# Copy a file to another location and modify its contents
# Copies an <input> file to an <output> file and
# substitutes variable values referenced as @VAR@ or ${VAR} in the input file content.
# Each variable reference will be replaced with the current value of the variable,
# or the empty string if the variable is not defined
# https://cmake.org/cmake/help/v3.12/command/configure_file.html
configure_file (
  "${PROJECT_SOURCE_DIR}/libdedx/dedx_config.h.in"
  "${PROJECT_SOURCE_DIR}/libdedx/dedx_config.h"
  )

include_directories ("${PROJECT_SOURCE_DIR}/libdedx")
link_directories ("${PROJECT_SOURCE_DIR}/libdedx")
add_subdirectory(buildbins)
add_subdirectory(examples)
add_subdirectory(libdedx)
add_subdirectory(test)

# add uninstall target according to CMAKE FAQ:
# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Do we want to build the tests?
#option(ENABLE_TESTS "Enable unit tests" ON)
#if(ENABLE_TESTS)
#  add_subdirectory(test)
#endif()