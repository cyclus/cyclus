#! /usr/bin/env python

import os

# Utility functions
def addnewlines(lines):
    lines=[line+'\n' for line in lines]
    return lines

def makewritelines(lines):
    myline=''
    for line in lines:
        myline+=line
    return myline

# Filename and directory info
current_dir=os.getcwd()+'/'
input='quick.raw'
mypath=current_dir+input
fin=open(mypath,'r')

# Read the input file
in_lines=fin.readlines()
fin.close()

massBWR=0
massPWR=0
fracBWR={}
fracPWR={}
inBWR=False
inPWR=False

for line in in_lines:
    line_arr = line.split()
    test = line_arr[0]
    if test == 'StorageFacilityIniStocks:':
        fac_type = line_arr[1][6:9]
        mass = float(line_arr[2])
        if fac_type == 'BWR':
            massBWR += mass
        if fac_type == 'PWR':
            massPWR += mass
    
    if test == 'Recipe':
        inBWR = False
        inPWR = False
    if inBWR:
        isotope = line_arr[0]
        frac = float(line_arr[2])
        fracBWR[isotope] = frac * massBWR
    elif inPWR:
        isotope = line_arr[0]
        frac = float(line_arr[2])
        fracPWR[isotope] = frac * massPWR

    test = line_arr[1]
    if test == 'nwtrb:spent_BWR_uo2':
        inBWR = True
    elif test == 'nwtrb:spent_PWR_uo2':
        inBWR = False
        inPWR = True
    
    
total_mass = massBWR + massPWR
mU234 = fracBWR['92234'] + fracBWR['92234']
mU235 = fracBWR['92235'] + fracBWR['92235']
mU236 = fracBWR['92236'] + fracBWR['92236']
mU238 = fracBWR['92238'] + fracBWR['92238']
mPu238 = fracBWR['94238'] + fracBWR['94238']
mPu239 = fracBWR['94239'] + fracBWR['94239']
mPu240 = fracBWR['94240'] + fracBWR['94240']
mPu241 = fracBWR['94241'] + fracBWR['94241']
mPu242  = fracBWR['94242'] + fracBWR['94242']
mActinides = 0.0
lAct = 89
hAct = 103
mFPs = 0.0
lFPs = 70
hFPs = 170
for isotope in iter(fracBWR):
    test = int(isotope[0:2])
    if test >= lAct and test <=hAct:
        mActinides += fracBWR[isotope]
    test = int(isotope[2:])
    if test >= lFPs and test <=hFPs:
        mFPs += fracBWR[isotope]
for isotope in iter(fracPWR):
    test = int(isotope[0:2])
    if test >= lAct and test <=hAct:
        mActinides += fracPWR[isotope]
    test = int(isotope[2:])
    if test >= lFPs and test <=hFPs:
        mFPs += fracPWR[isotope]

print "Total mass: %1.3E \n\
      mU234: %1.3E \n\
      mU235: %1.3E \n\
      mU236: %1.3E \n\
      mU238: %1.3E \n\
      mPu238: %1.3E \n\
      mPu239: %1.3E \n\
      mPu240: %1.3E \n\
      mPu241: %1.3E \n\
      mPu242: %1.3E \n\
      mActinides (with above masses included): %1.3E \n\
      mFPs: %1.3E" \
      % (total_mass,mU234,mU235,mU236,mU238, \
             mPu238,mPu239,mPu240,mPu241,mPu242, \
             mActinides,mFPs)
