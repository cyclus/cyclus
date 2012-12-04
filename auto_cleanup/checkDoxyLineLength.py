#! /usr/bin/env python

# File: checkDoxyLineLength.py
# Author: Matthew Gidden
# Date: March 31, 2012

# Purpose: 
# To check that all lines in Doxygen comments do not exceed 73 characters.

# Detailed Behavior: 
# This script will recursively perform its operation on 
# every header (.h) file beginning in the current directory.
# In its current implementation, the script will find Doxygen
# code blocks beginning with /** and ending with */. It will then
# reform the documentation block to no more than 73 characters in length.

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

def rasterBodyLines(body_lines,nspaces, offset, hard_limit):
    prev_chars = []
    return_lines = []
    for line in body_lines:
        for char in line.split(' '):
            # insert at front so we can pop
            prev_chars.insert(0,char)
        compilation_line = ' ' * (nspaces + offset)
        limit_not_reached = True
        while (limit_not_reached) and (len(prev_chars) > 0):
            future_len = len(compilation_line) + len(prev_chars[-1]) + 1
            if (future_len > hard_limit):
                limit_not_reached = False
            else:
                add_chars = prev_chars.pop()
                compilation_line += add_chars + ' '
        return_lines.append(compilation_line + '\n')

    return return_lines


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
    body_lines = []
    max_line_length = 73

    # go through lines
    continuation = False
    nspaces = 0

    for line in original_lines:
        # hit the end of a doxygen code block
        if ((line.lstrip()[0:2] == "*/") and (continuation == True)):
            # 3 offset for /**
            lines = rasterBodyLines(body_lines,nspaces,3,max_line_length)
            for nline in lines:
                new_lines.append(nline)
            line = ' ' * (nspaces + 1) + line.lstrip()
            new_lines.append(line)
            continuation = False
            
        # hit the beginning of a doxygen code block
        elif line.lstrip()[0:3] == "/**":
            new_lines.append(line)
            nspaces = len(line.split("/**")[0])
            body_lines = []
            continuation = True
        
        # in the middle of a doxygen code block
        elif continuation == True:
            body_lines.append(line.strip())
        
        # not in a doxygen code block
        else:
            new_lines.append(line)
            
    mylines=makewritelines(new_lines)
    outfile=open(file_name,'w')
    outfile.write(mylines)
    outfile.close()

