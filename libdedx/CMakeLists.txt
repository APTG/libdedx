# ./libdedx
# ESTAR removed from installer
set(dedx_data data/ASTAR.bin data/astarEng.dat data/mstarEng.dat data/pstarEng.bin data/ASTAR.dat data/compos.txt data/MSTAR.dat data/gas_states.dat data/pstarEng.dat data/astarEng.bin data/effective_charge.dat data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/PSTAR.dat data/short_names data/ICRU73.dat data/ICRU73.bin data/ICRU_PSTAR.dat data/ICRU_PSTAR.bin data/ICRU_ASTAR.dat data/ICRU_ASTAR.bin data/icru73Eng.dat data/icru73Eng.bin data/ICRU73_NEW.dat data/ICRU73_NEW.bin data/icru73_newEng.dat data/icru73_newEng.bin data/composition data/betheEng.dat data/betheEng.bin data/icru_pstarEng.dat data/icru_pstarEng.bin data/icru_astarEng.dat data/icru_astarEng.bin data/atima_compos)

set(dedx_data_bin data/ASTAR.bin data/astarEng.bin data/ICRU73.bin data/icru73Eng.bin data/ICRU73_NEW.bin data/icru73_newEng.bin data/MSTAR.bin data/mstarEng.bin data/PSTAR.bin data/ICRU_PSTAR.bin data/ICRU_ASTAR.bin data/pstarEng.bin data/betheEng.bin data/icru_astarEng.bin data/icru_pstarEng.bin data/compos.txt data/effective_charge.dat data/short_names data/composition data/gas_states.dat data/atima_compos)

#TODO: do we neet data/short_names to be installed?

#include_directories(${CMAKEDEMO_SOURCE_DIR}/libdedx)

add_library (dedx SHARED dedx_tools.c dedx_bethe.c  dedx.c  dedx_file_access.c  dedx_file.c  dedx_mpaul.c  dedx_mstar.c  dedx_spline.c  dedx_split.c  dedx_periodic_table.c tools/dedx_math.c dedx_validate.c)
# removed math_dedx.h
# removed dedx_atima.c

target_link_libraries (dedx m)

#link_directories(${CMAKEDEMO_BINARY_DIR}/libdedx)

#TODO : this does not work?
#SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_SOURCE_DIR}/data/*.bin")


install (FILES dedx.h dedx_tools.h DESTINATION include)
install (TARGETS dedx DESTINATION lib)
if(WIN32)
  install (FILES ${dedx_data_bin} DESTINATION data)
else()
  install (FILES ${dedx_data_bin} DESTINATION share/libdedx)
endif()
