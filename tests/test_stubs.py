import os
import subprocess
import shutil
import tempfile

from contextlib import contextmanager

@contextmanager
def tmpdir():
    d = tempfile.mkdtemp()
    yield d
    shutil.rmtree(d)

def test_stubs():
    flavors = ['facility', 'inst', 'region']
    pth = 'foo'
    with tmpdir() as d:
        src = os.path.join(d, pth)
        bld = os.path.join(d, 'bar')
        inst = os.path.join(d, 'baz')
        
        stub_cmd = 'cycstub --type {0} {1}:{1}:{2}'
        inst_cmd = 'python install.py --build_dir {0} --prefix {1}'
        tst_cmd = './bin/{}_unit_tests'
        
        os.mkdir(src)
        for flav in flavors:
            # generate stub
            cmd = stub_cmd.format(flav, pth, 'tmp' + flav.capitalize())
            print(cmd)
            subprocess.call(cmd.split(), shell=(os.name=='nt'), cwd=src)
        # build/install stub
        cmd = inst_cmd.format(bld, inst)
        print(cmd)
        subprocess.call(cmd.split(), shell=(os.name=='nt'), cwd=src)
        # run unit tests for stub
        cmd = tst_cmd.format(pth)
        print(cmd)
        subprocess.call(cmd.split(), shell=(os.name=='nt'), cwd=inst)

if __name__ == '__main__':
    test_stubs()
    
    
    
