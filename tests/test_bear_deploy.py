import os
import re
import json
import subprocess
from tools import thread_count


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


def test_bear_deploy(thread_count):
    if os.path.exists('bears.h5'):
        os.remove('bears.h5')
    with open('bears.json', 'w') as f:
        json.dump(inputfile, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-j', thread_count, '-o', 'bears.h5', 'bears.json'],
                                universal_newlines=True, env=env)
    # test that the institution deploys a BearStore
    assert ("New fac: BearStore" in  s)
    # test that the first agents exist with right minimum production.
    agents = re.compile(r'Agent \d+ 8\.0')
    all_agents = set(agents.findall(s))
    assert (len(all_agents) >=  9)
    if os.path.exists('bears.json'):
        os.remove('bears.json')
    if os.path.exists('bears.h5'):
        os.remove('bears.h5')
