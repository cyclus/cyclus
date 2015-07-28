import os
import subprocess
import tempfile

from contextlib import contextmanager

@contextmanager
def tmpdir():
    d = tempfile.mkdtemp()
    yield d
    os.rmdir(d)

def test_stubs():
    flavors = ['facility', 'institution', 'region']
    pth = 'foo'
    with tmpdir() as d:
        src = os.path.join(d, pth)
        bld = os.path.join(d, 'bar')
        inst = os.path.join(d, 'baz')
        
        stub_cmd = 'cycstub {0} {1}:{0}:{0}'
        inst_cmd = 'python install.py --build_dir {0} --prefix {1}'
        tst_cmd = '{}_unit_tests'
        
        os.mkdir(src)
        for flav in flavors:
            # generate stub
            cmd = stub_cmd.format(flav, pth)
            subprocess.call(cmd.split(), shell=(os.name=='nt'), cwd=src)
            # build/install stub
            cmd = inst_cmd.format(bld, inst)
            subprocess.call(cmd.split(), shell=(os.name=='nt'), cwd=src)
            # run unit tests for stub
            cmd = tst_cmd.format(flav)
            subprocess.call(cmd.split(), shell=(os.name=='nt'), 
                            cwd=os.path.join(inst, pth))

if __name__ == '__main__':
    test_stubs()
    
    
    
