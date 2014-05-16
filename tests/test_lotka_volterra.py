#! /usr/bin/env python
from __future__ import print_function

import nose
from nose.tools import assert_equal, assert_almost_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
import hashlib

from tools import check_cmd
from helper import tables_exist, find_ids, exit_times, h5out, clean_outs

prey = "Prey"
pred = "Predator"

def agent_time_series(f, names):
    """Return a list of timeseries corresponding to the number of agents in a
    Cyclus simulation
    
    Parameters
    ----------
    f : PyTables file
        the output file
    names : list
        the list of agent names
    """
    names = [prey, pred]
    digests = [hashlib.sha1(name).digest() for name in names]
    print("digests:", digests)

    tbl = f.root.Info
    nsteps = tbl.cols.Duration[:][0]
    entries = {name: [0] * nsteps for name in names}
    exits = {name: [0] * nsteps for name in names}

    # Get specific tables and columns
    agent_entry = f.get_node("/AgentEntry")[:]

    # Find agent ids
    agent_ids = agent_entry["AgentId"]
    agent_type = agent_entry["Prototype"]
    prey_ids = find_ids(prey, agent_type, agent_ids)
    pred_ids = find_ids(pred, agent_type, agent_ids)

    # entries per timestep
    for x in agent_entry:
        name = None
        id = x['AgentId']
        if id in prey_ids:
            name = prey
        elif id in pred_ids:
            name = pred
        if name is not None:
            entries[name][x['EnterTime']] += 1
    
    # cumulative entries
    for k, v in entries.items():
        for i in range(len(v) - 1):
            v[i+1] += v[i]

    if not hasattr(f.root, 'AgentExit'):
        return entries
    
    # exits per timestep
    agent_exit = f.get_node("/AgentExit")[:]
    for x in agent_exit:
        name = None
        id = x['AgentId']
        if id in prey_ids:
            name = prey
        elif id in pred_ids:
            name = pred
        if name is not None:
            exits[name][x['ExitTime']] += 1

    # cumulative exits
    for k, v in exits.items():
        for i in range(len(v) - 1):
            v[i+1] += v[i]

    # return difference
    ret = {}
    for name in names:
        i = entries[name]
        # shift by one to account for agents that enter/exit in the same
        # timestep
        o = [0] + exits[name][:-1] 
        ret[name] = [i - o for i, o in zip(i, o)]
            
    return ret

def test_predator_only():
    """Tests simulations with Predators only.

    The population is expected to die off after a timestep.
    """
    clean_outs()

    # A reference simulation input for Lotka-Volterra simulation
    sim_input = "./input/predator.xml"

    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    
    print("Confirming valid Cyclus execution.")
    assert_equal(rtn, 0)

    output = tables.open_file(h5out, mode = "r")
    series = agent_time_series(output, [prey, pred])
    print("Prey:", series[prey], "Predators:", series[pred])
    
    prey_exp = [0 for n in range(10)]
    pred_exp = [1, 1] + [0 for n in range(8)]

    assert_equal(series[prey], prey_exp)
    assert_equal(series[pred], pred_exp)

    output.close()
    clean_outs()

def test_prey_only():
    """Tests simulations with Preys only.

    The population is expected to grow exponentially.
    """
    clean_outs()
    sim_input = "./input/prey.xml"
    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    
    print("Confirming valid Cyclus execution.")
    assert_equal(rtn, 0)

    output = tables.open_file(h5out, mode = "r")
    series = agent_time_series(output, [prey, pred])
    print("Prey:", series[prey], "Predators:", series[pred])
    
    prey_exp = [2**n for n in range(10)]
    pred_exp = [0 for n in range(10)]
    
    assert_equal(series[prey], prey_exp)
    assert_equal(series[pred], pred_exp)

    output.close()
    clean_outs()

def test_lotka_volterra():
    """Tests simulations with Preys and Predators

    Preys offer a resource representing itself. Predators acquire the resources
    of the preys. When the prey's resource gets accepted, it decommissions
    itself. Preys and predators have a fixed life expectancy. However, if
    a predator does not get any resource for several time steps,
    it must decommission itself as well.
    After certain time steps, predators and preys reproduce and deploy new
    predators and preys respectively.

    A single oscillation is expected and the peak of the predator population is
    expected to occur after the peak of the prey population.
    """
    clean_outs()
    sim_input = "./input/lotka_volterra_determ.xml"
    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    
    print("Confirming valid Cyclus execution.")
    assert_equal(rtn, 0)

    output = tables.open_file(h5out, mode = "r")
    series = agent_time_series(output, [prey, pred])
    print("Prey:", series[prey], "Predators:", series[pred])
    
    prey_max = series[prey].index(max(series[prey]))
    pred_max = series[pred].index(max(series[pred]))
    print("t_prey_max:", prey_max, "t_pred_max:", pred_max)
    
    assert_true(prey_max < pred_max)

    output.close()
    clean_outs()

if __name__ == "__main__":
    nose.runmodule()
