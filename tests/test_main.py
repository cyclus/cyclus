"""Tests Python main CLI for Cyclus."""
from __future__ import print_function, unicode_literals
import pytest

from cyclus.main import main

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


@pytest.fixture(params=cases)
def case(request):
    yield request.param

def test_main(case):
    try:
        main(args=case)
        res = True
    except Exception:
        res = False
        raise
    assert  res


