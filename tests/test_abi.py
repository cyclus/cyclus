import os
import sys
import subprocess
import pytest


cycdir = os.path.dirname(os.path.dirname(__file__))
reldir = os.path.join(cycdir, 'release')
sys.path.insert(0, reldir)

import tools

try:
    import smbchk
except ImportError:
    smbchk = None


def test_abi_stability():
    pytest.skip('manually remove this skip to test ABI stability')
    if smbchk is None:
        pytest.skip('Could not import smbchk!')
    if os.name != 'posix':
        pytest.skip('can only check for ABI stability on posix systems.')
    libcyc = os.path.join(cycdir, 'build', 'lib', 'libcyclus.so')
    if not os.path.exists(libcyc):
        pytest.skip('libcyclus could not be found, '
                       'cannot check for ABI stability')
    args = '--update -t HEAD --no-save --check'.split()
    with tools.indir(reldir):
        obs = smbchk.main(args=args)
    assert(obs)


