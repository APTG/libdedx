cmake_minimum_required(VERSION 2.6.0)

find_package(PythonLibs)
if (NOT PYTHONLIBS_FOUND)
  message(STATUS "Python libraries NOT found")
endif (NOT PYTHONLIBS_FOUND)

find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
  message(STATUS "Python interpreter NOT found")
endif (NOT PYTHONINTERP_FOUND)

if(PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
  set(DO_PYTHON_BINDINGS ON BOOL)
  message(STATUS "Python bindings will be compiled")
endif(PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)

if(DO_PYTHON_BINDINGS)
  add_custom_target( python_bindings ALL )

  add_custom_command(
    TARGET python_bindings
    POST_BUILD
    
    COMMAND echo "\\#!/usr/bin/python" > setup.py
    COMMAND echo "from distutils.core import setup, Extension" >> setup.py
    COMMAND echo "" >> setup.py
    COMMAND echo "module1 = Extension\\(\\'libdedx\\'," >> setup.py
    COMMAND echo "                    define_macros = [\\(\\'MAJOR_VERSION\\', \\'${DEDX_VERSION_MAJOR}\\'\\)," >> setup.py
    COMMAND echo "                                     \\(\\'MINOR_VERSION\\', \\'${DEDX_VERSION_MINOR}\\'\\)]," >> setup.py
    COMMAND echo "                    include_dirs = [\\'${PROJECT_SOURCE_DIR}/libdedx\\']," >> setup.py
    COMMAND echo "                    libraries = [\\'dedx\\']," >> setup.py
    COMMAND echo "                    library_dirs = [\\'${PROJECT_BINARY_DIR}/libdedx\\']," >> setup.py
    COMMAND echo "                    sources = [\\'libdedx.c\\']\\)" >> setup.py
    COMMAND echo "setup \\(name = \\'libdedx\\'," >> setup.py
    COMMAND echo "       version = \\'${DEDX_VERSION_MAJOR}.${DEDX_VERSION_MINOR}.${DEDX_VERSION_PATCH}\\'," >> setup.py
    COMMAND echo "       description = \\'Wrapper for stopping power library libdedx\\'," >> setup.py
    COMMAND echo "       author = \\'Jakob Toftegaard\\'," >> setup.py
    COMMAND echo "       author_email = \\'jakob.toftegaard@gmail.com\\'," >> setup.py
    COMMAND echo "       url = \\'http://libdedx.sourceforge.net/\\'," >> setup.py
    COMMAND echo "       long_description = \\'\\'," >> setup.py
    COMMAND echo "       ext_modules = [module1]\\)" >> setup.py
    COMMAND echo "" >> setup.py
    
    COMMAND ${PYTHON_EXECUTABLE} setup.py build
    COMMENT "Build python bindings..."
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python
    DEPENDS dedx
    )
endif()

#add_custom_target(python_bindings_install
#  COMMAND echo Nb test2
#  COMMAND ${PYTHON_EXECUTABLE} setup.py install
#  COMMENT "Install python bindings..."
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python
#  DEPENDS dedx
#)

# TODO: this simply runs python ./setup.py install after libdEdx was installed.
# This is not the proper way of doing it.
if(DO_PYTHON_BINDINGS)
    install(CODE "execute_process(COMMAND echo Install python bindings...)")
    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py install WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python)")
endif()