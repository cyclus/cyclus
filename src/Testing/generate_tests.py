"""generate_tests

A simple module and default main execution to generate a listing of all
non-disabled tests in a google-test-based executable.

The default main function returns to stdin a list of all tests in the
executable.
"""

import os
import subprocess
import sys

try:
    import argparse as ap
except ImportError:
    import pyne._argparse as ap

def parse_tests(test_lines):
    """Return a list of google test names.

    Arguments: 

    test_lines -- a list of the output of a google test exectuable using the
    --gtest_list_tests flag. If the output is in a file, test_lines are the
    result of file.readlines().
    """
    tests = []
    for test_line in test_lines:
        line = test_line.strip()
        if line[-1] == ".":
            current_test = line
        else:        
            assert current_test is not None
            tests.append(current_test + line) 
    return tests

def write_tests_to_output(tests, executable, output):
    """writes a list of test names as ADD_TEST cmake macros to an output file
    
    Arguments
    tests -- a list of all test names to be added to the output file
    exectuable -- the name of the test executable
    output -- the output file to write to
    """
    with open(output, 'a') as f:
        for test in tests:
            line = "ADD_TEST(" + test + " " + \
                executable + " " + "--gtest_filter=" + test + ")"
            f.write(line + '\n')
            
def main():
    description = "A simple program to print a list of all full test names " +\
    "in a google-test-based executable." 
    parser = ap.ArgumentParser(description=description)

    executable = 'the path to the test exectuable to call'
    parser.add_argument('--executable', help=executable, required=True)

    output = 'the cmake script to write test addition to'
    parser.add_argument('--output', help=output)

    args = parser.parse_args()

    # assert os.path.exists(args.executable)
    # assert os.path.exists(args.output)

    rtn = subprocess.Popen([args.executable, "--gtest_list_tests"], 
                           stdout=subprocess.PIPE, shell=(os.name=='nt'))
    
    tests = parse_tests(rtn.stdout.readlines())
    rtn.stdout.close()

    write_tests_to_output(tests, args.executable, args.output)

if __name__ == "__main__":
    sys.exit(main())
