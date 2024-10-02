"""Tests Python wrapping on Env object."""
from __future__ import print_function, unicode_literals

import sys
from cyclus import lib

ENV = lib.Env()

def test_path_base():
    assert ENV.path_base("/home/cyclus") == "/home"


def test_paths():
    assert  len(ENV.install_path) > 0
    assert  len(ENV.build_path) > 0
    assert  len(ENV.get('HOME')) > 0
    assert  len(ENV.rng_schema()) > 0
    # cyclus_path
    cp = ENV.cyclus_path
    assert  len(cp) > 0
    for path in cp:
        assert  isinstance(path, str)
    assert  len(ENV.env_delimiter) > 0
    assert  len(ENV.path_delimiter) > 0
    if sys.platform.startswith('darwin'):
        ext = '.dylib'
    elif sys.platform.startswith('linux'):
        ext = '.so'
    else: # we are not on a supported platform, make failing assertion
        assert "platform" == "darwin or linux"
    assert  len(ENV.find_module(f'libagents{ext}')) > 0


def test_nuc_data():
    assert  len(ENV.nuc_data) > 0
    ENV.set_nuc_data_path(ENV.nuc_data)


def test_allow_milps():
    assert(isinstance(ENV.allow_milps, bool))
