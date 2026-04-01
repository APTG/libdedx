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

# Minimal packaging metadata for CPack-generated release artifacts.
set(CPACK_PACKAGE_NAME "libdedx")
set(CPACK_PACKAGE_VENDOR "Aarhus Particle Therapy Group")
set(CPACK_PACKAGE_CONTACT "https://github.com/APTG/libdedx/issues")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/APTG/libdedx")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library for charged particle stopping power calculations")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/cmake/package-description.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${DEDX_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DEDX_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${DEDX_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${DEDX_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${DEDX_PROJECT_VERSION}"
)
set(CPACK_SOURCE_IGNORE_FILES
    "/build/;/build-release/;/build-coverage/;/build-packaging/;/.git/;/.venv/;/.idea/;/.vscode/;__pycache__/;\\\\.pyc$"
)

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "https://github.com/APTG/libdedx/issues")
string(CONCAT CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "libdedx is a C library for charged particle stopping power calculations (dE/dx) in materials.\n"
    "Authors: Jakob Toftegaard, Niels Bassler, and Leszek Grzanka.\n"
    ".\n"
    "This package contains the shared and static libraries, public headers, exported CMake package files,\n"
    "and the getdedx command-line tool."
)

set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Science")
set(CPACK_RPM_PACKAGE_URL "https://github.com/APTG/libdedx")
set(CPACK_RPM_PACKAGE_RELEASE 1)
string(CONCAT CPACK_RPM_PACKAGE_DESCRIPTION
    "libdedx is a C library for charged particle stopping power calculations (dE/dx) in materials.\n"
    "\n"
    "Authors: Jakob Toftegaard, Niels Bassler, and Leszek Grzanka.\n"
    "\n"
    "This package contains the shared and static libraries, public headers, exported CMake package files,\n"
    "and the getdedx command-line tool."
)

include(CPack)

# add uninstall target according to CMAKE FAQ:
# https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
