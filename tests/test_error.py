"""Tests Python wrapping for cyclus errors."""
from __future__ import print_function, unicode_literals

from nose.tools import assert_equal, assert_true

from cyclus import lib

def test_warn_limit():
    orig = lib.get_warn_limit()
    yield assert_true, orig >= 0
    lib.set_warn_limit(42)
    yield assert_equal, 42, lib.get_warn_limit()
    lib.set_warn_limit(orig)

def test_warn_as_error():
    orig = lib.get_warn_as_error()
    yield assert_true, isinstance(orig, bool)
    lib.set_warn_as_error(True)
    yield assert_true, lib.get_warn_as_error()
    lib.set_warn_as_error(orig)



