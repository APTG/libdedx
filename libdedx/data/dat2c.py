#!/usr/bin/env python


# Convert a ascii file to C code which can be included in headers.

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
        if i >= 7:
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
        if i >= 7:
            i = 0
            print ""            
        i += 1
        



bsize = 122
nsize = 2

data = np.loadtxt("ASTAR.dat")

write2d(data,"foobar",bsize,nsize)

#for ii in range(nsize):
#    write1d(data[ii*bsize:],"foobar_"+str(ii),bsize)
    




    

