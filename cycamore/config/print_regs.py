from __future__ import print_function

import os

reg_tst_name = """RegressionTests"""
end_tst_token = """Testing:"""

def main():
    build_tst_dir = os.path.join(os.getcwd(), "Testing", "Temporary")

    fname = os.path.join(build_tst_dir, "LastTestsFailed.log")

    print_regs = False
    if os.path.exists(fname):
        with open(fname) as f:
            lines = f.readlines()
    
        for line in lines:
            if reg_tst_name in line:
                print_regs = True
    
    fname = os.path.join(build_tst_dir, "LastTest.log")
    with open(fname) as f:
        lines = f.readlines()

    if print_regs:
        in_regs = False
        for line in lines:
            # entering regression test region
            if reg_tst_name in line:
                in_regs = True
            # leaving regression test region
            if in_regs and end_tst_token in line:
                in_regs = False
            if in_regs:
                print(line.strip())

if __name__ == "__main__":
    main()
