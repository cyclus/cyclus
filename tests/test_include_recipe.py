#! /usr/bin/env python

from nose.tools import assert_false, assert_true, assert_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import tables_exist, find_ids, exit_times, \
    h5out, sqliteout, clean_outs

"""Tests"""
def test_include_recipe():
    """Testing for including of other XML files.
    """
    clean_outs()
    # Cyclus simulation input for recipe including
    sim_input = "./input/include_recipe.xml"
    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands
    clean_outs()
