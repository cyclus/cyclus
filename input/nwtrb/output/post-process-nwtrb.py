#! /usr/bin/env python

import os
import h5py

# Filename and directory info
current_dir=os.getcwd()+'/'
h5file=h5py.File('test.h5')
thislist = []
h5file.visit(thislist.append)
trans = h5file['output/materials/matHist']#output/transactions/transList']
for member in trans:
    print member
h5file.close()

