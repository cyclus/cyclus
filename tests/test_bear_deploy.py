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

CWD = os.path.dirname(__file__)
INPUT = os.path.join(CWD, "input")

def test_bear_deploy():

    sim_input = os.path.join(INPUT, 'bears.json')
    sim_output = os.path.join(INPUT, 'bears.h5')

    if os.path.exists(sim_output):
        os.remove(sim_output)
    with open(sim_input, 'w') as f:
        json.dump(inputfile, f)

    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    holdsrtn = [1]  # needed because nose does not send() to test generator

    cmd = ["cyclus", "-o", sim_output, "--input-file", sim_input]
    yield check_cmd, cmd, ".", holdsrtn
    print(holdsrtn[0])
    print(holdsrtn)
    s = holdsrtn[0] 

    # test that the institution deploys a BearStore
    assert_in("New fac: BearStore", s)
    # test that the first agents exist with right minimum production.
    agents = re.compile('Agent \d+ 8\.0')
    all_agents = set(agents.findall(s))
    assert_greater_equal(len(all_agents), 9)

    if os.path.exists(sim_input):
        os.remove(sim_input)
    if os.path.exists(sim_output):
        os.remove(sim_output)

