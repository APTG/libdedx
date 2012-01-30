include_directories ("${PROJECT_SOURCE_DIR}")

#include_directories(${CMAKEDEMO_SOURCE_DIR}/libdedx)
#link_directories(${PROJECT_BINARY_DIR})

add_executable(dedx_example dedx_example.c)
add_executable(getdedx dedx_get.c)
add_executable(dedx_list dedx_list.c)
add_executable(dedx_simple dedx_simple.c)
add_executable(dedx_bethe dedx_bethe.c)
add_executable(dedx_get_parallel dedx_get_parallel.c)

target_link_libraries (dedx_example dedx m)
target_link_libraries (getdedx dedx m)
target_link_libraries (dedx_list dedx m)
target_link_libraries (dedx_simple dedx m)
target_link_libraries (dedx_bethe dedx m)
target_link_libraries (dedx_get_parallel dedx m)

#install (TARGETS dedx_example DESTINATION bin)
install (TARGETS getdedx DESTINATION bin)
