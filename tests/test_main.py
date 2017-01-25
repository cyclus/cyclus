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
        ['--agent-version', ':agents:KFacility'],
        ['--schema-path', '/path/to/schema'],
        ['--agent-annotations', ':agents:NullRegion'],
        ['--agent-listing', ':agents'],
        ['--no-agent'],
        ['--no-mem'],
        ['-v', '5'],
        ['--warn-limit', '21'],
        ['-p'],
        ['--include'],
        ['--install-path'],
        ['--cmake-module-path'],
        ['--build-path'],
        ['--rng-schema'],
        ['--nuc-data'],
        ]
    for case in cases:
        try:
            main(args=case)
            res = True
        except Exception:
            res = False
            raise
        yield assert_true, res


