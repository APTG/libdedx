include_directories ("${PROJECT_SOURCE_DIR}")

#include_directories(${CMAKEDEMO_SOURCE_DIR}/libdedx)
#link_directories(${PROJECT_BINARY_DIR})

add_executable(dedx_build_bin dedx_build_bin.c)
target_link_libraries (dedx_build_bin dedx m)

#install (TARGETS dedx_build_bin DESTINATION bin)

#exec_program(dedx_build_bin)

add_custom_target( binary_table ALL )

#TODO: should probably remove .bin files firs from previous builds?
add_custom_command(
  TARGET binary_table
  POST_BUILD
  COMMAND echo Removing old *.bin files...
  COMMAND rm -fv ${PROJECT_SOURCE_DIR}/libdedx/data/*.bin
  COMMAND dedx_build_bin
  COMMENT "Execute command to build platform dependent binary data tables..."
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libdedx
  DEPENDS dedx dedx_build_bin
)



# TODO: add testing suite
#ENABLE_TESTING()
#ADD_TEST(dedx_build_bin dedx_build_bin)
