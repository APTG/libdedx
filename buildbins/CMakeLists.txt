include_directories ("${PROJECT_SOURCE_DIR}")

# here we create `dedx_build_bin` executable which builds *.bin files from *.dat files
add_executable(dedx_build_bin dedx_build_bin.c)
target_link_libraries (dedx_build_bin dedx)

# Add a target with no output so it will always be built.
# The target has no output file and is always considered out of date even if the commands try to create a file with the name of the target
# ALL Indicate that this target should be added to the default build target so that it will be run every time
add_custom_target( binary_table ALL )

# once `dedx_build_bin` we call it to generate *.bin files in `libdedx` directory

# Add a custom build rule to the generated build system.
# The second signature adds a custom command to a target such as a library or executable.
# This is useful for performing an operation before or after building the target.
# The command becomes part of the target and will only execute when the target itself is built.
# If the target is already built, the command will not execute
# https://cmake.org/cmake/help/v3.12/command/add_custom_command.html
add_custom_command(
  TARGET binary_table
  POST_BUILD  # POST_BUILD - Run after all other rules within the target have been executed.
  # `${CMAKE_COMMAND}` -E executes CMake commands (like echo or rm) in a portable way, this should work on Linux and Windows
  COMMAND ${CMAKE_COMMAND} -E echo "Removing old *.bin files..."
  COMMAND ${CMAKE_COMMAND} -E rm "${PROJECT_SOURCE_DIR}/libdedx/data/*.bin"
  COMMAND dedx_build_bin
  COMMENT "Execute command to build platform dependent binary data tables..."
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libdedx
  DEPENDS dedx dedx_build_bin
)