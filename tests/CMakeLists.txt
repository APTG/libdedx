include_directories("${PROJECT_SOURCE_DIR}")

add_executable(loadTest loadTest.c)

target_link_libraries(loadTest dedx)

if(NOT WIN32)
    target_link_libraries(loadTest m)
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

# ---- New-style tests: one file per physics program ----
# Each test_*.c has its own main() and returns number of failures.
# Adding a new program test is as simple as adding a test_<program>.c file.
file(GLOB TEST_SOURCES test_*.c)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE dedx)
    if(NOT WIN32)
        target_link_libraries(${test_name} PRIVATE m)
    endif()

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
