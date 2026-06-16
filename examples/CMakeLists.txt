add_executable(dedx_example dedx_example.c)
add_executable(getdedx dedx_get.c)
add_executable(dedx_list dedx_list.c)
add_executable(dedx_bethe dedx_bethe.c)
add_executable(dedx_use_wrappers dedx_use_wrappers.c)
add_executable(dedx_get_sample_csv dedx_get_sample_csv.c)
add_executable(dedx_csda dedx_csda.c)
add_executable(dedx_bench_lookup dedx_bench_lookup.c)
add_executable(dedx_custom_compound dedx_custom_compound.c)

target_link_libraries(dedx_example dedx)
target_link_libraries(getdedx dedx)
target_link_libraries(dedx_list dedx)
target_link_libraries(dedx_bethe dedx)
target_link_libraries(dedx_use_wrappers dedx)
target_link_libraries(dedx_get_sample_csv dedx)
target_link_libraries(dedx_csda dedx)
target_link_libraries(dedx_bench_lookup dedx)
target_link_libraries(dedx_custom_compound dedx)

# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
if(NOT WIN32)
    target_link_libraries(dedx_example m)
    target_link_libraries(getdedx m)
    target_link_libraries(dedx_list m)
    target_link_libraries(dedx_bethe m)
    target_link_libraries(dedx_use_wrappers m)
    target_link_libraries(dedx_get_sample_csv m)
    target_link_libraries(dedx_csda m)
    target_link_libraries(dedx_bench_lookup m)
    target_link_libraries(dedx_custom_compound m)
endif()

foreach(example dedx_example dedx_bethe dedx_list dedx_use_wrappers dedx_get_sample_csv dedx_csda dedx_custom_compound)
    add_test(NAME ${example} COMMAND ${example})
endforeach()

add_test(NAME getdedx_lookup COMMAND getdedx 2 1 276 100)
add_test(NAME getdedx_list_programs COMMAND getdedx -1 1 276 100)

install(TARGETS getdedx DESTINATION "${CMAKE_INSTALL_BINDIR}")

# C++ example consumers (demonstrate the extern "C" public headers from C++).
option(DEDX_BUILD_CXX_EXAMPLES "Build C++ example consumers (requires a C++ compiler)" ON)
if(DEDX_BUILD_CXX_EXAMPLES)
    add_subdirectory(cpp)
endif()
