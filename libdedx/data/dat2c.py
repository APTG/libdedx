#!/usr/bin/env python
#
# Convert a ascii file to C code which can be included in headers.
#
# 12.03.2013 Niels Bassler <bassler@phys.au.dk>
#

import numpy as np

def write2d(_data, _name, _bsize, _nsize): #
    i = 1
    j = 0
    k = 1

    _size = _bsize*_nsize
    
    print _name+"[%i][%i] = {{" %(_nsize,_bsize)

    for x in _data:


        # check if start of new block
        if j == 1:
            print "  {"
        if j == 0: # make a nice start
            j = 1

        # check if start of line
        if i == 1:
            print (" "),

        # check if end of block
        if j >= _bsize:
            # check if end of data
            if k >= _size:
                print ("%8.5e}" %x) # foobar
                print "};"
                return

            print ("%8.5e}," %x) # foobar
            print ""
            j = 0
            i = 0
        else:
        # print the data
            print ("%8.5e," %x), # foobar
        j += 1

        # check if end of data
        if k > _size:
            print ("};") # foobar
            print ""
            return
        k += 1

        # check if end of line
        if i >= 6:
            i = 0
            print ""            
        i += 1


def write1d(_data, _name, _bsize): # 

    # _data:  linear data array
    # _name:  name of varioable
    # _bsize: number of data to be written.


    i = 1
    j = 1
    
    print _name+"[%i] = {" %(_bsize)
    for x in _data:

        # check if start of line
        if i == 1:
            print (" "),

        # check if end of block
        if j >= _bsize:
            print ("%8.5e};" %x) # foobar
            print ""
            return
        else:
        # print the data
            print ("%8.5e," %x), # foobar
        j += 1


        # check if end of line
        if i >= 6:
            i = 0
            print ""            
        i += 1
        

#### ASTAR ####
#data = np.loadtxt("astarEng.dat")
#write1d(data[1:],"static const float astar_energy",122)  # 122 = data[0]
#data = np.loadtxt("ASTAR.dat")
#write2d(data,"static const float astar",122,74)

### ESTAR ###
#data = np.loadtxt("estarEng.dat")
#write1d(data[1:],"static const float estar_energy",133)
#data = np.loadtxt("ESTAR.dat")
#write2d(data,"static const float estar",132,77)

### PSTAR ###
#data = np.loadtxt("pstarEng.dat")
#write1d(data[1:],"static const float pstar_energy",133)
#data = np.loadtxt("PSTAR.dat")
#write2d(data,"static const float pstar",133,74)

### MSTAR ###
#data = np.loadtxt("mstarEng.dat")
#write1d(data[1:],"static const float mstar_energy",132)
#data = np.loadtxt("MSTAR.dat")
#write2d(data,"static const float mstar",132,78)

### ICRU73 ###
#data = np.loadtxt("icru73Eng.dat")
#write1d(data[1:],"static const float icru73_energy",53)
#data = np.loadtxt("ICRU73.dat")
#write2d(data,"static const float icru73",53,896)

### ICRU73_NEW ###
#data = np.loadtxt("icru73_newEng.dat")
#write1d(data[1:],"static const float icru73new_energy",53)
#data = np.loadtxt("ICRU73_NEW.dat")
#write2d(data,"static const float icru73new",53,36)

### ICRU49 PROTONS ###
#data = np.loadtxt("icru_pstarEng.dat")
#write1d(data[1:],"static const float icru_pstar_energy",133)
#data = np.loadtxt("ICRU_PSTAR.dat")
#write2d(data,"static const float icru_pstar",133,74)
    
### ICRU49 ALPHAS ###
#data = np.loadtxt("icru_astarEng.dat")
#write1d(data[1:],"static const float icru_astar_energy",122)
#data = np.loadtxt("ICRU_ASTAR.dat")
#write2d(data,"static const float icru_astar",122,74)
   
### BETHE energy grid (what is this used for anyway?)  ###
data = np.loadtxt("betheEng.dat")
write1d(data[1:],"static const float bethe_energy",122)
