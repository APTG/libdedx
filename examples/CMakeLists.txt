include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(dedx_example dedx_example.c)
add_executable(getdedx dedx_get.c)
add_executable(dedx_list dedx_list.c)
add_executable(dedx_bethe dedx_bethe.c)
add_executable(dedx_use_wrappers dedx_use_wrappers.c)
add_executable(dedx_get_sample_csv dedx_get_sample_csv.c)

target_link_libraries (dedx_example dedx)
target_link_libraries (getdedx dedx)
target_link_libraries (dedx_list dedx)
target_link_libraries (dedx_bethe dedx)
target_link_libraries (dedx_use_wrappers dedx)
target_link_libraries (dedx_get_sample_csv dedx)

# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
IF (NOT WIN32)
    target_link_libraries (dedx_example m)
    target_link_libraries (getdedx m)
    target_link_libraries (dedx_list m)
    target_link_libraries (dedx_bethe m)
    target_link_libraries (dedx_use_wrappers m)
    target_link_libraries (dedx_get_sample_csv m)
ENDIF()

# some of the examples need data/ folder in the same location as generated executable
add_custom_command(TARGET dedx_example PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/libdedx/data $<TARGET_FILE_DIR:dedx_example>/data)

install (TARGETS getdedx DESTINATION bin)