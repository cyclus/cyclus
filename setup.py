from __future__ import print_function, unicode_literals
import re
import os
import sys
from distutils.core import setup
import setuptools
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
    ('__version__\s=*', (lambda ver: "__version__ = '{0}'".format(ver)),
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
    scripts = []
    with open('README.rst') as f:
        readme = f.read()
    skw = dict(
        name=PROJECT,
        description='Agent-based fuel cycle simulator.',
        long_description=readme,
        license='BSD 3-clause',
        version=ver.partition('-')[0],
        author='Cyclus Developers',
        maintainer='Cyclus Developers',
        author_email='cyclus-users@googlegroups.com',
        url='http://fuelcycle.org',
        platforms=['posix'],
        classifiers=['Operating System :: POSIX',
                     'Programming Language :: Python :: 3',
                     'Programming Language :: Python :: 2'],
        packages=['cyclus'],
        package_dir={'cyclus': 'cyclus'},
        package_data={'cyclus': ['*.pxd', '*.so', '*.dylib']},
        scripts=scripts,
        zip_safe=False,
        )
    if sys.version_info[0] < 3:
        skw['packages'] = [x.encode() for x in skw['packages']]
    setup(**skw)


def main(argv=None):
    """The main installer for cyclus Python bindings."""
    cyclus_version = os.environ.get('CYCLUS_PROJECT_VERSION', '')
    if not cyclus_version:
        msg = "$CYCLUS_PROJECT_VERSION not set, this is needed to install cyclus. "
        msg += "Normally this is set by the CMake build system, and you should "
        msg += "not be running setup.py directly."
        raise RuntimeError(msg)
    version_update(cyclus_version)
    run_setup(cyclus_version)


if __name__ == '__main__':
    main()

