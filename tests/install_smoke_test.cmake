set(smoke_root "${TEST_BINARY_DIR}/install-smoke")
set(smoke_build "${smoke_root}/build")
set(smoke_prefix "${smoke_root}/prefix")
set(smoke_consumer "${smoke_root}/install_consumer")
set(smoke_consumer_build "${smoke_root}/consumer-build")
set(smoke_install_libdir "lib")

if(DEFINED CMAKE_INSTALL_LIBDIR AND NOT CMAKE_INSTALL_LIBDIR STREQUAL "")
    set(smoke_install_libdir "${CMAKE_INSTALL_LIBDIR}")
endif()

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

set(smoke_libdir "${smoke_prefix}/${smoke_install_libdir}")
if(NOT EXISTS "${smoke_libdir}" AND EXISTS "${smoke_prefix}/lib64")
    set(smoke_libdir "${smoke_prefix}/lib64")
endif()

execute_process(
    COMMAND "${CMAKE_C_COMPILER}"
            "-I${smoke_prefix}/include"
            "-L${smoke_libdir}"
            "${SOURCE_DIR}/tests/install_consumer.c"
            -o "${smoke_consumer}"
            -ldedx
            -lm
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke consumer compile failed: ${rc}")
endif()

set(runtime_env)
if(APPLE)
    list(APPEND runtime_env
        "DYLD_LIBRARY_PATH=${smoke_libdir}:$ENV{DYLD_LIBRARY_PATH}"
    )
else()
    list(APPEND runtime_env
        "LD_LIBRARY_PATH=${smoke_libdir}:$ENV{LD_LIBRARY_PATH}"
    )
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
            ${runtime_env}
            "${smoke_consumer}"
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke consumer run failed: ${rc}")
endif()

set(consumer_configure_args
    -S "${SOURCE_DIR}/tests/install_consumer_cmake"
    -B "${smoke_consumer_build}"
    "-DCMAKE_PREFIX_PATH=${smoke_prefix}"
    "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
)
if(DEFINED CMAKE_GENERATOR AND NOT CMAKE_GENERATOR STREQUAL "")
    list(PREPEND consumer_configure_args -G "${CMAKE_GENERATOR}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" ${consumer_configure_args}
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke CMake consumer configure failed: ${rc}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" --build "${smoke_consumer_build}" --parallel
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke CMake consumer build failed: ${rc}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
            ${runtime_env}
            "${smoke_consumer_build}/install_consumer"
    RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "install smoke CMake consumer run failed: ${rc}")
endif()
