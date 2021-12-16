include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(loadTest loadTest.c)
add_executable(test test.c)

if(WIN32)
target_link_libraries (loadTest dedx)
target_link_libraries (test dedx)
ELSE()
target_link_libraries (loadTest dedx m)
target_link_libraries (test dedx m)
ENDIF()

# TODO: add testing suite
ENABLE_TESTING()
ADD_TEST(test_initialiaze loadTest 1)
ADD_TEST(test_load_ASTAR loadTest 2)
ADD_TEST(test_load_PSTAR loadTest 3)
ADD_TEST(test_load_MSTAR loadTest 4)
ADD_TEST(test_load_ICRU49_proton loadTest 5)
ADD_TEST(test_load_ICRU49_helium loadTest 6)
ADD_TEST(test_load_Bethe loadTest 7)
ADD_TEST(test_load_ICRU73 loadTest 8)
ADD_TEST(test_load_ICRU73_NEW loadTest 9)

