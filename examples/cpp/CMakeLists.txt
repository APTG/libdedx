# C++ example consumers.
#
# These demonstrate that the public libdedx headers can be included and linked
# from C++ thanks to their extern "C" guards. The library remains C-only, so
# C++ is enabled lazily here and the whole subdirectory is skipped when no C++
# compiler is available.
include(CheckLanguage)
check_language(CXX)
if(NOT CMAKE_CXX_COMPILER)
    message(STATUS "No C++ compiler found; skipping C++ example consumers")
    return()
endif()
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(cpp_examples dedx_cpp_core dedx_cpp_wrappers)

foreach(example ${cpp_examples})
    add_executable(${example} ${example}.cpp)
    # Linking the `dedx` target transitively pulls in its PUBLIC dependencies
    # (e.g. the math library on Unix), so no per-platform handling is needed.
    target_link_libraries(${example} PRIVATE dedx)
    if(MSVC)
        target_compile_options(${example} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${example} PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME ${example} COMMAND ${example})
endforeach()
