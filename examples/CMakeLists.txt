include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(dedx_example dedx_example.c)
add_executable(getdedx dedx_get.c)
add_executable(dedx_list dedx_list.c)
add_executable(dedx_bethe dedx_bethe.c)
add_executable(dedx_use_wrappers dedx_use_wrappers.c)

target_link_libraries (dedx_example dedx)
target_link_libraries (getdedx dedx)
target_link_libraries (dedx_list dedx)
target_link_libraries (dedx_bethe dedx)
target_link_libraries (dedx_use_wrappers dedx)

# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
IF (NOT WIN32)
    target_link_libraries (dedx_example m)
    target_link_libraries (getdedx m)
    target_link_libraries (dedx_list m)
    target_link_libraries (dedx_bethe m)
    target_link_libraries (dedx_use_wrappers m)
ENDIF()

install (TARGETS getdedx DESTINATION bin)