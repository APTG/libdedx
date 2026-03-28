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

if(NOT WIN32)
    add_test(
        NAME test_install_consumer
        COMMAND ${CMAKE_COMMAND}
                -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
                -DTEST_BINARY_DIR=${PROJECT_BINARY_DIR}
                -DCMAKE_COMMAND=${CMAKE_COMMAND}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                -P ${PROJECT_SOURCE_DIR}/tests/install_smoke_test.cmake)
endif()
