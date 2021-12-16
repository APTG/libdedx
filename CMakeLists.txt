# As minimum CMake we choose version which supports Ubuntu 18.04 LTS
# I followed advice from https://hsf-training.github.io/hsf-training-cmake-webpage/aio/index.html
# CMake 3.10 was released 20.11.2017 (https://www.kitware.com/cmake-3-10-0-available-for-download/)

# consider using newer version of CMake, i.e. CMake 3.15 (released 17.07.2019) supports `-j N threads` and `-t target`
# (following https://indico.jlab.org/event/420/contributions/7961/attachments/6507/8734/CMakeSandCroundtable.slides.pdf)

cmake_minimum_required (VERSION 3.10)

project (dedx C)

SET(CMAKE_BUILD_TYPE DEBUG)

SET(CMAKE_FIND_ROOT_PATH /usr/bin)

# The version number.
set (DEDX_VERSION_MAJOR 1)
set (DEDX_VERSION_MINOR 2)
set (DEDX_VERSION_PATCH 1)
set (DEDX_WC_REVISION   0)

if (WIN32)
  set (DEDX_DATA_PATH "data/")
  set (DEDX_DATA_PATH_LOCAL "data/")
else()
  set (DEDX_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/libdedx/")
  set (DEDX_DATA_PATH_LOCAL "data/")
endif()

# TODO: avoid checking for CXX, we dont need it.

configure_file (
  "${PROJECT_SOURCE_DIR}/libdedx/dedx_config.h.in"
  "${PROJECT_SOURCE_DIR}/libdedx/dedx_config.h"
  )

#set (CMAKE_VERBOSE_MAKEFILE true)
set (CMAKE_VERBOSE_MAKEFILE false)
#ADD_DEFINITIONS(-O2 -Wall)

include_directories ("${PROJECT_SOURCE_DIR}/libdedx")
link_directories ("${PROJECT_SOURCE_DIR}/libdedx")
add_subdirectory(buildbins)
#add_subdirectory(examples)
add_subdirectory(libdedx)
#add_subdirectory(test)


#add uninstall target accoring to CMAKE FAQ:
# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
