import os
import subprocess
import shutil
import sys
import tempfile
from contextlib import contextmanager
import pytest


@contextmanager
def tmpdir():
    d = tempfile.mkdtemp()
    yield d
    shutil.rmtree(d)

@contextmanager
def tmplog(fname):
    file_ptr = open(fname, mode='w')
    yield file_ptr
    os.remove(fname)

@pytest.mark.parametrize("pth", ['stubtest', 'stub_test', 'stubTest'])
def test_stubs(pth):
    log = f'stub_test_{pth}.log'
    agent_name_prefix = 'tmp'

    stub_cmd = f'cycstub --type {{0}} {pth}:{pth}:{{1}}'
    inst_cmd = 'python3 install.py --build_dir {0} --prefix {1}'
    tst_cmd = f'./bin/{pth}_unit_tests'
    run_cmd = f'cyclus example_{agent_name_prefix}_facility.xml'

    with tmpdir() as d:
        src = os.path.join(d, pth)
        bld = os.path.join(d, 'bar')
        inst = os.path.join(d, 'baz')

        with tmplog(log) as f:

            msg = f'**Error, check the log in {log}**'
            os.mkdir(src)

            for flav in ['facility', 'inst', 'region']:
                # generate stub
                cmd = stub_cmd.format(flav, agent_name_prefix + flav.capitalize())
                print(cmd)
                try:
                    subprocess.check_call(cmd.split(), shell=(os.name=='nt'),
                                            cwd=src, stdout=f, stderr=f)
                except subprocess.CalledProcessError as e:
                    print(msg)
                    raise e

            # build/install stub
            cmd = inst_cmd.format(bld, inst)
            print(cmd)
            try:
                subprocess.check_call(cmd.split(), shell=(os.name=='nt'),
                                        cwd=src, stdout=f, stderr=f)
            except subprocess.CalledProcessError as e:
                print(msg)
                pytest.skip(msg)  # skip if we can't install for some reason.

            # run unit tests for stub
            cmd = tst_cmd
            print(cmd)
            try:
                subprocess.check_call(cmd.split(), shell=(os.name=='nt'), cwd=inst,
                                        stdout=f, stderr=f)
            except subprocess.CalledProcessError as e:
                print(msg)
                raise e

            # run sample file for stub
            my_env = os.environ.copy()
            my_env["CYCLUS_PATH"] = f"{inst}/lib/cyclus"

            cmd = run_cmd
            print(cmd)
            try:
                subprocess.check_call(cmd.split(), shell=(os.name=='nt'), cwd=src,
                                        stdout=f, stderr=f, env=my_env)
            except subprocess.CalledProcessError as e:
                print(msg)
                raise e
