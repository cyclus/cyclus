#!/usr/bin/env python
"""Welcome to cymetric's setup.py script. This is a little non-standard because pyne
is a multilanguage project.  Still, this script follows a predicatable ordering:

1. Parse command line arguments
2. Call cmake from the 'build' directory
3. Call make from the 'build' directory
4. Use distuitls/setuptools from the 'build' directory

This gives us the best of both worlds. Compiled code is installed with cmake/make
and Cython/Python code is installed with normal Python tools. The only trick here is
how the various command line arguments are handed off to the three sub-processes.

To accomplish this we use argparser groups to group command line arguments based on
whether they go to:

1. the setup() function,
2. cmake,
3. make, or
4. other - typically used for args that apply to multiple other groups or
   modify the environment in some way.

To add a new command line argument, first add it to the appropriate group in the
``parse_args()`` function.  Then, modify the logic in the cooresponding
``parse_setup()``, ``parse_cmake()``, ``parse_make()``, or ``parse_others()``
functions to consume your new command line argument.  It is OK for more than
one of the parser functions to consume the argument. Where appropriate,
ensure the that argument is appended to the argument list that is returned by these
functions.
"""
from __future__ import print_function

import os
import sys
import imp
import argparse
import platform
import warnings
import subprocess
from glob import glob
from distutils import core, dir_util


VERSION = '1.5.5'
IS_NT = os.name == 'nt'


def main():
    scripts = [os.path.join('scripts', f) for f in os.listdir('scripts')]
    scripts = [s for s in scripts if ((IS_NT and s.endswith('.bat'))
                                      or (not IS_NT and
                                          not s.endswith('.bat')))]
    packages = ['cymetric']
    pack_dir = {'cymetric': 'cymetric'}
    setup_kwargs = {
        "name": "cymetric",
        "version": VERSION,
        "description": 'Cyclus Metric Calculator',
        "author": 'Cyclus Development Team',
        "author_email": 'cyclus-dev@googlegroups.com',
        "url": 'http://fuelcycle.org/',
        "packages": packages,
        "package_dir": pack_dir,
        "scripts": scripts,
        }
    rtn = core.setup(**setup_kwargs)
    return rtn


if __name__ == "__main__":
    main()
