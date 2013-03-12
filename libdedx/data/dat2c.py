#!/usr/bin/env python
#
# Convert a ascii file to C code which can be included in headers.
#
# 12.03.2013 Niels Bassler <bassler@phys.au.dk>
#

import numpy as np

def write2d(_data, _name, _bsize, _nsize,_type="float"): #
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
                if _type == "float":
                    print ("%8.5e}" %x) # foobar
                if _type == "int" :
                    print ("%3i}" %x) #
                print "};"
                return

            print ("%8.5e}," %x) # foobar
            print ""
            j = 0
            i = 0
        else:
        # print the data
            if _type == "float":
                print ("%8.5e," %x), # foobar
            if _type == "int" :
                print ("%3i" %x), #                
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


def write1d(_data, _name, _bsize, _type="float"): # 

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
            if _type == "float":
                print ("%8.5e};" %x) # foobar
            if _type == "int":
                print ("%3i};" %x) # foobar
            print ""
            return
        else:
        # print the data
            if _type == "int":
                print ("%3i," %x), # foobar
            if _type == "float":
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
#data = np.loadtxt("betheEng.dat")
#write1d(data[1:],"static const float bethe_energy",122)

### COMPOSITION ###
data  = [] # array holding ICRU compound ID
nele  = [] # number of elements for corresponding compound
cindx = [] # array index for compound
cdata_elem = [] # long array with all elements, pointer cindx needed to access
cdata_frac = [] # long array with all fractions, pointer cindx needed to access


lines = open("composition",'r').readlines()
i = 0
j = 0

for line in lines:
    if line[0] == "#":
        data.append(int(line[1:]))
        i = 0
        cindx.append(j) # start index of each new element

        

    if line[0] == "\n":
        nele.append(i)


    if (line[0].isdigit()):        
        i += 1
        j += 1
        cdata_elem.append(int(line.split(":")[0]))
        cdata_frac.append(float(line.split(":")[1]))

# need to manually add the last element since CR is missing at end of file.
nele.append(i)

write1d(data,"static const int _dedx_compos_list", len(data), "int")
write1d(nele,"static const int _dedx_compos_nele", len(nele), "int")
write1d(cindx,"static const int _dedx_compos_cindx", len(nele), "int")
write1d(cdata_elem,"static const int _dedx_compos_elem", len(cdata_elem), "int")
write1d(cdata_frac,"static const float _dedx_compos_frac", len(cdata_frac), "float")

