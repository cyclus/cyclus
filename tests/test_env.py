"""Tests Python wrapping on Env object."""
from __future__ import print_function, unicode_literals

from nose.tools import assert_equal, assert_true

from cyclus import lib

ENV = lib.Env()

def test_path_base():
    yield assert_equal, ENV.path_base("/home/cyclus"), "/home"


def test_paths():
    yield assert_true, len(ENV.install_path) > 0
    yield assert_true, len(ENV.build_path) > 0
    yield assert_true, len(ENV.get('HOME')) > 0
    yield assert_true, len(ENV.rng_schema()) > 0
    # cyclus_path
    cp = ENV.cyclus_path
    yield assert_true, len(cp) > 0
    for path in cp:
        yield assert_true, isinstance(path, str)
    yield assert_true, len(ENV.env_delimiter) > 0
    yield assert_true, len(ENV.path_delimiter) > 0
    yield assert_true, len(ENV.find_module('agents')) > 0


def test_nuc_data():
    yield assert_true, len(ENV.nuc_data) > 0
    ENV.set_nuc_data_path(ENV.nuc_data)


def test_allow_milps():
    assert_true(isinstance(ENV.allow_milps, bool))
