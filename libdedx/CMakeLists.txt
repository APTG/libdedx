# ESTAR removed from installer
set(dedx_data data/ASTAR.bin data/astarEng.dat data/mstarEng.dat data/pstarEng.bin data/ASTAR.dat data/compos.txt data/MSTAR.dat data/gas_states.dat data/pstarEng.dat data/astarEng.bin data/effective_charge.dat data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/PSTAR.dat data/short_names data/ICRU73.dat data/ICRU73.bin data/ICRU_PSTAR.dat data/ICRU_PSTAR.bin data/ICRU_ASTAR.dat data/ICRU_ASTAR.bin data/icru73Eng.dat data/icru73Eng.bin data/ICRU73_NEW.dat data/ICRU73_NEW.bin data/icru73_newEng.dat data/icru73_newEng.bin data/composition data/betheEng.dat data/betheEng.bin data/icru_pstarEng.dat data/icru_pstarEng.bin data/icru_astarEng.dat data/icru_astarEng.bin data/atima_compos)

set(dedx_data_bin data/ASTAR.bin data/astarEng.bin data/ICRU73.bin data/icru73Eng.bin data/ICRU73_NEW.bin data/icru73_newEng.bin data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/ICRU_PSTAR.bin data/ICRU_ASTAR.bin data/pstarEng.bin data/betheEng.bin data/icru_astarEng.bin data/icru_pstarEng.bin data/compos.txt data/effective_charge.dat data/short_names data/composition data/gas_states.dat data/atima_compos)

add_library (dedx STATIC dedx_tools.c dedx_bethe.c  dedx.c   dedx_wrappers.c dedx_file_access.c  dedx_file.c  dedx_mpaul.c  dedx_mstar.c  dedx_spline.c  dedx_split.c  dedx_periodic_table.c tools/dedx_math.c dedx_validate.c)
# removed math_dedx.h
# removed dedx_atima.c

target_link_libraries (dedx)

# Visual Studio does not need or want you to explicitly request linking the math library.
# You must avoid adding it as a link library when building for Windows.
# see https://stackoverflow.com/questions/54935559/linking-math-library-in-cmake-file-on-windows-and-linux
IF (NOT WIN32)
  target_link_libraries (dedx m)
ENDIF()

install (FILES dedx.h dedx_tools.h dedx_wrappers.h DESTINATION include)
install (TARGETS dedx DESTINATION lib)
if(WIN32)
  install (FILES ${dedx_data_bin} DESTINATION data)
else()
  install (FILES ${dedx_data_bin} DESTINATION share/libdedx)
endif()
