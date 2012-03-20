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
    #  COMMAND echo Nb test1
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