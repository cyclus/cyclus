#! /usr/bin/python

from nose.tools import assert_false
import os
import tables
import numpy as np
from tools import check_cmd

""" Tests """
def test_null_sink():
    """ Testing for null sink case without a source facility """
    #Cyclus simulation input for null sink testing
    sim_input = "./Inputs/null_sink.xml"
    holdsrtn = [1] # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]
    if rtn != 0:
        return # don't execute further commands

    output = tables.open_file("./output_temp.h5", mode = "r")
    paths = ["/Transactions", "/TransactedResources"] # this must contain tables to test
    #No resource exchange is expected
    for path in paths:
        yield assert_false, output.__contains__(path)

    output.close()
    os.remove("./output_temp.h5")
