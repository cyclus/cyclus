#!/usr/bin/python

import sys
import os
import re
 
def main():
    check_inputs(sys.argv)
    path = get_path()
    files = get_files(path)
    for file in files :
        file_to_test = TestFile(file) 
        file_to_test.test_no_errors()

def check_inputs():
    if len(sys.argv) != 2: 
        print 'Usage: run_all_inputs.py <path_to_Input_folder>' 
        sys.exit(1) 
 
def get_path():
    path = sys.argv[1]
    return path

def get_files(path):
    for root, dirs, files in os.walk(path, followlinks=True):
        if '.git' in dirs:
            dirs.remove('.git')
        for name in files: 
            if not re.search("\.xml",name):
                files.remove(name)
    return files

class TestFile(self, file_path):
    '''An object representing the inputxml file to test'''
  def __init__(self, file_path):
    self.name = file_path # strip off front bit
    self. = 

  def test(self):
    os.system("./cyclus " + os.path.join(root, name))
    print("The Classy classmember speaks with clarity.")

if __name__ == '__main__' : main()
