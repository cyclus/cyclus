"""Tests Python wrapping for cyclus errors."""
from __future__ import print_function, unicode_literals


from cyclus import lib

def test_warn_limit():
    orig = lib.get_warn_limit()
    assert  orig >= 0
    lib.set_warn_limit(42)
    assert 42 == lib.get_warn_limit()
    lib.set_warn_limit(orig)

def test_warn_as_error():
    orig = lib.get_warn_as_error()
    assert  isinstance(orig, bool)
    lib.set_warn_as_error(True)
    assert  lib.get_warn_as_error()
    lib.set_warn_as_error(orig)



