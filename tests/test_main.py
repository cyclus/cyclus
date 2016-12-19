"""Tests Python main CLI for Cyclus."""
from __future__ import print_function, unicode_literals

from nose.tools import assert_equal, assert_true

from cyclus.main import main


def test_main():
    cases = [
        ['-V'],
        ['--schema'],
        ['--flat-schema', '--schema'],
        ['--agent-schema', ':agents:KFacility'],
        ]
    for case in cases:
        try:
            main(args=case)
            res = True
        except Exception:
            res = False
        yield assert_true, res


