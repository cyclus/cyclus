#! /usr/bin/env python

import os
import numpy as np
from tools import check_cmd, thread_count
from helper import clean_outs, which_outfile


CWD = os.path.dirname(__file__)
INPUT = os.path.join(CWD, "input")

def test_include_recipe(thread_count):
    """Testing for including of other XML files.
    """
    clean_outs()
    # Cyclus simulation input for recipe including
    sim_input = os.path.join(INPUT, "include_recipe.xml")
    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile(thread_count)
    cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, CWD, holdsrtn)
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands
    clean_outs()


def test_include_source_w_archetype(thread_count):
    """Testing for including of other XML files with additional archetypes block in other files.
    """
    clean_outs()
    # Cyclus simulation input for recipe including
    sim_input = os.path.join(INPUT, "include_source_w_archetype.xml")
    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile(thread_count)
    cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, CWD, holdsrtn)
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands
    clean_outs()
