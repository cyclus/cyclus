#! /usr/bin/env python

import os
import sys

# Utility functions
def addnewlines(lines):
    lines=[line+'\n' for line in lines]
    return lines

def makewritelines(lines):
    myline=''
    for line in lines:
        myline+=line
    return myline

def iseven(number):
    if number%2==0:
        return True
    else:
        return False

def addBriefLines(lines,givenLine):
    

# Filename and directory info
file_front = sys.argv[1]
file_back = sys.argv[2]
file_name = file_front + '.' + file_back

current_dir = os.getcwd()+'/'
mypath = current_dir+file_name
fin = open(mypath,'r')

# read all the lines
original_lines = fin.read()
fin.close()

# set up a new container
new_lines = []

# go through lines
continuation = false
for line in original_lines:
    if line.lstrip()[0:2] == '///':
        addBriefLines(new_lines,line)

    elif line.lstrip()[0:2] == '/*!':
        beginContinuation(new_lines,line)
        continuation = true
        
    elif (line.lstrip() == '*/') and (continuation = true):
        endContinuation(new_lines,line)
        continuation = false

    elif continuation == true:
        moreContinuation(new_lines,line)
    
    else:
        new_lines.append(line)
    
        
# Create lines under Rows header
rows=['ROWS']
rows.append(" N R100")
for i in range(len(weights)):
    index=i+1
    row_index="R"+str(index+100)
    rows.append(" L "+row_index)

# Get input heading info
input=file_name+'.info'
mypath=current_dir+input
infofile=open(mypath,'r')
infolines=infofile.read().split('\n')
infofile.close()

# Add lines together and create .mps input file
mylines=infolines[:-1]+rows+columns+rhs+bounds+['ENDATA']
mylines=makewritelines(addnewlines(mylines))
output=file_name+'.mps'
mypath=current_dir+output
outfile=open(mypath,'w')
outfile.write(mylines)
outfile.close()

