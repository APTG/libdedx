include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(loadTest loadTest.c)
add_executable(dedx_test test.c)

target_link_libraries (loadTest dedx)
target_link_libraries (dedx_test dedx)

# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
if(NOT WIN32)
    target_link_libraries(loadTest m)
    target_link_libraries(dedx_test m)
endif()

# enable_testing() is called in the root CMakeLists.txt
add_test(NAME test_initialiaze     COMMAND loadTest 1)
add_test(NAME test_load_ASTAR      COMMAND loadTest 2)
add_test(NAME test_load_PSTAR      COMMAND loadTest 3)
add_test(NAME test_load_MSTAR      COMMAND loadTest 4)
add_test(NAME test_load_ICRU49_proton COMMAND loadTest 5)
add_test(NAME test_load_ICRU49_helium COMMAND loadTest 6)
add_test(NAME test_load_Bethe      COMMAND loadTest 7)
add_test(NAME test_load_ICRU73     COMMAND loadTest 8)
add_test(NAME test_load_ICRU73_NEW COMMAND loadTest 9)

