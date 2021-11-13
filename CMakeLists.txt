cmake_minimum_required(VERSION 3.20)

project(dedx C)

include_directories ("${PROJECT_SOURCE_DIR}/libdedx")
link_directories ("${PROJECT_SOURCE_DIR}/libdedx")
add_subdirectory(buildbins)
#add_subdirectory(examples)
add_subdirectory(libdedx)

set (CMAKE_VERBOSE_MAKEFILE false)
ADD_DEFINITIONS(-O2 -Wall)

#Python bindings
option(PYTHON_BINDINGS "Build Python bindings" ON)

if(PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

#add uninstall target according to CMAKE FAQ:
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)