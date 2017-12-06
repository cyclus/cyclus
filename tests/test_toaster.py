import json
import subprocess
import os

from nose.tools import assert_in


INPUTFILE = {'simulation': {'archetypes': {'spec': [
                                        {'lib': 'toaster', 'name': 'DefaultToaster'},
                                        {'lib': 'agents', 'name': 'NullRegion'},
                                        {'lib': 'agents', 'name': 'NullInst'}
                                        ]},
                'control': {'duration': '2',
                            'startmonth': '1',
                            'startyear': '2000'},
                'facility': [{'config': {'DefaultToaster': {'level': 10.0}},
                              'name': 'Dummy'}],
                'region': {'config': {'NullRegion': None},
                           'institution': {'config': {'NullInst': None},
                                           'initialfacilitylist': {'entry': [{'number': '1',
                                                                              'prototype': 'Dummy'}]},
                                           'name': 'SingleInstitution'},
                           'name': 'SingleRegion'}}}


def test_pyagent_defaults():
    if os.path.exists('default-toaster.h5'):
        os.remove('default-toaster.h5')
    with open('default-toaster.json', 'w') as f:
        json.dump(INPUTFILE, f)
    env = dict(os.environ)
    env['PYTHONPATH'] = "."
    s = subprocess.check_output(['cyclus', '-o', 'default-toaster.h5', 'default-toaster.json'],
                                universal_newlines=True, env=env)
    # tests default value set on facility
    assert_in("Bread is rye", s)
    # tests that value in input file overrides default value
    assert_in("Toast level is 10", s)
    if os.path.exists('default-toaster.json'):
        os.remove('default-toaster.json')
    if os.path.exists('default-toaster.h5'):
        os.remove('default-toaster.h5')
