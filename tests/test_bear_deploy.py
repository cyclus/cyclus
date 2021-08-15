import os
import re
import json
import subprocess

from nose.tools import assert_in, assert_true, assert_greater_equal
from tools import check_cmd


inputfile = {
 'simulation': {
  'archetypes': {
   'spec': [
    {'lib': 'bear_deploy', 'name': 'DemandFac'},
    {'lib': 'agents', 'name': 'NullRegion'},
    {'lib': 'bear_deploy', 'name': 'NOInst'},
   ],
  },
  'control': {'duration': 35, 'startmonth': 1, 'startyear': 2000},
  'facility': {
   'config': {
    'DemandFac': {
     'commodity': 'bears',
     'production_rate_max': 12,
     'production_rate_min': 8,
    },
   },
   'name': 'BearStore',
  },
  'region': {
   'config': {'NullRegion': '\n      '},
   'institution': {
    'config': {
     'NOInst': {
      'growth_commod': 'bears',
      'growth_rate': 0.1,
      'initial_demand': 20.0,
      'prototypes': {'val': 'BearStore'},
     },
    },
    'initialfacilitylist': {'entry': {'number': 2, 'prototype': 'BearStore'}},
    'name': 'SingleInstitution',
   },
   'name': 'SingleRegion',
  },
 },
}

INPUT = os.path.join(os.path.dirname(__file__), "input")


def test_bear_deploy():

    print( "IN BEAR")
    sim_input = os.path.join(INPUT, 'bears.json')
    sim_output = os.path.join(INPUT, 'bears.h5')
    print( "BEAR 1")

    if os.path.exists(sim_output):
        os.remove(sim_output)
    with open(sim_input, 'w') as f:
        json.dump(inputfile, f)
    print( "BEAR 2")
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    print( "BEAR 3")

    cmd = ["cyclus", "-o", sim_output, "--input-file", sim_input]
    s = [1]
    yield check_cmd, cmd, '.', s
    rtn = s[0]
    print(rtn)
    print(s)

    # s = subprocess.check_output(['cyclus', '-o', sim_output, sim_input],
    #                             universal_newlines=True, env=env)
    print( "BEAR 4")
    # test that the institution deploys a BearStore
    print( "BEAR 5")
    assert_in("New fac: BearStore", s)
    print( "BEAR 1")
    # test that the first agents exist with right minimum production.
    print( "BEAR 1")
    agents = re.compile('Agent \d+ 8\.0')
    print( "BEAR 1")
    all_agents = set(agents.findall(s))
    print( "BEAR 1")
    assert_greater_equal(len(all_agents), 9)
    print( "BEAR 1")
    if os.path.exists(sim_input):
        os.remove(sim_input)
    if os.path.exists(sim_output):
        os.remove(sim_output)
    print( "BEAR 1")

    print( "OUT BEAR")
