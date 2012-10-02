#! /usr/bin/env python

from tempfile import mkstemp
from shutil import move
from os import remove, close, walk, getcwd, stat

def replace(file, pattern, subst):
    #Create temp file
    fh, abs_path = mkstemp()
    new_file = open(abs_path,'w')
    old_file = open(file)
    for line in old_file:
        new_file.write(line.replace(pattern, subst))
    #close temp file
    new_file.close()
    close(fh)
    old_file.close()
    #Remove original file
    remove(file)
    #Move new file
    move(abs_path, file)

# Filename and directory info
start_path = getcwd()

for (path, dirs, files) in walk(start_path):
    for file in files:   
        parts = file.split(".")
        if parts[-1] == "cpp":
            replace(path+"/"+file,"constructor","construct"+parts[0])
            replace(path+"/"+file,"destructor","destruct"+parts[0])
