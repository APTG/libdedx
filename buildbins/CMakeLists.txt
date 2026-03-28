include_directories ("${PROJECT_SOURCE_DIR}")

# here we create `dedx_build_bin` executable which builds *.bin files from *.dat files
add_executable(dedx_build_bin dedx_build_bin.c)
target_link_libraries (dedx_build_bin dedx)

# Add a target with no output so it will always be built.
# The target has no output file and is always considered out of date even if the commands try to create a file with the name of the target
# ALL Indicate that this target should be added to the default build target so that it will be run every time
add_custom_target(binary_table ALL)

# Ensure dedx and dedx_build_bin are built before binary_table runs
add_dependencies(binary_table dedx_build_bin)

add_custom_command(
  TARGET binary_table
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Removing old *.bin files..."
  COMMAND ${CMAKE_COMMAND} -E remove -f "${PROJECT_SOURCE_DIR}/libdedx/data/*.bin"
  COMMAND $<TARGET_FILE:dedx_build_bin>
  COMMENT "Execute command to build platform dependent binary data tables..."
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libdedx
)