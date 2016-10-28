from __future__ import print_function, unicode_literals
import re
import os
import sys
from distutils.core import setup
from pprint import pprint

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


def run_setup(ver):
    """Runs the setup function."""
    skw = dict(
        name=PROJECT,
        description='A general purpose, Python-ish shell',
        long_description=readme,
        license='BSD',
        version=ver,
        author='Cyclus Developers',
        maintainer='Cyclus Developers',
        author_email='cyclus-users@googlegroups.com',
        url='http://fuelcycle.org',
        platforms='Cross Platform',
        classifiers=['Programming Language :: Python :: 3'],
        packages=['xonsh', 'xonsh.ply.ply', 'xonsh.ptk', 'xonsh.parsers',
                  'xonsh.xoreutils', 'xontrib',
                  'xonsh.completers', 'xonsh.prompt'],
        package_dir={'xonsh': 'xonsh', 'xontrib': 'xontrib'},
        package_data={'xonsh': ['*.json', '*.githash'], 'xontrib': ['*.xsh']},
        cmdclass=cmdclass,
        scripts=scripts,
        )


def main(argv=None):
    """The main installer for cyclus Python bindings."""
    core_version = os.environ.get('CYCLUS_CORE_VERSION', '')
    if not core_version:
        msg = "$CYCLUS_CORE_VERSION not set, this is needed to install cyclus. "
        msg += "Normally this is set by the CMake build system, and you should "
        msg += "not be running setup.py directly."
        raise RuntimeError(msg)
    version_update(core_version)
    run_setup(core_version)


if __name__ == '__main__':
    main()

