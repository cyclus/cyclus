#! /usr/bin/env python

# File: conformDoxygen.py
# Author: Matthew Gidden
# Date: March 28, 2012

# Purpose: 
# To conform all doxygen comments in header files
# to the Cyclus standard.
# Note! This is currently incomplete, read the details below

# Detailed Behavior: 
# This script will recursively perform its operation on 
# every header (.h) file beginning in the current directory.
# In its current implementation, the script will find Doxygen
# code blocks beginning with /** and ending with */. For each
# line in between, if a leading asterisk is used, it will be
# removed. The remaining line will then be indented correctly
# with respect to the leading /**'s indentation. Finally, new
# lines (i.e. blank lines) immediately after the initial /**
# will be removed.

# To Do:
# 1) Change any /*!s to /**
# 2) Auto-remove @briefs

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

def cleanLine(line):
    stripped_line = line.lstrip(' ')
    if stripped_line[0] == '*':
        clean_line = stripped_line[1:]
    else:
        clean_line = stripped_line
    return clean_line.lstrip(' ')


# Filename and directory info
start_path = os.getcwd()

files_to_change = []

for (path, dirs, files) in os.walk(start_path):
    for file in files:
        file_split = file.split('.')
        if (len(file_split)>1):
            ext = file_split[1]
            if (ext == 'h'):
                files_to_change.append(path+'/'+file)

for file_name in files_to_change:
    fin = open(file_name,'r')

    # read all the lines
    original_lines = fin.readlines()
    fin.close()
    
    # set up a new container
    new_lines = []

    # go through lines
    continuation = False
    firstline = False
    nspaces = 0

    for line in original_lines:
        doAppend = True
        if ((line.lstrip()[0:2] == "*/") and (continuation == True)):
            line = ' ' * (nspaces + 1) + line.lstrip()
            continuation = False
            
        elif line.lstrip()[0:3] == "/**":
            nspaces = len(line.split("/**")[0])
            continuation = True
            firstline = True
            
        elif continuation == True:
            # we need to work with a line that is just text, 
            # no spaces in front
            clean_line = cleanLine(line)
            # if this is the first line after a /**
            if firstline == True:
                # only append if the first line isn't empty
                if clean_line == '\n':
                    doAppend = False
                firstline = False
            # if we want to append the line, 
            # add the right number of spaces to it
            line = ' ' * (nspaces + 3) + clean_line
        
        if doAppend == True:
            new_lines.append(line)
            
    mylines=makewritelines(new_lines)
    outfile=open(file_name,'w')
    outfile.write(mylines)
    outfile.close()

