cmake_minimum_required (VERSION 2.6)

project (dedx C)

#cross compilation, 32 bit
#set(CMAKE_SYSTEM_NAME  Windows)
#set(CMAKE_C_COMPILER /usr/bin/i686-w64-mingw32-gcc)
#set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
#set(CMAKE_LD i686-w64-mingw32-ld)
#set(CMAKE_C_FLAGS -static)

#cross compilation, 64 bit
#set(CMAKE_SYSTEM_NAME  Windows)
#set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
#set(CMAKE_C_FLAGS -static)
# øv bøv, låårtet vi'er 'æt :/

SET(CMAKE_FIND_ROOT_PATH /usr/bin)

# The version number.
set (DEDX_VERSION_MAJOR 1)
set (DEDX_VERSION_MINOR 2)
set (DEDX_VERSION_PATCH 0)
set (DEDX_WC_REVISION   0)
# SVN will be 0 if not in Working Copy
include(FindSubversion)
if( EXISTS ${CMAKE_SOURCE_DIR}/.svn/ )
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} DEDX)
endif()


#Python bindings
option(PYTHON_BINDINGS "Build Python bindings" ON)

# Do we want to build the tests?
#option(ENABLE_TESTS "Enable unit tests" ON)
#if(ENABLE_TESTS)
#  include(CTest)
#  enable_testing()
#  add_subdirectory(test)
#endif()

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
ADD_DEFINITIONS(-O2 -Wall)

include_directories ("${PROJECT_SOURCE_DIR}/libdedx")
link_directories ("${PROJECT_SOURCE_DIR}/libdedx")
if(WIN32)
else()
endif()
add_subdirectory(buildbins)
add_subdirectory(examples)
add_subdirectory(libdedx)
add_subdirectory(test)
#add_subdirectory(python)

if(PYTHON_BINDINGS)
  add_subdirectory(python)
endif()



# TODO: this is not working either:
#SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_SOURCE_DIR}/data/*.bin")

#add uninstall target accoring to CMAKE FAQ:
# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


# PRE INSTALL:
# remove old bin in data directory and  
# have to run dedx_build_bin before installation

# packaging 
SET(CPACK_PACKAGE_VENDOR "Aarhus Particle Therapy Group")
set(CPACK_PACKAGE_CONTACT "bassler@phys.au.dk")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Libary for Electronic Charged Particle Stopping Powers")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${DEDX_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${DEDX_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${DEDX_VERSION_PATCH})
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_PROCESSOR})
#IF(NOT DEFINED CPACK_SYSTEM_NAME)
#  SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
#ENDIF(NOT DEFINED CPACK_SYSTEM_NAME)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_SYSTEM_NAME})
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CPACK_SYSTEM_NAME}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_PACKAGE_EXECUTABLES "dedx" "getdedx" "dedx_list")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES ";/CMakeCache.txt;/*\\\\.cbp;/*\\\\.depend;/dEdx\\\\.layout;/Makefile;/*\\\\.bin;/build/;/CMakeFiles/;/.svn/;.*\\\\~;/libdedx/utils;${CPACK_SOURCE_IGNORE_FILES}")
#set(CPACK_INSTALL_DIRECTORIES "/testing")
INCLUDE(CPack)
