set(smoke_root "${TEST_BINARY_DIR}/install-smoke")
set(smoke_build "${smoke_root}/build")
set(smoke_prefix "${smoke_root}/prefix")
set(smoke_consumer "${smoke_root}/install_consumer")

file(REMOVE_RECURSE "${smoke_root}")
file(MAKE_DIRECTORY "${smoke_root}")

set(configure_args
    -S "${SOURCE_DIR}"
    -B "${smoke_build}"
    -DCMAKE_INSTALL_PREFIX=${smoke_prefix}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)
if(DEFINED CMAKE_GENERATOR AND NOT CMAKE_GENERATOR STREQUAL "")
    list(PREPEND configure_args -G "${CMAKE_GENERATOR}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" ${configure_args}
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke configure failed: ${rc}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" --build "${smoke_build}" --parallel
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke build failed: ${rc}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" --install "${smoke_build}"
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke install failed: ${rc}")
endif()

execute_process(
    COMMAND "${CMAKE_C_COMPILER}"
            "-I${smoke_prefix}/include"
            "-L${smoke_prefix}/lib"
            "${SOURCE_DIR}/tests/install_consumer.c"
            -o "${smoke_consumer}"
            -ldedx
            -lm
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke consumer compile failed: ${rc}")
endif()

execute_process(COMMAND "${smoke_consumer}"
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke consumer run failed: ${rc}")
endif()
