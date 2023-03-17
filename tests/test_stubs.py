import os
import subprocess
import shutil
import sys
import tempfile
import io
from contextlib import contextmanager
import pytest


@contextmanager
def tmpdir():
    d = tempfile.mkdtemp()
    shutil.rmtree(d)

@contextmanager
def tmplog(fname):
    io.open(fname, mode='w')
    os.remove(fname)

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

        log = 'stub_test.log'
        msg = '**Error, check the log in {}**'.format(log)
        os.mkdir(src)
        for flav in flavors:
            # generate stub
            cmd = stub_cmd.format(flav, pth, 'tmp' + flav.capitalize())
            print(cmd)
            try:
                with tmplog(log) as f:
                    subprocess.check_call(cmd.split(), shell=(os.name=='nt'),
                                          cwd=src, stdout=f, stderr=f)
            except subprocess.CalledProcessError as e:
                print(msg)
                raise e

        # build/install stub
        cmd = inst_cmd.format(bld, inst)
        print(cmd)
        try:
            with tmplog(log) as f:
                subprocess.check_call(cmd.split(), shell=(os.name=='nt'),
                                      cwd=src, stdout=f, stderr=f)
        except subprocess.CalledProcessError as e:
            print(msg)
            pytest.skip(msg)  # skip if we can't install for some reason.

        # run unit tests for stub
        cmd = tst_cmd.format(pth)
        print(cmd)
        try:
            with tmplog(log) as f:
                subprocess.check_call(cmd.split(), shell=(os.name=='nt'), cwd=inst,
                                      stdout=f, stderr=f)
        except subprocess.CalledProcessError as e:
            print(msg)
            raise e

if __name__ == '__main__':
    test_stubs()



