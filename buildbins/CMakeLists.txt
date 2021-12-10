include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(dedx_build_bin dedx_build_bin.c)
target_link_libraries (dedx_build_bin dedx)

add_custom_target( binary_table ALL )

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