import os
import sys
import subprocess

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises
from nose.plugins.skip import SkipTest

cycdir = os.path.dirname(os.path.dirname(__file__))
reldir = os.path.join(cycdir, 'release')
sys.path.insert(0, reldir)

import tools

try:
    import smbchk
except ImportError:
    smbchk = None

def find_libcyc():
    tried = []
    libcyc = os.path.join(cycdir, 'build', 'lib', 'libcyclus.so')
    if os.path.exists(libcyc):
        print('lib cyclus exists: ', libcyc)
        return libcyc, tried
    tried.append(libcyc)

    try:
        base = subprocess.check_output('which cyclus'.split()).strip()
        libcyc = os.path.join(os.path.dirname(base), 
                              '..', 'lib', 'libcyclus.so')
        if os.path.exists(libcyc):
            print('lib cyclus exists: ', libcyc)
            return libcyc, tried
    except subprocess.CalledProcessError:
        pass
    tried.append(libcyc)

    base = os.path.expanduser('~/anaconda/envs/_build')
    print('base', base)
    libcyc = os.path.join(os.path.dirname(base), 'lib', 'libcyclus.so')
    if os.path.exists(libcyc):
        print('lib cyclus exists: ', libcyc)
        return libcyc, tried
    tried.append(libcyc)
    
    cmd = "find / -type f -name libcyclus.so -executable 2>/dev/null"
    try:
        output = subprocess.check_output(cmd, shell=True, 
                                         stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        output = e.output
    
    libcyc = output.split('\n')[0]
    tried.append(libcyc)
    print('lib cyc found with find: ', output)
    return libcyc, tried

def test_abi_stability():
    if smbchk is None:
        raise SkipTest('Could not import smbchk!')
    if os.name != 'posix':
        raise SkipTest('can only check for ABI stability on posix systems.')
    libcyc, tried = find_libcyc()
    if not os.path.exists(libcyc):
        msg = 'libcyclus could not be found, cannot check for ABI stability\n'
        msg += 'Final libcyc: {}\nTried: {}\nHOME: {}'.format(
            libcyc, tried, os.environ['HOME']) 
        raise SkipTest(msg)
    prefix = os.path.join(os.path.dirname(libcyc), '..')
    args = '--update -t HEAD --no-save --check --prefix {}'
    args = args.format(prefix).split()
    with tools.indir(reldir):
        obs = smbchk.main(args=args)
    assert_true(obs)

if __name__ == "__main__":
    nose.runmodule()
