# Object library: compiled once with -fPIC so the objects feed both the
# static and the shared library without a second compilation pass.
add_library(dedx_objects OBJECT
    dedx_tools.c dedx_bethe.c dedx.c dedx_wrappers.c dedx_data_access.c
    dedx_embedded_data.c dedx_embedded_metadata.c dedx_mpaul.c dedx_mstar.c dedx_spline.c
    dedx_periodic_table.c dedx_validate.c
)
set_property(TARGET dedx_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(dedx_objects PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)
target_include_directories(dedx_objects PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_BINARY_DIR}"
)
# Version passed as compile definitions, derived from git tag at configure time
target_compile_definitions(dedx_objects PRIVATE
    DEDX_VERSION_MAJOR=${DEDX_VERSION_MAJOR}
    DEDX_VERSION_MINOR=${DEDX_VERSION_MINOR}
    DEDX_VERSION_PATCH=${DEDX_VERSION_PATCH}
)

# Static library — for C/CMake consumers; PUBLIC so include dirs propagate.
add_library(dedx STATIC)
target_link_libraries(dedx PUBLIC dedx_objects)
target_include_directories(dedx PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(dedx PRIVATE "${PROJECT_SOURCE_DIR}/src")
# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
if(NOT WIN32)
    target_link_libraries(dedx PUBLIC m)
endif()

# Shared library — for Python ctypes and other dynamic consumers.
add_library(dedx_shared SHARED)
target_link_libraries(dedx_shared PUBLIC dedx_objects)
target_include_directories(dedx_shared PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(dedx_shared PRIVATE "${PROJECT_SOURCE_DIR}/src")
if(WIN32)
    set_target_properties(dedx_shared PROPERTIES
        OUTPUT_NAME dedx
        ARCHIVE_OUTPUT_NAME dedx_shared
    )
else()
    set_target_properties(dedx_shared PROPERTIES OUTPUT_NAME dedx)
endif()
if(NOT WIN32)
    target_link_libraries(dedx_shared PUBLIC m)
endif()

install(FILES
    "${PROJECT_SOURCE_DIR}/include/dedx.h"
    "${PROJECT_SOURCE_DIR}/include/dedx_elements.h"
    "${PROJECT_SOURCE_DIR}/include/dedx_error.h"
    "${PROJECT_SOURCE_DIR}/include/dedx_tools.h"
    "${PROJECT_SOURCE_DIR}/include/dedx_wrappers.h"
    DESTINATION include
)

install(TARGETS dedx dedx_shared
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
