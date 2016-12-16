"""Tests Python wrapping on Env object."""
from __future__ import print_function, unicode_literals

from nose.tools import assert_equal, assert_true

from cyclus import lib

LOGGER = lib.Logger()

def test_report_level():
    orig = LOGGER.report_level
    yield assert_true, orig >= lib.LEV_ERROR
    yield assert_true, orig <= lib.LEV_DEBUG5
    LOGGER.report_level = 4
    yield assert_true, LOGGER.report_level == 4
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
    yield assert_true, isinstance(s, str)
    level = LOGGER.to_log_level(s)
    yield assert_true, isinstance(level, int)
    yield assert_equal, lib.LEV_ERROR, level


