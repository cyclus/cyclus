import os
import sys

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises
from nose.plugins.skip import SkipTest

cycdir = os.path.dirname(os.path.dirname(__file__))
reldir = os.path.join(cycdir, 'release')
sys.path.insert(0, reldir)

import tools

import smbchk

def test_abi_stability():
    if os.name != 'posix':
        raise SkipTest('can only check for ABI stability on posix systems.')
    libcyc = os.path.join(cycdir, 'build', 'lib', 'libcyclus.so')
    if not os.path.exists(libcyc):
        raise SkipTest('libcyclus could not be found, '
                       'cannot check for ABI stability')
    args = '--update -t HEAD --no-save --check'.split()
    with tools.indir(reldir):
        obs = smbchk.main(args=args)
    assert_true(obs)

if __name__ == "__main__":
    nose.runmodule()
