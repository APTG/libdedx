# Builds the libdedx._core nanobind extension. This file is added by the
# top-level CMakeLists.txt when DEDX_BUILD_PYTHON is ON (scikit-build-core sets
# this automatically via SKBUILD). The extension statically links the `dedx`
# library target defined in src/CMakeLists.txt, producing a self-contained
# single-file module.

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate the Python development headers and nanobind's CMake package. When built
# under scikit-build-core the interpreter and nanobind are provided by the build
# environment; the snippet below also works for a plain `cmake` invocation.
if(NOT Python_EXECUTABLE)
    find_package(Python 3.9 REQUIRED COMPONENTS Interpreter Development.Module)
else()
    find_package(Python 3.9 REQUIRED COMPONENTS Development.Module)
endif()

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE nanobind_ROOT
)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(_core
    NB_STATIC
    src/dedx_core.cpp
)
target_link_libraries(_core PRIVATE dedx)
# The binding calls a couple of internal helpers (e.g. dedx_internal_get_nucleon)
# whose headers live in src/, which the `dedx` target only exposes privately.
target_include_directories(_core PRIVATE "${PROJECT_SOURCE_DIR}/src")

# Install the compiled module into the libdedx package directory inside the wheel.
install(TARGETS _core LIBRARY DESTINATION libdedx)
