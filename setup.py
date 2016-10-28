from __future__ import print_function, unicode_literals
import re
import os
import sys
from distutils.core import setup

PROJECT = 'cyclus'

#
# Version updates borrowed from xonsh
# Copyright 2015-2016, the xonsh developers. All rights reserved.
#

def replace_in_file(pattern, new, fname):
    """Replaces a given pattern in a file"""
    with open(fname, 'r') as f:
        raw = f.read()
    lines = raw.splitlines()
    ptn = re.compile(pattern)
    for i, line in enumerate(lines):
        if ptn.match(line):
            lines[i] = new
    upd = '\n'.join(lines) + '\n'
    with open(fname, 'w') as f:
        f.write(upd)


VERSION_UPDATE_PATTERNS = [
    ('__version__\s*=.*', (lambda ver: "__version__ = '{0}'".format(ver)),
        [PROJECT, '__init__.py']),
    ]

def version_update(ver):
    """Updates version strings in relevant files."""
    for p, n, f in VERSION_UPDATE_PATTERNS:
        if callable(n):
            n = n(ver)
        replace_in_file(p, n, os.path.join(*f))


def main(argv=None):
    """The main installer for cyclus Python bindings."""
    core_version = os.environ.get('CYCLUS_CORE_VERSION', '')
    if not core_version:
        msg = "$CYCLUS_CORE_VERSION not set, this is needed to install cyclus. "
        msg += "Normally this is set by the CMake build system, and you should "
        msg += "not be running setup.py directly."
        raise RuntimeError(msg)
    version_update(core_version)


if __name__ == '__main__':
    main()



