"""Tests Python wrapping on Env object."""
from __future__ import print_function, unicode_literals


from cyclus import lib

LOGGER = lib.Logger()

def test_report_level():
    orig = LOGGER.report_level
    assert  orig >= lib.LEV_ERROR
    assert  orig <= lib.LEV_DEBUG5
    LOGGER.report_level = 4
    assert  LOGGER.report_level == 4
    LOGGER.report_level = orig


def test_no_agent():
    orig = LOGGER.no_agent
    LOGGER.no_agent = True
    LOGGER.no_agent = orig


def test_no_mem():
    orig = LOGGER.no_mem
    LOGGER.no_mem = True
    LOGGER.no_mem = orig


def test_to_log_level_string():
    s = LOGGER.to_string(lib.LEV_ERROR)
    assert  isinstance(s, str)
    level = LOGGER.to_log_level(s)
    assert  isinstance(level, int)
    assert lib.LEV_ERROR == level


