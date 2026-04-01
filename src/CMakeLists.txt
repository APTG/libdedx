# Object library: compiled once with -fPIC so the objects feed both the
# static and the shared library without a second compilation pass.
add_library(dedx_objects OBJECT
    dedx_tools.c
    dedx_bethe.c
    dedx.c
    dedx_wrappers.c
    dedx_data_access.c
    dedx_embedded_data.c
    dedx_embedded_metadata.c
    dedx_mpaul.c
    dedx_mstar.c
    dedx_spline.c
    dedx_periodic_table.c
    dedx_validate.c
)
set_property(TARGET dedx_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(dedx_objects PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_BINARY_DIR}"
)
# Version passed as compile definitions, derived from git tag at configure time.
target_compile_definitions(dedx_objects PRIVATE
    DEDX_VERSION_MAJOR=${DEDX_VERSION_MAJOR}
    DEDX_VERSION_MINOR=${DEDX_VERSION_MINOR}
    DEDX_VERSION_PATCH=${DEDX_VERSION_PATCH}
    DEDX_VERSION_STRING="${GIT_VERSION}"
)

# Static library for C/CMake consumers.
add_library(dedx STATIC
    $<TARGET_OBJECTS:dedx_objects>
)
target_include_directories(dedx PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(dedx PRIVATE "${PROJECT_SOURCE_DIR}/src")
set_target_properties(dedx PROPERTIES EXPORT_NAME static)
if(NOT WIN32)
    target_link_libraries(dedx PUBLIC m)
endif()

# Shared library for Python ctypes and other dynamic consumers.
add_library(dedx_shared SHARED
    $<TARGET_OBJECTS:dedx_objects>
)
target_include_directories(dedx_shared PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(dedx_shared PRIVATE "${PROJECT_SOURCE_DIR}/src")
set_target_properties(dedx_shared PROPERTIES
    EXPORT_NAME shared
    OUTPUT_NAME dedx
    VERSION "${DEDX_PROJECT_VERSION}"
    SOVERSION "${DEDX_VERSION_MAJOR}"
)
if(WIN32)
    set_target_properties(dedx_shared PROPERTIES
        ARCHIVE_OUTPUT_NAME dedx_shared
    )
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
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(TARGETS dedx dedx_shared
    EXPORT dedxTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT dedxTargets
    NAMESPACE dedx::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dedx"
)
