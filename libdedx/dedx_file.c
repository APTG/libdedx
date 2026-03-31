/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dedx_file.h"

const char *dedx_internal_get_program_file(int program) {
    switch (program) {
    case DEDX_ASTAR:
        return "ASTAR";
    case DEDX_PSTAR:
        return "PSTAR";
    case DEDX_ESTAR:
        return "ESTAR";
    case DEDX_MSTAR:
        return "MSTAR";
    case DEDX_ICRU73_OLD:
        return "ICRU73";
    case DEDX_ICRU73:
        return "ICRU73_NEW";
    case DEDX_ICRU49:
        return "ICRU_ASTAR";
    case _DEDX_0008:
        return "ICRU_PSTAR";
    default:
        return "";
    }
}

const char *dedx_internal_get_energy_file(int program) {
    switch (program) {
    case DEDX_ASTAR:
        return "astarEng";
    case DEDX_PSTAR:
        return "pstarEng";
    case DEDX_ESTAR:
        return "estarEng";
    case DEDX_MSTAR:
        return "mstarEng";
    case DEDX_ICRU73_OLD:
        return "icru73Eng";
    case DEDX_ICRU73:
        return "icru73_newEng";
    case DEDX_ICRU49:
        return "icru_astarEng";
    case _DEDX_0008:
        return "icru_pstarEng";
    case DEDX_BETHE_EXT00:
        return "betheEng";
    default:
        return "";
    }
}
