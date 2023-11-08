#! /usr/bin/env python
from __future__ import print_function, unicode_literals
import os
import sys
import tarfile
import platform
import subprocess
import shutil
import io
import argparse as ap


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
    on_darwin = platform.system() == 'Darwin'
    libext = '.dylib' if on_darwin else '.so'

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
        if cmake_cmd is not None:
            cmake_cmd += ['-DDEFAULT_ALLOW_MILPS=' +
                          ('TRUE' if args.allow_milps else 'FALSE')]
        if args.deps_root:
            cmake_cmd += ['-DDEPS_ROOT_DIR=' + absexpanduser(args.deps_root)]
        if args.coin_root:
            cmake_cmd += ['-DCOIN_ROOT_DIR=' + absexpanduser(args.coin_root)]
        if args.boost_root:
            cmake_cmd += ['-DBOOST_ROOT=' + absexpanduser(args.boost_root)]
        if args.cyclus_root:
            cmake_cmd += ['-DCYCLUS_ROOT_DIR='+absexpanduser(args.cyclus_root)]
        if args.hdf5_root:
            h5root = absexpanduser(args.hdf5_root)
            cmake_cmd += ['-DHDF5_ROOT=' + h5root,
                          '-DHDF5_LIBRARIES={0}/lib/libhdf5{1};{0}/lib/libhdf5_hl{1}'.format(h5root, libext),
                          '-DHDF5_LIBRARY_DIRS=' + h5root + '/lib',
                          '-DHDF5_INCLUDE_DIRS=' + h5root + '/include',
                          ]
        if args.build_type:
            cmake_cmd += ['-DCMAKE_BUILD_TYPE=' + args.build_type]
        if args.core_version:
            cmake_cmd += ['-DCORE_VERSION=' + args.core_version]
        if args.D is not None:
            cmake_cmd += ['-D' + x for x in args.D]
        if args.cmake_debug:
            cmake_cmd += ['-Wdev', '--debug-output']
        if args.code_coverage is not None:
            cmake_cmd += ['-DCODE_COVERAGE=' + ('TRUE' if args.code_coverage else 'FALSE')]
        if args.fast is not None:
            fast = 'TRUE' if args.fast else 'FALSE'
            cmake_cmd.append('-DCYCLUS_FAST_COMPILE=' + fast)

        check_windows_cmake(cmake_cmd)
        rtn = subprocess.check_call(cmake_cmd, cwd=args.build_dir,
                                    shell=(os.name == 'nt'))

    if args.config_only:
        return

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
        sys.exit("May not uninstall Cyclus since it has not yet been built.")
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

    config_only = 'only configure the package, do not build or install'
    parser.add_argument('--config-only', action='store_true', help=config_only)

    build_only = 'only build the package, do not install'
    parser.add_argument('--build-only', action='store_true', help=build_only)

    test = 'run tests after building'
    parser.add_argument('--test', action='store_true', help=test)

    parser.add_argument('--allow-milps', action='store_true',
                        dest='allow_milps', default=None,
                        help='Allows mixed integer linear programs by default')
    parser.add_argument('--dont-allow-milps', action='store_false',
                        dest='allow_milps',
                        help="Don't Allows mixed integer linear programs "
                             "by default")

    deps = "the path to the directory containing all dependencies"
    parser.add_argument('--deps-root', '--deps_root', help=deps,
                        default=None, dest='deps_root')

    coin = "the relative path to the Coin-OR libraries directory"
    parser.add_argument('--coin-root', '--coin_root', help=coin)

    boost = "the relative path to the Boost libraries directory"
    parser.add_argument('--boost_root', help=boost)

    hdf5 = "the path to the HDF5 libraries directory"
    parser.add_argument('--hdf5_root', help=hdf5)

    cyclus = "the relative path to Cyclus installation directory"
    parser.add_argument('--cyclus-root', '--cyclus_root', help=cyclus)

    cmake_prefix_path = "the cmake prefix path for use with FIND_PACKAGE, " + \
        "FIND_PATH, FIND_PROGRAM, or FIND_LIBRARY macros"
    parser.add_argument('--cmake_prefix_path', help=cmake_prefix_path)

    build_type = "the CMAKE_BUILD_TYPE"
    parser.add_argument('--build-type', '--build_type', help=build_type)

    parser.add_argument('--core-version', dest='core_version', default=None,
                        help='Sets the core version number.')

    parser.add_argument('-D', metavar='VAR', action='append',
                        help='Set enviornment variable(s).')
    parser.add_argument('--cmake-debug', action='store_true', default=False,
                        dest='cmake_debug', help='puts CMake itself in a debug mode '
                                                 'when dealing with build system issues.')
    code_coverage = "enable code coverage analysis using gcov/lcov"
    parser.add_argument('--code_coverage', help=code_coverage, default=False, action="store_true")
    parser.add_argument('--fast', default=None, dest='fast',
                        action='store_true', help="Will try to compile "
                        "from assembly, if possible. This is faster than "
                        "compiling from source (default).")
    parser.add_argument('--slow', dest='fast',
                        action='store_false', help="Will NOT try to compile "
                        "from assembly, if possible. This is slower as it "
                        "must compile from source.")

    args = parser.parse_args()
    # modify roots as needed
    if args.deps_root is not None:
        roots = ['coin_root', 'boost_root', 'hdf5_root', 'cyclus_root']
        for name in roots:
            if not getattr(args, name, None):
                setattr(args, name, args.deps_root)
    # run code
    if args.uninstall:
        uninstall_cyclus(args)
    else:
        install_cyclus(args)


if __name__ == "__main__":
    main()
