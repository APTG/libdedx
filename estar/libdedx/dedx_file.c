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

char * _dedx_get_program_file(int program)
{
    char * path;
    switch(program)
    {
        case 1:
            path = "ASTAR";
            break;
        case 2:
            path = "PSTAR";
            break;
        case 3:
            path = "ESTAR";
            break;
        case 4:
            path = "MSTAR";
            break;
        case 5:
            path = "ICRU73";
            break;
        case 6:
            path = "ICRU73_NEW";
            break;
	case 7:
	    path = "ICRU_ASTAR";
	    break;
	case 8:
	    path = "ICRU_PSTAR";
	    break;
        default:
            path = "";
    }
    return path;
}
char * _dedx_get_energy_file(int program)
{
    char * path;
    switch(program)
    {
        case 1:
            path = "astarEng";
            break;
        case 2:
            path = "pstarEng";
            break;
        case 3:
            path = "estarEng";
            break;
        case 4:
            path = "mstarEng";
            break;
        case 5:
            path = "icru73Eng";
            break;
        case 6:
            path = "icru73_newEng";
            break;
	case 7:
	    path = "icru_astarEng";
	    break;
	case 8:
	    path = "icru_pstarEng";
	    break;
        case 101:
            path = "betheEng";
            break;
        default:
            path = "";
    }
    return path;
}
