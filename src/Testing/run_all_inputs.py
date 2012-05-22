#!/usr/bin/python

import sys
import os
import re
 
def main():
    check_inputs()
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
            if re.search("\.xml",name):
                files.append(os.path.join(root, name))
            else :
                files.remove(name)
    return files

class TestFile():
    '''An object representing the inputxml file to test'''
    def __init__(self, file_path):
        self.name = file_path # strip off front bit
  
    def test_no_errors(self):
        os.system("./cyclus -v9 " + self.name)
        

if __name__ == '__main__' : main()
