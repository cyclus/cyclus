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

def addBriefLines(lines,givenLine):
    line_halves = givenLine.split("///")
    line1 = line_halves[0] + '/**\n'
    line2 = line_halves[0] + ' * @brief' + line_halves[1]
    line3 = line_halves[0] + ' */\n'
    lines.append(line1)
    lines.append(line2)
    lines.append(line3)

def beginContinuation(lines,givenLine):
    line_halves = givenLine.split('/*!')
    line = line_halves[0] + '/**' + line_halves[1]
    lines.append(line)

def moreContinuation(lines,givenLine,nspaces):
    line = ''
    for i in range(nspaces):
        line += ' '
    if (givenLine.lstrip()[0:2] != "*/"):
        line += " * "
    if len(givenLine.lstrip()) == 0:
        line += givenLine.lstrip() + "\n"
    else:
        line += givenLine.lstrip()
    lines.append(line)

# Filename and directory info
file_front = sys.argv[1]
file_back = sys.argv[2]
file_name = file_front + '.' + file_back

current_dir = os.getcwd()+'/'
mypath = current_dir+file_name
fin = open(mypath,'r')

# read all the lines
original_lines = fin.readlines()
fin.close()

# set up a new container
new_lines = []

# go through lines
continuation = False
nspaces = 0

for line in original_lines:
    if ((line.lstrip()[0:2] == "*/") and (continuation == True)):
        #moreContinuation(new_lines,line,nspaces)
        new_lines.append(line)
        continuation = False

    elif continuation == True:
        moreContinuation(new_lines,line,nspaces)
    
    elif line.lstrip()[0:3] == "/*!":
        beginContinuation(new_lines,line)
        continuation = True
        nspaces = len(line.split("/*!")[0])

    elif line.lstrip()[0:3] == "///":
        addBriefLines(new_lines,line)
         
    else:
        new_lines.append(line)
    
# Add lines together and create .mps input file
mylines=makewritelines(new_lines)
output='tmp'+file_name
mypath=current_dir+output
outfile=open(mypath,'w')
outfile.write(mylines)
outfile.close()

