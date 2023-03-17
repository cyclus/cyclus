import os
import sys
import subprocess


cycdir = os.path.dirname(os.path.dirname(__file__))
reldir = os.path.join(cycdir, 'release')
sys.path.insert(0, reldir)

import tools

try:
    import smbchk
except ImportError:
    smbchk = None


def test_abi_stability():
    raise SkipTest('manually remove this skip to test ABI stability')
    if smbchk is None:
        raise SkipTest('Could not import smbchk!')
    if os.name != 'posix':
        raise SkipTest('can only check for ABI stability on posix systems.')
    libcyc = os.path.join(cycdir, 'build', 'lib', 'libcyclus.so')
    if not os.path.exists(libcyc):
        raise SkipTest('libcyclus could not be found, '
                       'cannot check for ABI stability')
    args = '--update -t HEAD --no-save --check'.split()
    with tools.indir(reldir):
        obs = smbchk.main(args=args)
    assert(obs)


if __name__ == "__main__":
    nose.runmodule()
