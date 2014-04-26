#! /usr/bin/env python

from nose.tools import assert_equal, assert_almost_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times

def test_minimal_cycle():
    """Tests simulations with Preys and Predators

    Preys offer a resource representing itself. Predators acquire the resources
    of the preys. When the prey's resource gets accepted, it decommissions
    itself. Preys and predators have a fixed life expectancy. However, if
    a predator does not get any resource for several time steps,
    it must decommission itself as well.
    After certain time steps, predators and preys reproduce and deploy new
    predators and preys respectively.

    Oscillating behavior is expected.
    """

    # A reference simulation input for Lotka-Volterra simulation
    sim_input = "./Inputs/lotka_volterra.xml"

    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands

    output = tables.open_file("./output_temp.h5", mode = "r")
    # tables of interest
    paths = ["/AgentEntry", "/Resources", "/Transactions", "/Info"]
    # Check if these tables exist
    yield assert_true, table_exist(output, paths)
    if not table_exist(output, paths):
        output.close()
        os.remove("./output_temp.h5")
        # This is a starter sqlite db created implicitly
        os.remove("./output_temp.sqlite")
        return  # don't execute further commands

    # Get specific tables and columns
    agent_entry = output.get_node("/AgentEntry")[:]
    info = output.get_node("/Info")[:]
    resources = output.get_node("/Resources")[:]
    transactions = output.get_node("/Transactions")[:]

    # Find agent ids
    agent_ids = agent_entry["AgentId"]
    agent_impl = agent_entry["Implementation"]
    agent_protos = agent_entry["Prototype"]
    duration = info["Duration"][0]

    # Track transacted resources
    quantities = resources["Quantity"]

    output.close()
    os.remove("./output_temp.h5")
    # This is a starter sqlite db created implicitly
    os.remove("./output_temp.sqlite")
