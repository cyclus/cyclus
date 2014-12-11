#! /usr/bin/env python
from __future__ import print_function, unicode_literals
import os
import sys
import tarfile
import subprocess
import shutil
import io
if sys.version_info[0] < 3:
    from urllib import urlopen
else:
    from urllib.request import urlopen

try:
    import argparse as ap
except ImportError:
    import pyne._argparse as ap

absexpanduser = lambda x: os.path.abspath(os.path.expanduser(x))


def check_windows_cmake(cmake_cmd):
    if os.name == 'nt':
        files_on_path = set()
        for p in os.environ['PATH'].split(';')[::-1]:
            if os.path.exists(p):
                files_on_path.update(os.listdir(p))
        if 'cl.exe' in files_on_path:
            pass
        elif 'sh.exe' in files_on_path:
            cmake_cmd += ['-G "MSYS Makefiles"']
        elif 'gcc.exe' in files_on_path:
            cmake_cmd += ['-G "MinGW Makefiles"']
        cmake_cmd = ' '.join(cmake_cmd)

def update_describe():
    root_dir = os.path.split(__file__)[0]
    fname = os.path.join(root_dir, 'src', 'version.cc.in')
    cmd = 'touch {0}'.format(fname)
    subprocess.check_call(cmd.split(), shell=(os.name == 'nt'))

def install_cyclus(args):
    if not os.path.exists(args.build_dir):
        os.mkdir(args.build_dir)
    elif args.clean_build:
        shutil.rmtree(args.build_dir)
        os.mkdir(args.build_dir)

    root_dir = os.path.split(__file__)[0]
    makefile = os.path.join(args.build_dir, 'Makefile')

    if not os.path.exists(makefile):
        rtn = subprocess.call(['which', 'cmake'], shell=(os.name == 'nt'))
        if rtn != 0:
            sys.exit("CMake could not be found, "
                     "please install CMake before developing Cyclus.")
        cmake_cmd = ['cmake', os.path.abspath(root_dir)]
        if args.prefix:
            cmake_cmd += ['-DCMAKE_INSTALL_PREFIX=' +
                          absexpanduser(args.prefix)]
        if args.cmake_prefix_path:
            cmake_cmd += ['-DCMAKE_PREFIX_PATH=' +
                          absexpanduser(args.cmake_prefix_path)]
        if args.coin_root:
            cmake_cmd += ['-DCOIN_ROOT_DIR=' + absexpanduser(args.coin_root)]
        if args.boost_root:
            cmake_cmd += ['-DBOOST_ROOT=' + absexpanduser(args.boost_root)]
        if args.build_type:
            cmake_cmd += ['-DCMAKE_BUILD_TYPE=' + args.build_type]
        check_windows_cmake(cmake_cmd)
        rtn = subprocess.check_call(cmake_cmd, cwd=args.build_dir,
                                    shell=(os.name == 'nt'))

    if args.update:
        update_describe()

    make_cmd = ['make']
    if args.threads:
        make_cmd += ['-j' + str(args.threads)]
    rtn = subprocess.check_call(make_cmd, cwd=args.build_dir,
                                shell=(os.name == 'nt'))

    if args.test:
        make_cmd += ['test']
    elif not args.build_only:
        make_cmd += ['install']

    rtn = subprocess.check_call(make_cmd, cwd=args.build_dir,
                                shell=(os.name == 'nt'))

def uninstall_cyclus(args):
    makefile = os.path.join(args.build_dir, 'Makefile')
    if not os.path.exists(args.build_dir) or not os.path.exists(makefile):
        sys.exist("May not uninstall Cyclus since it has not yet been built.")
    rtn = subprocess.check_call(['make', 'uninstall'], cwd=args.build_dir,
                                shell=(os.name == 'nt'))


def main():
    localdir = absexpanduser('~/.local')

    description = "A Cyclus installation helper script. " +\
        "For more information, please see cyclus.github.com."
    parser = ap.ArgumentParser(description=description)

    build_dir = 'where to place the build directory'
    parser.add_argument('--build_dir', help=build_dir, default='build')

    uninst = 'uninstall'
    parser.add_argument('--uninstall', action='store_true', help=uninst, default=False)

    noupdate = 'do not update the hash in version.cc'
    parser.add_argument('--no-update', dest='update', action='store_false', 
                        help=noupdate, default=True)

    clean = 'attempt to remove the build directory before building'
    parser.add_argument('--clean-build', action='store_true', help=clean)

    threads = "the number of threads to use in the make step"
    parser.add_argument('-j', '--threads', type=int, help=threads)

    prefix = "the relative path to the installation directory"
    parser.add_argument('--prefix', help=prefix, default=localdir)

    build_only = 'only build the package, do not install'
    parser.add_argument('--build-only', action='store_true', help=build_only)

    test = 'run tests after building'
    parser.add_argument('--test', action='store_true', help=test)

    coin = "the relative path to the Coin-OR libraries directory"
    parser.add_argument('--coin_root', help=coin)

    boost = "the relative path to the Boost libraries directory"
    parser.add_argument('--boost_root', help=boost)

    cmake_prefix_path = "the cmake prefix path for use with FIND_PACKAGE, " + \
        "FIND_PATH, FIND_PROGRAM, or FIND_LIBRARY macros"
    parser.add_argument('--cmake_prefix_path', help=cmake_prefix_path)

    build_type = "the CMAKE_BUILD_TYPE"
    parser.add_argument('--build_type', help=build_type)

    args = parser.parse_args()
    if args.uninstall:
        uninstall_cyclus(args)
    else:
        install_cyclus(args)

if __name__ == "__main__":
    main()
