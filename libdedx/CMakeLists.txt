# ESTAR removed from installer
set(dedx_data data/ASTAR.bin data/astarEng.dat data/mstarEng.dat data/pstarEng.bin data/ASTAR.dat data/compos.txt data/MSTAR.dat data/gas_states.dat data/pstarEng.dat data/astarEng.bin data/effective_charge.dat data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/PSTAR.dat data/short_names data/ICRU73.dat data/ICRU73.bin data/ICRU_PSTAR.dat data/ICRU_PSTAR.bin data/ICRU_ASTAR.dat data/ICRU_ASTAR.bin data/icru73Eng.dat data/icru73Eng.bin data/ICRU73_NEW.dat data/ICRU73_NEW.bin data/icru73_newEng.dat data/icru73_newEng.bin data/composition data/betheEng.dat data/betheEng.bin data/icru_pstarEng.dat data/icru_pstarEng.bin data/icru_astarEng.dat data/icru_astarEng.bin)

set(dedx_data_bin data/ASTAR.bin data/astarEng.bin data/ICRU73.bin data/icru73Eng.bin data/ICRU73_NEW.bin data/icru73_newEng.bin data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/ICRU_PSTAR.bin data/ICRU_ASTAR.bin data/pstarEng.bin data/betheEng.bin data/icru_astarEng.bin data/icru_pstarEng.bin data/compos.txt data/effective_charge.dat data/short_names data/composition data/gas_states.dat)

# Object library: compiled once with -fPIC so the objects feed both the
# static and the shared library without a second compilation pass.
add_library(dedx_objects OBJECT
    dedx_tools.c dedx_bethe.c dedx.c dedx_wrappers.c dedx_file_access.c
    dedx_file.c dedx_mpaul.c dedx_mstar.c dedx_spline.c dedx_split.c
    dedx_periodic_table.c tools/dedx_math.c dedx_validate.c
)
set_property(TARGET dedx_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(dedx_objects PUBLIC
    "${PROJECT_SOURCE_DIR}/libdedx"
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
# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
if(NOT WIN32)
    target_link_libraries(dedx PUBLIC m)
endif()

# Shared library — for Python ctypes and other dynamic consumers.
add_library(dedx_shared SHARED)
target_link_libraries(dedx_shared PUBLIC dedx_objects)
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

install(FILES dedx.h dedx_elements.h dedx_error.h dedx_tools.h dedx_wrappers.h DESTINATION include)

install(TARGETS dedx dedx_shared
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(WIN32)
    install(FILES ${dedx_data_bin} DESTINATION data)
else()
    install(FILES ${dedx_data_bin} DESTINATION share/libdedx)
endif()
